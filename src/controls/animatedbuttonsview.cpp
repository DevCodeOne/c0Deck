#include "animatedbuttonsview.h"
#include "controls/componentcreator.h"

void ButtonControl::initializeComponent() {}

ButtonControl ButtonControl::createInstance(const Control &control, ComponentCreator &creator) {
    creator.createComponent(control.name, "ButtonView");
    return ButtonControl{};
}