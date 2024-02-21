#include "commands.h"

#include <cstdio>

void Commands::initializeComponent(Actions &actions) {
    actions.registerFunc(&Commands::execute_command, "exec");
}

std::string Commands::execute_command(std::string command) {
    system(command.c_str());
    return "";
}