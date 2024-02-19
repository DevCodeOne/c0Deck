import QtQuick 2.15
import QtQuick.Layouts 1.11
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

Item {

    id: main
    required property string name
    required property int volume
    required property int spacing

    signal doAction(string action)

    Rectangle {

        width: parent.width - 2 * spacing
        height: parent.height - spacing * 0.5
        anchors.centerIn: parent

        color: "transparent"
        border.color: Material.accent
        border.width: 5
        radius: 10

        RowLayout {
            width: parent.width - spacing
            height: parent.height
            anchors.centerIn: parent.center

            Label {
                text: main.name
                font.pixelSize: 22
                verticalAlignment: Text.AlignVCenter
                leftPadding: 10
                Layout.fillWidth: true
            }

            Slider {
                id: streamVolume
                from: 0
                to: 100
                value: volume
                Layout.preferredWidth: main.width * 2/3
                Layout.fillHeight: true
                onMoved: doAction(name + " : " + parseInt(value))
            }

            Label {
                text: parseInt(streamVolume.value)
                Layout.preferredWidth: main.width * 1/12
                font.pixelSize: 22
            }
        }
    }
}