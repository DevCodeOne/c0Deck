#include <QtWebEngine>

#include "webcontrol.h"

void WebControl::initializeComponent() {
    QtWebEngine::initialize();
}

WebControl WebControl::createInstance(const Control &control, ComponentCreator &creator) {
    creator.createComponent(control.name, "ButtonView");
    return WebControl{};
}