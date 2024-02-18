#pragma once

#include <variant>
#include <string>
#include <filesystem>

#include "components.h"
#include "instance.h"
#include "config.h"
#include "client_data.h"
#include "build_config.h"

using ComponentRegistry = Detail::ComponentRegistry<Components<ComponentList>>;
using Instance = Detail::Instance<ComponentRegistry, Config, ClientData>;
using InstanceEvents = Detail::InstanceEvents<Instance>;
using ComponentProperties = std::map<std::string, std::variant<std::string, std::filesystem::path, int, unsigned int>>;