#pragma once

#include <optional>
#include <functional>
#include <map>

#include "components.h"

class Instance;

struct InstanceEvents {
    using RuntimeInitFunc = std::function<bool(Instance &)>;
    using SetConfigFunc = std::function<bool(Instance &)>;

    static bool doNothing(Instance &) { return true; }

    RuntimeInitFunc init{ doNothing };
    SetConfigFunc setConf{ doNothing };
};

class Instance {
    public:

        Instance(InstanceEvents events);

        Instance(const Instance &other) = delete;
        Instance(Instance &&other) = default;

        Instance &operator=(const Instance &other) = delete;
        Instance &operator=(Instance &&other) = default;

        void createComponent(Control &controlDescription);

        bool setConfig(Config &conf);
        const Config &getConfig() const;

        const std::map<std::string, std::string> &getArguments() const;
        std::vector<std::string> getArgumentKeys() const;
        // TODO: maybe add possibility to specify type
        bool addArgument(const std::string &name);
        bool setArgumentValue(std::string name, std::string value);
        bool initRuntime();

    private:
        ComponentRegistry components;
        Config config;
        InstanceEvents events;
        std::map<std::string, std::string> arguments;
};