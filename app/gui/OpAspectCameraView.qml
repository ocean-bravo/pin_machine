import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtGraphicalEffects 1.15
import ImageItem 1.0

// Поддерживает соотношение сторон ratio
Item {
    property real ratio: DataBus.resolution_height === undefined ? 0.75 : DataBus.resolution_height / DataBus.resolution_width

    Layout.fillWidth: true    // aspect
    Layout.fillHeight: true   // aspect
    Layout.maximumHeight: implicitHeight  // aspect
    implicitWidth: height / ratio  // aspect
    implicitHeight: width * ratio  // aspect

    Rectangle {
        anchors.left: parent.left // aspect
        height: parent.height  // aspect
        implicitWidth: height / parent.ratio // aspect

        border.color: colors.accent_90
        border.width: 2
        radius: 8
        color: colors.black_30
        clip: true

        Column {
            anchors.centerIn: parent
            Image {
                width: 100
                height: 100
                source: "images/videocam_off_80.svg"
                fillMode: Image.Pad

            }
            OpText {
                width: 100
                color: "white"
                font.pixelSize: 16
                font.weight: Font.Medium
                text: qsTr("Изображение отключено")
                horizontalAlignment: Text.AlignHCenter
            }
        }

        ImageItem {
            id: image
            image: DataBus["live_preview_image_" + DataBus.live_preview_mode]
            anchors.fill: parent
            anchors.margins: parent.border.width // Чтобы была видна желтая граница
            crossColor: colors.accent_90

            property bool adapt: true

            layer.enabled: true
            layer.effect: OpacityMask {
                maskSource: Item {
                    width: image.width
                    height: image.height
                    Rectangle {
                        anchors.centerIn: parent
                        width: image.adapt ? image.width : Math.min(image.width, image.height)
                        height: image.adapt ? image.height : width
                        radius: 8
                    }
                }
            }
        }

        // Перехватываю мышь над картинкой, чтобы картинка не зумилась и не двигалась оператором
        MouseArea {
            anchors.fill: image
            onWheel: {
                wheel.accepted = true
            }
        }
    }
}
