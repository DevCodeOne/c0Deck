import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.11

Rectangle {
    width: iconSize
    height: iconSize
    color: "transparent"
    border.color: Material.accent
    border.width: 5
    radius: 10
    Button {
        width: iconSize
        height: iconSize
        background: 
            AnimatedImage {
                id : animation
                source: "qrc:/ArrowAnimated.gif"
            }
    }
}