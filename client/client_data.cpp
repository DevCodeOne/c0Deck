#include "client_data.h"

std::shared_ptr<Client> ClientData::getClientInstance() {
    return clientInstance;
}
