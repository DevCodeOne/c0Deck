#include "client.h"

#include "spdlog/spdlog.h"

Client::Client(std::shared_ptr<grpc::Channel> channel) : mChannel(Greeter::NewStub(channel)) {}

std::string Client::doAction(const std::string &actionStr) {
    ActionRequest req;
    req.set_action(actionStr);
    ActionResult rep;
    grpc::ClientContext context;
    
    grpc::Status status = mChannel->doAction(&context, req, &rep);

    if (status.ok()) {
        return rep.result();
    } else {
        spdlog::debug("Server returned error");
    }

    return "";
   
}