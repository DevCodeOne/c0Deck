#include <fstream>
#include <cstdlib>

#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"

#include "application_entry.h"
#include "instance.h"

int main(int argc, char *argv[]) {
    InstanceEvents events {
        .init = {
            [](Instance &instance) {
                spdlog::set_level(spdlog::level::debug);

                const auto &copyMap = instance.getArguments();
                if (copyMap.contains("loglevel")) {
                    const std::string loglevel = copyMap.at("loglevel");
                }

                if (copyMap.contains("config")) {
                    const auto &configPath = copyMap.at("config");

                    try {
                        std::ifstream configInput(configPath);
                        auto loadedConfig = nlohmann::json::parse(configInput);
                        spdlog::debug("Loaded config at {} with contents : {}", configPath, loadedConfig.dump());

                        Config config = loadedConfig.template get<Config>();
                        instance.setConfig(config);
                    } catch (std::exception &e) {
                        spdlog::error("Couldn't open config file at {}, cause: {}", configPath ,e.what());
                        // Couldn't load config
                        return false;
                    }
                }
                return true;
            }
        },
        .setConf = {
            [](Instance &instance) {
                const auto &config = instance.getConfig();
                spdlog::debug("Set config");

                spdlog::debug("Set server {} as target", config.getServer());
                return true;
            }
        }
    };

    Instance instance{events};
    instance.addArgument("config");
    instance.addArgument("loglevel");

    MainWindow::initialize(argc, argv, instance);
    return EXIT_SUCCESS;
}