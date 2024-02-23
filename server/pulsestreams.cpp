#include <chrono>
#include <future>
#include <pulse/mainloop.h>
#include <thread>

#include "pulse/context.h"
#include "pulse/def.h"
#include "pulse/introspect.h"
#include "pulse/mainloop-api.h"
#include "pulse/subscribe.h"

#include "pulsestreams.h"

PulseStreams::PulseStreams() : pulseRunner(PulseStreams::runThread, this) {
    serverLogger->debug("Pulsestreams instance");
}

void PulseStreams::subscribeCallback(pa_context *context, pa_subscription_event_type_t event,
                       uint32_t index, void *userData) {
    PulseStreams::serverLogger->debug("subscribeCallback");
    const auto maskedEvent = event & PA_SUBSCRIPTION_EVENT_FACILITY_MASK;
    switch (maskedEvent) {
    case PA_SUBSCRIPTION_EVENT_SINK_INPUT:
        if (maskedEvent == PA_SUBSCRIPTION_EVENT_REMOVE) {
        PulseStreams::serverLogger->debug("Removed sink with index {}", index);
        } else {
        PulseStreams::serverLogger->debug("Add sink with index {}", index);
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
    auto *thiz = reinterpret_cast<PulseStreams *>(userData);

    pa_proplist *proplist = pa_proplist_new();
    pa_proplist_sets(proplist, PA_PROP_APPLICATION_NAME, "c0Deck");
    pa_proplist_sets(proplist, PA_PROP_APPLICATION_ID, "noOrg.c0Deck");
    pa_proplist_sets(proplist, PA_PROP_APPLICATION_ICON_NAME, "audio-card");
    // TODO: create version of application in cmake
    pa_proplist_sets(proplist, PA_PROP_APPLICATION_VERSION, "0.1");

    pa_context *context = pa_context_new_with_proplist(api, nullptr, proplist);
    pa_proplist_free(proplist);
    pa_context_set_state_callback(context, contextCallback,
                                    reinterpret_cast<void *>(thiz));

    auto connectionResult =
        pa_context_connect(context, nullptr, PA_CONTEXT_NOFAIL, nullptr);

    if (connectionResult < 0) {
        if (pa_context_errno(context) == PA_ERR_INVALID) {
        }

        PulseStreams::serverLogger->debug("Couldn't connect to pulseaudio");
    }
}

void PulseStreams::runThread(std::stop_token token, PulseStreams *thiz) {
    using namespace std::chrono_literals;

    pa_mainloop *mainloop = pa_mainloop_new();
    pa_mainloop_api *api = pa_mainloop_get_api(mainloop);
    pa_mainloop_api_once(api, mainloopCallback, reinterpret_cast<void *>(thiz));

    auto mainloopRunner =
        std::jthread([mainloop, api]() { pa_mainloop_run(mainloop, 0); });

    while (!token.stop_requested()) {
        std::this_thread::sleep_for(500ms);
    }

    pa_mainloop_quit(mainloop, 0);

    if (mainloopRunner.joinable()) {
        mainloopRunner.join();
    }

    pa_mainloop_free(mainloop);
}