#pragma once

#include <functional>

#include "client_types.h"

class QCommandLineParser;

class MainWindow {
    public:
        static bool initialize(int argc, char *argv[], Instance &instance);
    private:
};
