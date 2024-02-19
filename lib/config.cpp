#include "config.h"

const std::string &Config::getServer() const {
    return server;
}

const Screen &Config::getScreenInfo() const {
    return screen;
}

const std::vector<Control> &Config::getControls() const {
    return controls;
}