import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtWebEngine 1.3
import QtQuick.Layouts 1.11

ApplicationWindow {
    id: window
    visible: true
    width: initialWidth
    height: initialHeight
    title: webView.title

    Material.theme: Material.Dark
    Material.accent: Material.Cyan

    Item {
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

                currentIndex: 0

                TabButton {
                    text: qsTr("MainControl")
                }

                TabButton {
                    text: qsTr("WebControl")
                }
            }

            StackLayout {
                Layout.fillWidth : true
                currentIndex: mainTabs.currentIndex

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

                WebEngineView {
                    Layout.fillWidth: true
                    id: webView
                    url: initialUrl

                    onNewViewRequested: function(request) {
                        request.openIn(webView)
                    }
                }
            }
        }
    }
}