
import QtQuick 2.15

import AudioDataTypes 1.0

Rectangle {
    id: container
    color: "transparent"

    signal doAction(action: string)

    required property int spacing
    required property int controlHeight
    required property string background
    required property AudioStreamList model

    AnimatedBackground {
        background: parent.background
    }

    GridView {
        id: streamList
        anchors.fill: parent
        cellWidth: container.width
        cellHeight: container.controlHeight + spacing * 0.5

        model: parent.model

        delegate : AudioStream {
            width: streamList.cellWidth
            height: container.controlHeight
            spacing: container.spacing

            Component.onCompleted: {
                doAction.connect(container.doAction);
            }
        }
    }
}