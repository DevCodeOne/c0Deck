import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

Rectangle {

    required property string action
    required property string icon

    color: "transparent"
    border.color: Material.accent
    border.width: 5
    radius: 10

    AnimatedImage {
        id: animated
        width: parent.width
        height: parent.height
        source: icon
    }
}