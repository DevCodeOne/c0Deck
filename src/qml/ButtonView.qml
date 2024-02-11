import QtQuick 2.12
import QtQuick.Layouts 1.11

GridLayout {
    id: buttonLayout
    columnSpacing: 10
    rowSpacing: 10
    columns: 10

    AnimatedButton {
        id: "first"
        Layout.preferredWidth: iconSize
        Layout.preferredHeight: iconSize
    }

}