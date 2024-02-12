#pragma once

#include <functional>
#include <utility>
#include <string>

class ComponentCreator {
    public:
        void createComponent(const std::string &title, const std::string &componentName);
};