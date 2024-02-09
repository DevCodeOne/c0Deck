import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material
import QtWebView
import QtQuick.Layouts

ApplicationWindow {
    id: window
    visible: true
    x: initialX
    y: initialY
    width: initialWidth
    height: initialHeight
    title: webView.title

    Material.theme: Material.Dark
    Material.accent: Material.Cyan

    ColumnLayout {
        anchors.fill: parent
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
                }

                AnimatedButton {
                    id: "second"
                }
            }

            WebView {
                Layout.fillWidth: true
                id: webView
                url: initialUrl
            }
        }
    }

}