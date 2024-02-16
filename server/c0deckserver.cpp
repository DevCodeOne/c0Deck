#include <cstdlib>
#include <cstdint>
#include <memory>
#include <latch>
#include <string>
#include <thread>
#include <csignal>

#include "service.grpc.pb.h"
#include "grpcpp/grpcpp.h"
#include "grpcpp/security/server_credentials.h"
#include "grpcpp/health_check_service_interface.h"
#include "grpcpp/ext/proto_server_reflection_plugin.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include "CLI/CLI.hpp"

namespace grpc {
    class GreeterServiceImpl : public Greeter::Service {
        Status doAction(ServerContext *context, const Request *request, Reply *reply) override {
            spdlog::debug("Got action : {}", request->action());
            return Status::OK;
        }
    };
}

std::unique_ptr<grpc::GreeterServiceImpl> serviceInstance = nullptr;
std::unique_ptr<grpc::Server> serverInstance = nullptr;
std::latch serverInitBarrier{ 1 };
auto serverLogger = spdlog::stdout_color_st("Server");
std::atomic_int signalStatus = 0;

void signalHandler(int signal) {
    switch (signal) {
        case SIGINT:
            serverLogger->debug("Received SIGINT signal");
            serverLogger->debug("Shutting down server ...");
            serverInstance->Shutdown();
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

    serverLogger->info("Server listening on : {}", serverAddress);
    serverInitBarrier.count_down(1);
    serverInstance->Wait();
}

int main(int argc, char *argv[]) {
    CLI::App app{"Starts c0DeckServer"};
    argv = app.ensure_utf8(argv);

    uint16_t serverPort = 9999;
    std::string listenTo = "0.0.0.0";
    bool verboseOutput = false;
    app.add_option("-p,--port", serverPort, "Port to start gRPC server on");
    app.add_option("-a,--adress", listenTo, "Adress to bind the server to");
    app.add_flag("-v", verboseOutput, "Enable verbose output");

    CLI11_PARSE(app, argc, argv);

    if (verboseOutput) {
        serverLogger->set_level(spdlog::level::debug);
    }

    std::jthread serverRunner{[listenTo, serverPort]() { runServer(listenTo, serverPort); }};

    serverLogger->debug("Before wait ..");
    serverInitBarrier.wait();

    std::signal(SIGINT, signalHandler);
    serverLogger->debug("Server is initialized");

    // Waiting for server thread to end
    if (serverRunner.joinable()) {
        serverRunner.join();
    }

    return EXIT_SUCCESS;
}