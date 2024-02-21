#pragma once

#include <string>

#include "serveractions.h"

class Commands {
    public:
        static inline constexpr std::string_view type = "commands";

        static void initializeComponent(Actions &actions);
        static Commands createInstance(auto &param, auto &creator) { return Commands{}; }
        static std::string execute_command(std::string command);
};
