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
        anchors.centerIn: parent
        width: parent.width - parent.border.width * 2
        height: parent.height- parent.border.width * 2
        source: icon
    }
}