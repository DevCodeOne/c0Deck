#pragma once

#include <utility>

template<bool Value>
struct GenerateIf {
    static void call(auto callable, auto&&... args) {
        callable(std::forward<decltype(args)>(args) ...);
    }
};

template<>
struct GenerateIf<false> {
    static void call(auto callable, auto&&... args) { }
};