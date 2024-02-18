#pragma once

#include <memory>

#include <QObject>

#include "client.h"

class Actions : public QObject {
    Q_OBJECT
    public:
        Actions(std::shared_ptr<Client> client, QObject *parent = nullptr);
    public slots:
        void handleAction(const QString &actionStr);
    private:
        std::shared_ptr<Client> clientInstance;

};