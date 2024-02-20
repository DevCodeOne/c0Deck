#pragma once

#include <string>
#include <vector>

#include "nlohmann/detail/macro_scope.hpp"
#include "nlohmann/json.hpp"

struct ServerConfig {
    std::string listenTo;
    unsigned int port;
    std::vector<std::string> modules;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ServerConfig, listenTo, port, modules);