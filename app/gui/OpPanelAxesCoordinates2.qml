import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15


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

            OpHeader { text: qsTr("Вид с камеры") }
            VGap16 {}

            OpAspectCameraView {}
            VGap16 {}

            Vspacer {}

            OpHeader { text: qsTr("Координаты осей") }

            Item {
                Layout.preferredHeight: 120
                Layout.fillWidth: true

                DebugRect { color: "#110000FF" }

                GridLayout {
                    anchors.fill: parent

                    columns: 4
                    rows: 2
                    rowSpacing: 0
                    columnSpacing: 24


                    Item {
                        Layout.preferredWidth: 112
                        Layout.preferredHeight: 40

                        Layout.row: 0
                        Layout.column: 0

                        DebugRect { color: "#110000FF" }

                        RowLayout {
                            anchors.fill: parent
                            spacing: 0

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
                        Layout.preferredWidth: 112
                        Layout.preferredHeight: 40

                        Layout.row: 0
                        Layout.column: 1

                        DebugRect { color: "#110000FF" }

                        RowLayout {
                            anchors.fill: parent
                            spacing: 0

                            MyText { text: "Y" }
                            HGap8 {}
                            OpDoubleSpinbox {
                                value: DataBus.yPos === undefined ? 0.0 : DataBus.yPos
                                editable: false
                            }

                        }
                    }

                    Hspacer {}

                    OpSolidButton {
                        Layout.rowSpan: 2
                        Layout.row: 0
                        Layout.column: 3
                        icon.source: "images/home.svg"

                        bgcolor: colors.primary_70

                        //Layout.preferredHeight: 44
                        Layout.preferredWidth: 50
                        Layout.preferredHeight: 50

                        onClicked: {
                            write("$H")
                        }

                        enabled: DataBus.homing_status === "Not ready"
                    }


                    Item {
                        Layout.preferredWidth: 112
                        Layout.preferredHeight: 40

                        Layout.row: 1
                        Layout.column: 0

                        DebugRect { color: "#110000FF" }

                        RowLayout {
                            anchors.fill: parent
                            spacing: 0

                            MyText { text: "Z" }
                            HGap8 {}
                            OpDoubleSpinbox {
                                value: DataBus.zPos === undefined ? 0.0 : DataBus.zPos
                                editable: false
                            }

                        }
                    }

                    Item {
                        Layout.preferredWidth: 112
                        Layout.preferredHeight: 40

                        Layout.row: 1
                        Layout.column: 1

                        DebugRect { color: "#110000FF" }

                        RowLayout {
                            anchors.fill: parent
                            spacing: 0

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
                        text: qsTr("Статус инициализации осей (homing):")
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
