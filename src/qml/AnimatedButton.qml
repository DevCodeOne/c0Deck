import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material
import QtQuick.Layouts

Rectangle {
    width: animation.width
    height: animation.height
    color: "transparent"
    border.color: Material.accent
    border.width: 5
    radius: 10
    Button {
        background: 
            AnimatedImage {
                width: iconSize
                height: iconSize
                sourceSize.width: iconSize
                sourceSize.height: iconSize
                id : animation
                source: "file:///home/chris/Programming/c0Deck/res/ArrowAnimated.gif"
            }
    }
}