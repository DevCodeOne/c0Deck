#include <algorithm>
#include <chrono>
#include <future>
#include <thread>
#include <utility>

#include <pulse/mainloop.h>
#include <pulse/context.h>
#include <pulse/def.h>
#include <pulse/introspect.h>
#include <pulse/mainloop-api.h>
#include <pulse/subscribe.h>

#include "pulsestreams.h"

static auto toPercentage(const pa_cvolume &volume) {
    const double avgVolume = pa_cvolume_avg(&volume);
    return std::clamp<uint8_t>(static_cast<uint8_t>((avgVolume * 100.0 + static_cast<double>(PA_VOLUME_NORM / 2.0)) / static_cast<double>(PA_VOLUME_NORM)),
                               static_cast<uint8_t>(0),
                               static_cast<uint8_t>(100));
}

PulseStreams::PulseStreams() : threadData(std::make_unique<PulseStreamsData>()) {
    threadData->pulseRunner = std::jthread(PulseStreams::runThread, threadData.get());
}

void PulseStreams::subscribeSinkCallback(pa_context *context, const pa_sink_info *sinkInfo, int eol, void *userData) {
    if (eol < 0 || sinkInfo == nullptr) {
        PulseStreams::serverLogger->debug("Sink callback error");
        return;
    }

    PulseStreams::serverLogger->debug("Name for stream : {} with volume : {}", sinkInfo->name, toPercentage(sinkInfo->volume));
}

void PulseStreams::subscribeSinkInputCallback(pa_context *context, const pa_sink_input_info *sinkInputInfo, int eol, void *userData) {
    if (eol < 0 || sinkInputInfo == nullptr || userData == nullptr) {
        // PulseStreams::serverLogger->debug("Sink info callback error");
        return;
    }

    if (sinkInputInfo->name == nullptr) {
        return;
    }

    auto &threadData = *reinterpret_cast<PulseStreamsData *>(userData);
    const auto avgVolume = static_cast<float>(toPercentage(sinkInputInfo->volume));

    if (auto stream = threadData.streams.find(sinkInputInfo->name); stream != threadData.streams.end()) {
        PulseStreams::serverLogger->debug("Found stream with name : {} with volume : {}", sinkInputInfo->name, avgVolume);
        stream->second.volume = avgVolume;
    } else {
        PulseStreams::serverLogger->debug("Added stream with name : {} with volume : {}", sinkInputInfo->name, avgVolume);
        threadData.streams.emplace(sinkInputInfo->name, PulseStreamInfo{sinkInputInfo->name, avgVolume});
    }
}

void PulseStreams::subscribeCallback(pa_context *context, pa_subscription_event_type_t event,
                       uint32_t index, void *userData) {
    PulseStreams::serverLogger->debug("subscribeCallback");
    const auto facility = event & PA_SUBSCRIPTION_EVENT_FACILITY_MASK;
    const auto eventType = event & PA_SUBSCRIPTION_EVENT_TYPE_MASK;
    switch (facility) {
    case PA_SUBSCRIPTION_EVENT_SINK:
        if (eventType == PA_SUBSCRIPTION_EVENT_REMOVE) {
            PulseStreams::serverLogger->debug("Removed sink with index {}", index);
        } else {
            PulseStreams::serverLogger->debug("Add sink with index {}", index);
            pa_operation *op = pa_context_get_sink_info_by_index(context,
                                                                 index,
                                                                 PulseStreams::subscribeSinkCallback,
                                                                 userData);
            if (op == nullptr) {
                PulseStreams::serverLogger->debug("Can't get sink info with index : {}", index);
                return;
            }
            pa_operation_unref(op);
        }
    break;
    case PA_SUBSCRIPTION_EVENT_SINK_INPUT:
        if (eventType == PA_SUBSCRIPTION_EVENT_REMOVE) {
            PulseStreams::serverLogger->debug("Removed sink input with index {}", index);
        } else {
            PulseStreams::serverLogger->debug("Add sink input with index {}", index);
            pa_operation *op = pa_context_get_sink_input_info(context,
                                                              index,
                                                              PulseStreams::subscribeSinkInputCallback,
                                                              userData);
            if (op == nullptr) {
                PulseStreams::serverLogger->debug("Can't get sink input info with index : {}", index);
                return;
            }
            pa_operation_unref(op);
        }
    }
}

