#include "chaiscript/chaiscript.hpp"

#include "chaiscript/dispatchkit/boxed_cast.hpp"
#include "serveractions.h"

Actions::Actions() : runtime(std::make_unique<chaiscript::ChaiScript>()) { }

std::string Actions::eval(std::string_view script) {
    const auto result = runtime->eval(script.data());
    return chaiscript::boxed_cast<std::string>(result);
}