import QtQuick 2.15
import QtMultimedia 5.15

import QmlDataTypes 1.0

Rectangle {
    id: container
    color: "transparent"

    signal doAction(action: string)

    required property int iconSize
    required property int spacing
    required property string background
    required property AnimatedButtonList model

    AnimatedBackground { background : parent.background }

    GridView {
        id: buttonGrid
        anchors.fill: parent

        cellWidth: iconSize + spacing
        cellHeight: iconSize + spacing

        model: parent.model

        delegate : AnimatedButton {
            TapHandler {
                onTapped: container.doAction(action)
            }

            width: buttonGrid.cellWidth - spacing
            height: buttonGrid.cellHeight - spacing
        }
    }
}