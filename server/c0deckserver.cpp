#include <cstdlib>
#include <cstdint>
#include <memory>
#include <latch>
#include <string>
#include <thread>
#include <tuple>
#include <csignal>

#include "pulsestreams.h"
#include "service.grpc.pb.h"
#include "grpcpp/grpcpp.h"
#include "grpcpp/security/server_credentials.h"
#include "grpcpp/health_check_service_interface.h"
#include "grpcpp/ext/proto_server_reflection_plugin.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include "CLI/CLI.hpp"

#include "components.h"
#include "instance.h"

#include "serverconfig.h"
#include "pulsestreams.h"
#include "serveractions.h"
#include "commands.h"

struct UserData {
    std::shared_ptr<spdlog::logger> serverLogger = spdlog::stdout_color_mt("Server");
    ServerActions actions{};
};

using ComponentRegistry = Detail::ComponentRegistry<Components<std::tuple<Commands, PulseStreams>>>;
using InstanceType = Detail::Instance<ComponentRegistry, ServerConfig, UserData>;
using InstanceEventType = Detail::InstanceEvents<InstanceType>;

std::latch serverInitBarrier{ 1 };
InstanceType instance(InstanceEventType {});
std::atomic_int shutdown = 0;

namespace grpc {
    class GreeterServiceImpl : public Greeter::Service {
        Status doAction(ServerContext *context, const ActionRequest *request, ActionResult *reply) override {
            instance.getData().serverLogger->debug("Got action : {}", request->action());
            try {
                const auto result = instance.getData().actions.eval(request->action());
                instance.getData().serverLogger->debug("Action result : {}", result);
                reply->set_result(result.c_str());
            } catch (std::exception &e) {
                instance.getData().serverLogger->debug("Exception occured when trying to exec command : {}", e.what(), request->action());
            }
            return Status::OK;
        }
    };
}

std::unique_ptr<grpc::GreeterServiceImpl> serviceInstance = nullptr;
std::unique_ptr<grpc::Server> serverInstance = nullptr;

void signalHandler(int signal) {
    switch (signal) {
        case SIGINT:
            shutdown = 1;
            break;
        case SIGHUP:
            // TODO: reload config
            break;
    }
    return;
}

void runServer(const std::string &listenTo, uint16_t port) {
    std::signal(SIGINT, SIG_IGN);
    using namespace grpc;

    std::string serverAddress = fmt::format("{}:{}", listenTo, port);
    serviceInstance = std::make_unique<GreeterServiceImpl>();

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;
    builder.AddListeningPort(serverAddress, grpc::InsecureServerCredentials());
    builder.RegisterService(serviceInstance.get());
    serverInstance = builder.BuildAndStart();

    instance.getData().serverLogger->info("Server listening on : {}", serverAddress);
    serverInitBarrier.count_down(1);
    serverInstance->Wait();
}

int main(int argc, char *argv[]) {
    CLI::App app{"Starts c0DeckServer"};
    argv = app.ensure_utf8(argv);

    uint16_t serverPort = 9999;
    bool verboseOutput = false;
    std::string configPath = "config.json";
    app.add_option("-c,--config", configPath, "Path to config file");
    app.add_flag("-v", verboseOutput, "Enable verbose output");

    CLI11_PARSE(app, argc, argv);

    if (verboseOutput) {
        instance.getData().serverLogger->set_level(spdlog::level::debug);
    }

    std::ifstream configInput(configPath);
    auto loadedConfig = nlohmann::json::parse(configInput);
    const auto serverConf = loadedConfig.get<ServerConfig>();

    ComponentRegistry registry;
    registry.initializeComponents(instance);

    for (const auto &currentModule : serverConf.modules) {
        registry.createInstance(currentModule, [](){}, instance);
    }

    std::jthread serverRunner{[&serverConf]() { runServer(serverConf.listenTo, serverConf.port); }};

    instance.getData().serverLogger->debug("Before wait ..");
    serverInitBarrier.wait();

    std::signal(SIGINT, signalHandler);
    instance.getData().serverLogger->debug("Server is initialized");

    while (!shutdown) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    instance.getData().serverLogger->debug("Received SIGINT signal");
    instance.getData().serverLogger->debug("Shutting down server ...");
    serverInstance->Shutdown();

    // Waiting for server thread to end
    if (serverRunner.joinable()) {
        serverRunner.join();
    }

    return EXIT_SUCCESS;
}