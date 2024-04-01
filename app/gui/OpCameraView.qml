import QtQml 2.12
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import ImageItem 1.0

Control {
    id: root

    padding: 16
    // leftInset: 0
    // rightInset: 0
    // topInset: 0
    // bottomInset: 0

    background: OpWhitePanel {}

    contentItem: ColumnLayout {

        spacing: 16

        OpText {
            text: qsTr("ВИД С КАМЕРЫ")

            Layout.preferredHeight: 30
            Layout.fillWidth: true
        }

        Rectangle {
            border.color: "#FFC800"
            border.width: 2
            radius: 8

            color: "#B3B3B3"

            Layout.fillHeight: true
            Layout.fillWidth: true

            ImageItem {
                id: image
                image: DataBus["live_preview_image_" + DataBus.live_preview_mode]
                anchors.fill: parent
            }
        }

        OpFrameButton {
            icon.source: "images/videocam.png"
            checkable: true
            text: checked ? qsTr("Выключить камеру") : qsTr("Включить камеру")

            Layout.preferredHeight: 56

            onPressed: {

            }

            onReleased: {

            }
        }


    }


}
