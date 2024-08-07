#pragma once

// TODO: add json schema validator

#include <cstdint>
#include <string_view>
#include <optional>

#include <nlohmann/json.hpp>

#include "json_utils.h"

enum struct Angle : uint16_t {
    _0,
    _90,
    _180,
    _270,
    landscape = _270,
    portrait = _0
};

NLOHMANN_JSON_SERIALIZE_ENUM(Angle, {
    {Angle::_0, "0"},
    {Angle::_90, "90"},
    {Angle::_180, "180"},
    {Angle::_270, "270"},
    {Angle::landscape, "landscape"},
    {Angle::portrait, "portrait"}
})

struct Screen {
    // absolute resolution width, max
    std::string width;
    // absolute resolution height, max
    std::string height;
    // angle values [0, 90, 180, 270], orientation landscape, portrait
    JsonOptional<Angle> rotation;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Screen, width, height);

struct Control {
    // Different control types
    std::string type;
    // Name of the tab
    std::string name;
    // Parameters for component type
    nlohmann::json params;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Control, type, name, params)

class Config {
    public:
        const std::string &getServer() const;

        const Screen &getScreenInfo() const;

        const std::vector<Control> &getControls() const;

        uint getIconSize() const;

    private:
        // Url, or ip + port
        std::string server;
        // absolute values, max
        Screen screen {"max", "max", Angle::_0};
        // Array of controls
        std::vector<Control> controls;
        
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Config, server, screen, controls)
};