import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

Rectangle {

    color: "transparent"
    border.color: Material.accent
    border.width: 5
    radius: 10
    Button {

        width: parent.width
        height: parent.height
        background: 
            AnimatedImage {
                id: animated
                width: parent.width
                height: parent.height
                source: "qrc:/ArrowAnimated.gif"
            }
    }
}