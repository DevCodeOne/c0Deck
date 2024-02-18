#pragma once

#include <memory>
#include <string>

#include "service.grpc.pb.h"
#include "grpcpp/grpcpp.h"

class Client {
    public:
        Client(std::shared_ptr<grpc::Channel> channel);

        std::string doAction(const std::string &actionStr);
    private:
        std::unique_ptr<Greeter::Stub> mChannel;
};