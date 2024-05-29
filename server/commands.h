#pragma once

#include <string>
#include <mutex>
#include <vector>

#include "spdlog/spdlog.h"

#include "serveractions.h"

struct Process {
    std::string command;
    std::future<int> returnCode;
    pid_t processId;
};

// TODO: create class for process creation and checking, especially for platform dependent code
class Commands {
    public:
        static inline constexpr std::string_view type = "commands";

        static void initializeComponent(auto &instance) { 
            serverLogger = instance.getData().serverLogger;
            initializeComponent(instance.getData().actions); 

            serverLogger->debug("initializedComponent : {}", type);
        }

        static void initializeComponent(ServerActions &actions);
        static Commands createInstance(auto &param, auto &creator) { return Commands{}; }
        static int execute_command(std::string command, bool postResults);

    private:
        static inline std::shared_ptr<spdlog::logger> serverLogger;
        static inline std::vector<Process> processes;
        static inline std::mutex processesMutex;
        static inline std::jthread processChecker;
};
