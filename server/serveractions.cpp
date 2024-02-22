#include "chaiscript/chaiscript.hpp"

#include "chaiscript/dispatchkit/boxed_cast.hpp"
#include "serveractions.h"

ServerActions::ServerActions() : runtime(std::make_unique<chaiscript::ChaiScript>()) { }

std::string ServerActions::eval(std::string_view script) {
    runtime->eval(script.data());
    return "Executed command";
}