#pragma once

#include <string>
#include <string_view>

#include "nlohmann/detail/macro_scope.hpp"
#include "nlohmann/json.hpp"

#include "config.h"

struct WebControlConfig {
    std::string initialUrl;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(WebControlConfig, initialUrl);

class WebControl {
    public:
        static inline constexpr std::string_view type = "WebControl";

        static void initializeComponent();

        WebControl(const WebControl &other) = delete;
        WebControl(WebControl &&other) = default;
        ~WebControl() = default;

        WebControl &operator=(const WebControl &other) = delete;
        WebControl &operator=(WebControl &&other) = default;

        template<typename CreatorType>
        static WebControl createInstance(CreatorType &creator, const Control &control);

    private:
        WebControl() = default;
};

template<typename CreatorType>
WebControl WebControl::createInstance(CreatorType &creator, const Control &control) {
    typename CreatorType::PropertiesType properties{};

    auto config = control.params.get<WebControlConfig>();
    properties["initialUrl"] = config.initialUrl;

    creator.createComponent(control.name, "WebControl", properties);
    return WebControl{};
}