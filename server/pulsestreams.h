#pragma once

#include <string_view>
#include <thread>
#include <memory>

#include "spdlog/spdlog.h"

#include "pulse/mainloop-api.h"
#include "pulse/introspect.h"

class PulseStreams {
    public:

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
        PulseStreams();

        static void runThread(std::stop_token token, PulseStreams *thiz);

        static inline std::shared_ptr<spdlog::logger> serverLogger;
        std::jthread pulseRunner;

        static void mainloopCallback(pa_mainloop_api *api, void *userData);
        static void contextCallback(pa_context *context, void *userData);
        static void sinkCallback(pa_context *context, const pa_sink_input_info *sinkInfo, int size, void *userData);
        static void subscribeCallback(pa_context *context, pa_subscription_event_type_t event, uint32_t index, void *userData);
};