void PulseStreams::sinkCallback(pa_context *context, const pa_sink_input_info *sinkInfo,
                  int size, void *userData) {
    PulseStreams::serverLogger->debug("sinkCallback");
    if (size < 0 || sinkInfo == nullptr) {
        if (pa_context_errno(context) == PA_ERR_NOENTITY) { }

        PulseStreams::serverLogger->debug("sinkCallback error");

        return;
    }

    if (sinkInfo->proplist != nullptr) {
        const char *applicationName = pa_proplist_gets(sinkInfo->proplist, PA_PROP_APPLICATION_NAME);

        PulseStreams::serverLogger->debug("application name : {}, stream name : {}", applicationName, sinkInfo->name);
    }
}

pa_context *context = nullptr;

void PulseStreams::contextCallback(pa_context *c, void *userData) {
    PulseStreams::serverLogger->debug("contextCallback");
    switch (pa_context_get_state(c)) {
        case PA_CONTEXT_UNCONNECTED:
            PulseStreams::serverLogger->debug("Context is not connected");
        break;
        case PA_CONTEXT_CONNECTING:
            PulseStreams::serverLogger->debug("Context is connecting");
        break;
        case PA_CONTEXT_AUTHORIZING:
            PulseStreams::serverLogger->debug("Context is authorizing");
        break;
        case PA_CONTEXT_SETTING_NAME:
        break;
        case PA_CONTEXT_READY: {
            PulseStreams::serverLogger->debug("Context is ready");
            pa_operation *op = nullptr;
            pa_context_set_subscribe_callback(c, subscribeCallback, userData);

            if (!(op = pa_context_subscribe(c,
                                            static_cast<pa_subscription_mask_t>(
                                                PA_SUBSCRIPTION_MASK_SINK_INPUT),
                                            nullptr, userData))) {
                PulseStreams::serverLogger->debug(
                    "Couldn't create pulseaudio subscribe callback");
            }
            pa_operation_unref(op);

            if (!(op = pa_context_get_sink_input_info_list(c, sinkCallback, userData))) {
                PulseStreams::serverLogger->debug("Couldn't get all sinks");
            }
            pa_operation_unref(op);
            }
        break;
        case PA_CONTEXT_FAILED: {
            // This is global context instead of the callback one
            pa_context_unref(c);
            context = nullptr;
            break;
        }
        case PA_CONTEXT_TERMINATED:
        default:
        break;
    }
}

void PulseStreams::mainloopCallback(pa_mainloop_api *api, void *userData) {
    PulseStreams::serverLogger->debug("mainloopCallback");

    pa_proplist *proplist = pa_proplist_new();
    pa_proplist_sets(proplist, PA_PROP_APPLICATION_NAME, "c0Deck");
    pa_proplist_sets(proplist, PA_PROP_APPLICATION_ID, "noOrg.c0Deck");
    pa_proplist_sets(proplist, PA_PROP_APPLICATION_ICON_NAME, "audio-card");
    // TODO: create version of application in cmake
    pa_proplist_sets(proplist, PA_PROP_APPLICATION_VERSION, "0.1");

    pa_context *context = pa_context_new_with_proplist(api, nullptr, proplist);
    pa_proplist_free(proplist);
    pa_context_set_state_callback(context, contextCallback,
                                    userData);

    auto connectionResult =
        pa_context_connect(context, nullptr, PA_CONTEXT_NOFAIL, nullptr);

    if (connectionResult < 0) {
        if (pa_context_errno(context) == PA_ERR_INVALID) {
        }

        PulseStreams::serverLogger->debug("Couldn't connect to pulseaudio");
    }
}

void PulseStreams::runThread(std::stop_token token, PulseStreamsData *data) {
    using namespace std::chrono_literals;

    pa_mainloop *mainloop = pa_mainloop_new();
    pa_mainloop_api *api = pa_mainloop_get_api(mainloop);
    pa_mainloop_api_once(api, mainloopCallback, reinterpret_cast<void *>(data));

    auto mainloopRunner =
        std::jthread([mainloop, api]() {
            pa_mainloop_run(mainloop, nullptr);
        });

    while (!token.stop_requested()) {
        std::this_thread::sleep_for(500ms);
    }

    pa_mainloop_quit(mainloop, 0);

    if (mainloopRunner.joinable()) {
        mainloopRunner.join();
    }

    pa_mainloop_free(mainloop);
}