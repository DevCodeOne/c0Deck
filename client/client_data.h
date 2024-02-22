#pragma once

#include "client.h"

enum struct ClientDataType {
    ClientInstance, LogLevel, Config
};

template<ClientDataType Type> 
struct DataType { };

class ClientData {
    public:
        template<typename T, ClientDataType Type>
        void setData(T data, const DataType<Type> &param) { }
        template<typename T>
        void setData(T data, const std::string &param);

        std::shared_ptr<Client> getClientInstance();
        std::string getLogLevel() const;
        std::string getConfig() const;
    private:
        std::shared_ptr<Client> clientInstance;
        std::string logLevel;
        std::string config;
};

template<>
inline void ClientData::setData<std::shared_ptr<Client>, ClientDataType::ClientInstance>(
    std::shared_ptr<Client> instance, const DataType<ClientDataType::ClientInstance> &) {
    clientInstance = std::move(instance);
}

template<>
inline void ClientData::setData<std::string, ClientDataType::LogLevel>(
    std::string level, const DataType<ClientDataType::LogLevel> &) {
    logLevel = std::move(level);
}

template<>
inline void ClientData::setData<std::string, ClientDataType::Config>(
    std::string conf, const DataType<ClientDataType::Config> &) {
    config = std::move(conf);
}

template<typename T>
void ClientData::setData(T data, const std::string &param) {
    if (param == "loglevel") {
        setData(data, DataType<ClientDataType::LogLevel>{});
    } else if (param == "config") {
        setData(data, DataType<ClientDataType::Config>{});
    }  else if (param == "clientInstance") {
        setData(data, DataType<ClientDataType::ClientInstance>{});
    }
}