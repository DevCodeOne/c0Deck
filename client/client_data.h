#pragma once

#include <cstdint>

#include "client.h"

enum struct ClientDataType : uint8_t {
    ClientInstance
};

template<ClientDataType Type> 
struct DataType { };

class ClientData {
    public:
        template<typename T, ClientDataType Type>
        void setData(T data, const DataType<Type> &param);
        std::shared_ptr<Client> getClientInstance();
    private:
        std::shared_ptr<Client> clientInstance;
};

template<>
inline void ClientData::setData<std::shared_ptr<Client>, ClientDataType::ClientInstance>(
    std::shared_ptr<Client> instance, 
    const DataType<ClientDataType::ClientInstance> &) {
    clientInstance = instance;
}