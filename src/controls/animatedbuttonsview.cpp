#include "animatedbuttonsview.h"

#include <QQmlEngine>

void ButtonControl::initializeComponent() {
    qmlRegisterType<AnimatedButtonList>("QmlDataTypes", 1, 0, "AnimatedButtonList");
}

ButtonControl::ButtonControl() : buttonList(std::make_unique<AnimatedButtonList>()) {
}

AnimatedButtonList *ButtonControl::getButtonList() {
    return buttonList.get();
}

const AnimatedButtonList *ButtonControl::getButtonList() const {
    return buttonList.get();
}