#pragma once

#include <functional>
#include <string>

namespace Detail {

// TODO: Prevent triggering the same event from within
template<typename InstanceType>
struct InstanceEvents {
    using RuntimeInitFunc = std::function<bool(InstanceType &)>;
    using SetConfigFunc = std::function<bool(InstanceType &)>;

    static bool doNothing(InstanceType &) { return true; }

    RuntimeInitFunc init{ doNothing };
    SetConfigFunc setConf{ doNothing };
};

// TODO: Add possibility to add userdata via enum 
// std::tuple, std::map with string or enum ?
// Special Dataclass ?
template<typename ComponentRegistry, typename ConfigType, typename UserData>
class Instance {
    public:

        using ComponentRegistryType = ComponentRegistry;
        using Config = ConfigType;

        Instance(InstanceEvents<Instance> events);

        Instance(const Instance &other) = delete;
        Instance(Instance &&other) = default;

        Instance &operator=(const Instance &other) = delete;
        Instance &operator=(Instance &&other) = default;

        bool setConfig(Config &conf);
        const Config &getConfig() const { return config; }

        // TODO: maybe add possibility to specify type
        bool initRuntime() { return events.init(*this); }

        // TODO: maybe add event for specific userdata
        template<typename T, typename ParamType>
        void setData(T &data, const ParamType &param) { userData.setData(data, param); }

        UserData &getData() { return userData; }
        const UserData &getData() const { return userData; }

        ComponentRegistry &getComponentRegistry() { return components; }

    private:
        ComponentRegistry components;
        Config config;
        InstanceEvents<Instance> events;
        UserData userData;
};

template<typename ComponentRegistry, typename Config, typename UserData>
Instance<ComponentRegistry, Config, UserData>::Instance(InstanceEvents<Instance> events) : events(events) {}

template<typename ComponentRegistry, typename Config, typename UserData>
bool Instance<ComponentRegistry, Config, UserData>::setConfig(Config &conf) {
    config = conf;
    // TODO: load config and check for errors
    if (!events.setConf(*this)) { return false; }

    return true;
}

}