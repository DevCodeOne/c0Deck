#include "instance.h"

Instance::Instance(InstanceEvents events) : events(events) {}

bool Instance::setConfig(Config &conf) {
    config = conf;
    // TODO: load config and check for errors
    if (!events.setConf(*this)) { return false; }

    return true;
}

const Config &Instance::getConfig() const {
    return config;
}

const std::map<std::string, std::string> &Instance::getArguments() const {
    return arguments;
}

std::vector<std::string> Instance::getArgumentKeys() const {
    std::vector<std::string> keys;
    for (const auto &[currentKey, _] : arguments) {
        keys.push_back(currentKey);
    }

    return keys;
}

bool Instance::addArgument(const std::string &name) {
    if (arguments.contains(name)) {
        return false;
    }

    arguments[name] = "";

    return true;
}

bool Instance::setArgumentValue(std::string name, std::string value) {
    if (!arguments.contains(name)) {
        return false;
    }

    arguments[name] = value;

    return true;
}

bool Instance::initRuntime() {
    return events.init(*this);
}
