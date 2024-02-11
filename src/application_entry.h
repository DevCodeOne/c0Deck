#pragma once

#include <functional>

#include "instance.h"

class QCommandLineParser;

class MainWindow {
    public:
        static bool initialize(int argc, char *argv[], Instance &instance);
    private:
};
