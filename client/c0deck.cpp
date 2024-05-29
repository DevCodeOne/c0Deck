#include <cstdlib>
#include <fstream>

#include "grpcpp/grpcpp.h"

#include "nlohmann/json.hpp"

#include "spdlog/spdlog.h"

#include "application_entry.h"
#include "client_types.h"
#include "client.h"

int main(int argc, char *argv[]) {
    InstanceEvents events {
        .init = {
            [](Instance &instance) {
                spdlog::set_level(spdlog::level::debug);

                const std::string loglevel = instance.getData().getLogLevel();

                const auto &configPath = instance.getData().getConfig();
                spdlog::debug("Config path : {}, Working dir : {}",
                              configPath,
                              std::filesystem::current_path().string());

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
                return true;
            }
        },
        .setConf = {
            [](Instance &instance) {
                const auto &config = instance.getConfig();
                spdlog::debug("Set config");

                spdlog::debug("Set server {} as target", config.getServer());

                auto clientInstance = std::make_shared<Client>(grpc::CreateChannel(config.getServer(), 
                                                                grpc::InsecureChannelCredentials()));

                instance.setData(clientInstance, DataType<ClientDataType::ClientInstance>{});
                return true;
            }
        }
    };

    Instance instance{events};

    MainWindow::initialize(argc, argv, instance);
    return EXIT_SUCCESS;
}