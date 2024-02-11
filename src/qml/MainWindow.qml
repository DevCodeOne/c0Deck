import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.11
import QtQml.Models 2.1
import QtWebEngine 1.3

ApplicationWindow {
    id: window
    visible: true
    width: initialWidth
    height: initialHeight
    title: "c0Deck"

    Material.theme: Material.Dark
    Material.accent: Material.Cyan

    Item {
        id: base
        transform: [
            Rotation {
                angle: landScape ? 270 : 0
            },
            Translate {
                x: 0
                y: landScape ? initialHeight : 0
            }
        ]
        width: initialWidth
        height: initialHeight
        x: 0
        y: 0

        ColumnLayout {
            width: parent.width
            height: parent.height
            spacing: 0
            TabBar {
                Layout.fillWidth : true
                id: mainTabs
                position: TabBar.Footer
                contentHeight: 50 
                objectName : "tabs"

                currentIndex: 0

            }

            StackLayout {
                Layout.fillWidth : true
                currentIndex: mainTabs.currentIndex
                objectName : "content"

            }
        }
    }
}