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

    component VGap8 : Item { width: 10; DebugRect {} Layout.preferredHeight: 8}
    component VGap16 : Item { width: 10; DebugRect {} Layout.preferredHeight: 16}
    component VGap20 : Item { width: 10; DebugRect {} Layout.preferredHeight: 20}

    component HGap8 : Item { height: 10; DebugRect {} Layout.preferredWidth: 8}
    component HGap24 : Item { height: 10; DebugRect {} Layout.preferredWidth: 24}

    component MyText: OpText {
        Layout.preferredWidth: 24
        Layout.fillHeight: true
        horizontalAlignment: Text.AlignLeft

        font.weight: Font.Medium
        font.pixelSize: 24
        color: colors.black_90
    }

    contentItem: Item {
        DebugRect { color: "#22FF0FF0" }

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            OpHeader { text: qsTr("ВИД С КАМЕРЫ") }
            VGap8 {}
            Rectangle {
                border.color: colors.accent_90
                border.width: 2
                radius: 8

                color: colors.black_30

                // Layout.fillWidth: true
                // Layout.preferredHeight: {
                //     if (DataBus.resolution_height === undefined)
                //         return width*0.75

                //     return width * DataBus.resolution_height / DataBus.resolution_width
                // }

                Layout.fillHeight: true

                Layout.preferredWidth: {
                    if (DataBus.resolution_height === undefined)
                        return height/0.75

                    return height / (DataBus.resolution_height / DataBus.resolution_width)
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

            VGap16 {}

            OpHeader { text: qsTr("КООРДИНАТЫ ОСЕЙ") }
            VGap8 {}

            Item {
                Layout.preferredHeight: 100
                Layout.fillWidth: true

                GridLayout {
                    anchors.fill: parent
                    columns: 3

                    Item {
                        Layout.preferredWidth: 100
                        RowLayout {
                            anchors.fill: parent


                            MyText { text: "X" }
                            HGap8 {}
                            OpDoubleSpinbox {
                                value: DataBus.xPos === undefined ? 0.0 : DataBus.xPos
                                editable: false
                            }
                        }
                    }

                    //HGap24 {}

                    Item {
                        Layout.preferredWidth: 100
                        RowLayout {
                            anchors.fill: parent

                            MyText { text: "Y" }
                            HGap8 {}
                            OpDoubleSpinbox {
                                value: DataBus.yPos === undefined ? 0.0 : DataBus.yPos
                                editable: false
                            }

                        }
                    }

                    OpSolidButton {
                        Layout.rowSpan: 2
                        icon.source: "images/home.svg"
                        display: AbstractButton.IconOnly

                        bgcolor: colors.primary_70

                        //Layout.preferredHeight: 44
                        Layout.preferredWidth: 50

                        onClicked: {
                            write("$H")
                        }

                        enabled: DataBus.homing_status === "Not ready"
                    }


                    Item {
                        Layout.preferredWidth: 100
                        RowLayout {
                            anchors.fill: parent

                            MyText { text: "Z" }
                            HGap8 {}
                            OpDoubleSpinbox {
                                value: DataBus.zPos === undefined ? 0.0 : DataBus.zPos
                                editable: false
                            }

                        }
                    }

                    Item {
                        Layout.preferredWidth: 100
                        RowLayout {
                            anchors.fill: parent

                            MyText { text: "W" }
                            HGap8 {}
                            OpDoubleSpinbox {
                                value: DataBus.wPos === undefined ? 0.0 : DataBus.wPos
                                editable: false
                            }

                        }
                    }





                }
            }

            VGap16 {}

            // Item {
            //     Layout.preferredHeight: 40
            //     Layout.fillWidth: true

            //     RowLayout {
            //         anchors.fill: parent
            //         spacing: 0

            //         MyText { text: "Z" }
            //         HGap8{}
            //         OpDoubleSpinbox {
            //             value: DataBus.zPos === undefined ? 0.0 : DataBus.zPos
            //             editable: false
            //         }

            //         HGap24{}

            //         MyText { text: "W" }
            //         HGap8{}
            //         OpDoubleSpinbox {
            //             value: DataBus.wPos === undefined ? 0.0 : DataBus.wPos
            //             editable: false
            //         }
            //         Hspacer {}
            //     }
            // }

            Item {
                Layout.preferredHeight: 40
                Layout.fillWidth: true

                RowLayout {
                    anchors.fill: parent
                    spacing: 4

                    OpHeader {
                        text: qsTr("СТАТУС ИНИЦИАЛИЗАЦИИ ОСЕЙ (HOMING):")
                        Layout.preferredHeight: 40
                    }

                    OpText {
                        text: {
                            if (DataBus.homing_status === "Not ready")   return qsTr("Не готов")
                            if (DataBus.homing_status === "In progress") return qsTr("В процессе")
                            if (DataBus.homing_status === "Ready")       return qsTr("Готов")
                        }

                        color: {
                            if (DataBus.homing_status === "Not ready")   return colors.error_80
                            if (DataBus.homing_status === "In progress") return colors.accent_90
                            if (DataBus.homing_status === "Ready")       return colors.green
                        }

                        font.pixelSize: 16
                        font.weight: Font.DemiBold

                        Layout.preferredHeight: 40
                        Layout.fillWidth: true
                    }


                }
            }
        }
    }
}
