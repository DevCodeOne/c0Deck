#pragma once

#include <string_view>
#include <thread>
#include <map>
#include <string>
#include <memory>

#include "spdlog/spdlog.h"

#include "pulse/mainloop-api.h"
#include "pulse/introspect.h"

struct PulseStreamInfo {
    std::string name;
    float volume;
};

class PulseStreams {
    public:
        PulseStreams(const PulseStreams &other) = delete;
        PulseStreams(PulseStreams &&other) = default;

        PulseStreams &operator=(const PulseStreams &other) = delete;
        PulseStreams &operator=(PulseStreams &&other) = default;


        static inline constexpr std::string_view type = "pulsestreams";

        static void initializeComponent(auto &instance) { 
            serverLogger = instance.getData().serverLogger; 
            serverLogger->debug("initializedComponent : {}", type);
        }

        static PulseStreams createInstance(auto &param, auto &creator) { 
            serverLogger->debug("Creating instance of pulsestreams");
            return PulseStreams{}; 
        }

    private:
        struct PulseStreamsData {
            std::map<std::string, PulseStreamInfo> streams{};
            std::jthread pulseRunner{};
        };

        PulseStreams();

        static void runThread(std::stop_token token, PulseStreamsData *thiz);
        static void mainloopCallback(pa_mainloop_api *api, void *userData);
        static void contextCallback(pa_context *context, void *userData);
        static void sinkCallback(pa_context *context, const pa_sink_input_info *sinkInfo, int size, void *userData);
        static void subscribeCallback(pa_context *context, pa_subscription_event_type_t event, uint32_t index, void *userData);
        static void subscribeSinkCallback(pa_context *c, const pa_sink_info *i, int eol, void *userdata);
        static void subscribeSinkInputCallback(pa_context *context, const pa_sink_input_info *sinkInputInfo, int eol, void *userData);

        static inline std::shared_ptr<spdlog::logger> serverLogger;
        std::unique_ptr<PulseStreamsData> threadData;
};