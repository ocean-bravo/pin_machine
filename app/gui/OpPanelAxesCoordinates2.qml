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


    // Поддерживает соотношение сторон ratio
    component AspectItem: Item {
        property real ratio: DataBus.resolution_height === undefined ? 0.75 : DataBus.resolution_height / DataBus.resolution_width

        Layout.fillWidth: true    // aspect
        Layout.fillHeight: true   // aspect
        Layout.maximumHeight: implicitHeight  // aspect
        implicitWidth: height / ratio  // aspect
        implicitHeight: width * ratio  // aspect

        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter // aspect
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


    contentItem: Item {
        DebugRect { color: "#22FF0FF0" }

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            OpHeader { text: qsTr("ВИД С КАМЕРЫ") }
            VGap8 {}

            AspectItem {





            }

            Vspacer {}

            OpHeader { text: qsTr("КООРДИНАТЫ ОСЕЙ") }

            Item {
                Layout.preferredHeight: 120
                Layout.fillWidth: true

                DebugRect { color: "#110000FF" }

                GridLayout {
                    anchors.fill: parent

                    columns: 3
                    rows: 2
                    rowSpacing: 0
                    columnSpacing: 0


                    Item {
                        Layout.preferredWidth: 120
                        Layout.preferredHeight: 40

                        Layout.row: 0
                        Layout.column: 0

                        DebugRect { color: "#110000FF" }

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
                        Layout.preferredWidth: 120
                        Layout.preferredHeight: 40

                        Layout.row: 0
                        Layout.column: 1

                        DebugRect { color: "#110000FF" }

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
                        Layout.row: 0
                        Layout.column: 2
                        icon.source: "images/home.svg"

                        bgcolor: colors.primary_70

                        //Layout.preferredHeight: 44
                        Layout.preferredWidth: 40
                        Layout.preferredHeight: 40

                        onClicked: {
                            write("$H")
                        }

                        enabled: DataBus.homing_status === "Not ready"
                    }


                    Item {
                        Layout.preferredWidth: 120
                        Layout.preferredHeight: 40

                        Layout.row: 1
                        Layout.column: 0

                        DebugRect { color: "#110000FF" }

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
                        Layout.preferredWidth: 120
                        Layout.preferredHeight: 40

                        Layout.row: 1
                        Layout.column: 1

                        DebugRect { color: "#110000FF" }

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

            Item {
                Layout.preferredHeight: 18
                Layout.fillWidth: true

                RowLayout {
                    anchors.fill: parent
                    spacing: 4

                    OpHeader {
                        text: qsTr("СТАТУС ИНИЦИАЛИЗАЦИИ ОСЕЙ (HOMING):")
                        Layout.fillWidth: false
                    }

                    OpText {
                        Layout.preferredHeight: 18
                        //Layout.fillWidth: true
                        //Layout.preferredWidth: implicitWidth

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
                    }
                    Hspacer {}
                }
            }
        }
    }
}
