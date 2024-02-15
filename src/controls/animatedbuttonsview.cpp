#include <QQmlEngine>

#include "spdlog/spdlog.h"

#include "animatedbuttonsview.h"

void ButtonControl::initializeComponent() {
    qmlRegisterType<AnimatedButtonList>("QmlDataTypes", 1, 0, "AnimatedButtonList");
}

ButtonControl::ButtonControl() : 
    buttonList(std::make_unique<AnimatedButtonList>()), 
    actionHandler(std::make_unique<ButtonActionHandler>()) {
}

AnimatedButtonList *ButtonControl::getButtonList() {
    return buttonList.get();
}

const ButtonActionHandler *ButtonControl::getActionHandler() const {
    return actionHandler.get();
}

const AnimatedButtonList *ButtonControl::getButtonList() const {
    return buttonList.get();
}

void ButtonActionHandler::handleAction(const QString &msg) {
    spdlog::debug("Action {}", msg.toStdString());
}