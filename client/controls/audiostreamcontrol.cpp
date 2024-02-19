#include <QQmlEngine>

#include "audiostreamcontrol.h"

void AudioStreamControl::initializeComponent() { 
    qmlRegisterType<AudioStreamList>("AudioDataTypes", 1, 0, "AudioStreamList");
}

const Actions *AudioStreamControl::getActionHandler() const { return actionHandler.get(); }

const AudioStreamList *AudioStreamControl::getStreamList() const { return streamList.get(); }

AudioStreamList *AudioStreamControl::getStreamList() { return streamList.get(); }
