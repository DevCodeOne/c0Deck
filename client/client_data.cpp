#include "client_data.h"

std::shared_ptr<Client> ClientData::getClientInstance() {
    return clientInstance;
}

std::string ClientData::getLogLevel() const {
    return logLevel;
}

std::string ClientData::getConfig() const {
    return config;
}
