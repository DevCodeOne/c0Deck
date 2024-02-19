import QtQuick 2.15
import QtMultimedia 5.15

Item {
    required property string background
    anchors.fill: parent

    MediaPlayer {
            id: player
            loops: MediaPlayer.Infinite
            source : background
            Component.onCompleted: {
                player.play()
            }
        }

        VideoOutput {
            id: videoOutput
            source: player
            fillMode: VideoOutput.PreserveAspectCrop
            flushMode: VideoOutput.LastFrame
            anchors.fill: parent
    }
}