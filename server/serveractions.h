#pragma once

#include <memory>
#include <string>
#include <string_view>

#include "chaiscript/chaiscript.hpp"


class ServerActions {
    public:
        ServerActions();
        ServerActions(const ServerActions &other) = delete;
        ServerActions(ServerActions &&other) = default;

        ServerActions &operator=(const ServerActions &other) = delete;
        ServerActions &operator=(ServerActions &&other) = default;

        template<typename Callable>
        void registerFunc(Callable callable, const std::string &funcName);
        std::string eval(std::string_view script);
    private:
        std::unique_ptr<chaiscript::ChaiScript> runtime = nullptr;
};

template<typename Callable>
void ServerActions::registerFunc(Callable callable, const std::string &funcName) {
    runtime->add(chaiscript::fun(callable), funcName);
}