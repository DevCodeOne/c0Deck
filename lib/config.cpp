#include "config.h"

const std::string &Config::getServer() const {
    return server;
}

const std::vector<Control> &Config::getControls() const {
    return controls;
}