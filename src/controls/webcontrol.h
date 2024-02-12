#pragma once

#include <string>
#include <string_view>

#include "config.h"
#include "controls/componentcreator.h"

class WebControl {
    public:
        static inline constexpr std::string_view type = "WebControl";

        static void initializeComponent();

        WebControl(const WebControl &other) = delete;
        WebControl(WebControl &&other) = default;
        ~WebControl() = default;

        WebControl &operator=(const WebControl &other) = delete;
        WebControl &operator=(WebControl &&other) = default;

        static WebControl createInstance(const Control &control, ComponentCreator &creator);

    private:
        WebControl() = default;
};