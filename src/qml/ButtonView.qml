import QtQuick 2.15

import QmlDataTypes 1.0

GridView {

    id: buttonGrid

    required property int iconSize
    required property int spacing

    cellWidth: 65
    cellHeight: 65

    model: AnimatedButtonList {}

    delegate : AnimatedButton {
        width: buttonGrid.cellWidth - 8
        height: buttonGrid.cellHeight - 8
    }

}