#include <QQmlEngine>

#include "spdlog/spdlog.h"

#include "animatedbuttonsview.h"

void ButtonControl::initializeComponent() {
    qmlRegisterType<AnimatedButtonList>("QmlDataTypes", 1, 0, "AnimatedButtonList");
}

AnimatedButtonList *ButtonControl::getButtonList() {
    return buttonList.get();
}

const Actions *ButtonControl::getActionHandler() const {
    return actionHandler.get();
}

const AnimatedButtonList *ButtonControl::getButtonList() const {
    return buttonList.get();
}

