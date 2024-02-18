#include "spdlog/spdlog.h"

#include "actions.h"

Actions::Actions(std::shared_ptr<Client> client, QObject *parent) 
    : QObject(parent), clientInstance(client) {}

void Actions::handleAction(const QString &msg) {
    if (clientInstance) {
        auto result = clientInstance->doAction(msg.toStdString());
        spdlog::debug("Action {} -> {}", msg.toStdString(), result);
    } else {
        spdlog::debug("Server is not initialized");
    }
}