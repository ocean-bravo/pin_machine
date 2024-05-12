import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtGraphicalEffects 1.15

import ImageItem 1.0

Control {
    id: root

    padding: 16

    background: OpWhitePanel {}

    contentItem: ColumnLayout {

        spacing: 16

        OpText {
            text: qsTr("ВИД С КАМЕРЫ")

            Layout.preferredHeight: 18
            Layout.fillWidth: true
        }

        Rectangle {
            border.color: colors.accent_90
            border.width: 2
            radius: 8

            color: colors.black_30

            Layout.fillWidth: true
            Layout.preferredHeight: {
                if (DataBus.resolution_height === undefined)
                    return width*0.75

                return width * DataBus.resolution_height / DataBus.resolution_width
            }

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

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }

        OpFrameButton {
            icon.source: checked ? "images/videocam_off_20.svg" : "images/videocam_on_20.svg"
            checkable: true
            text: checked ? qsTr("Выключить камеру") : qsTr("Включить камеру")
            color: colors.primary_90

            Layout.preferredHeight: 56


            onCheckedChanged: {
                if (checked) {
                    //resolutionList.setCurrentFormat()
                    Video4.start()
                }
                else {
                    Video4.stop()
                    execAfterDelay(function() { image.clear(); }, 100)
                }
            }
        }

        Timer {
            id: findCameraTimer
            interval: 1000
            repeat: false
            triggeredOnStart: false
            running: true
            onTriggered: {
                //Video4.reloadDevices()
            }
        }
    }
}
