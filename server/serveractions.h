#pragma once

#include <memory>
#include <string>
#include <string_view>

#include "chaiscript/chaiscript.hpp"


class Actions {
    public:
        Actions();
        Actions(const Actions &other) = delete;
        Actions(Actions &&other) = default;

        Actions &operator=(const Actions &other) = delete;
        Actions &operator=(Actions &&other) = default;

        template<typename Callable>
        void registerFunc(Callable callable, const std::string &funcName);
        std::string eval(std::string_view script);
    private:
        std::unique_ptr<chaiscript::ChaiScript> runtime = nullptr;
};

template<typename Callable>
void Actions::registerFunc(Callable callable, const std::string &funcName) {
    runtime->add(chaiscript::fun(callable), funcName);
}