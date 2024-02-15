import QtQuick 2.15

import QmlDataTypes 1.0

GridView {

    id: buttonGrid

    signal doAction(action: string)

    required property int iconSize
    required property int spacing

    cellWidth: iconSize + spacing
    cellHeight: iconSize + spacing


    model: AnimatedButtonList {}

    delegate : AnimatedButton {
        TapHandler {
            onTapped: buttonGrid.doAction(action)
        }

        width: buttonGrid.cellWidth - spacing
        height: buttonGrid.cellHeight - spacing
    }

}