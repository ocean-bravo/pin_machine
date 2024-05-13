import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15


Control {
    id: root

    padding: 16

    background: OpWhitePanel {}

    component MyText: OpText {
        Layout.fillHeight: true

        horizontalAlignment: Text.AlignHCenter

        font.weight: Font.Medium
        font.pixelSize: 28
        color: colors.black_90
    }

    component VGap16 : Item { width: 10; DebugRect {} Layout.preferredHeight: 16}
    component VGap20 : Item { width: 10; DebugRect {} Layout.preferredHeight: 20}

    component HGap8 : Item { height: 10; DebugRect {} Layout.preferredWidth: 8}
    component HGap24 : Item { height: 10; DebugRect {} Layout.preferredWidth: 24}

    contentItem: Item {
        DebugRect { color: "#22FF0FF0" }

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            OpHeader { text: qsTr("Расстояние между осью Камеры и осью Инструмента") }

            VGap16 {}

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: 40

                RowLayout {
                    anchors.fill: parent
                    spacing: 0

                    MyText { text: "dX" }
                    HGap8 {}
                    OpDoubleSpinbox {
                        value: DataBus.punch_tool_shift_dx
                        editable: false
                    }

                    HGap24 {}

                    MyText { text: "dY" }
                    HGap8 {}
                    OpDoubleSpinbox {
                        value: DataBus.punch_tool_shift_dy
                        editable: false
                    }

                    Hspacer {}

                }
            }

            VGap20 {}

            OpHeader { text: qsTr("Переместить точку под Камерой в точку под Инструментом") }

            VGap16 {}

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: 40

                RowLayout {
                    anchors.fill: parent
                    spacing: 0

                    Image {
                        source: "images/camera_tool.svg"
                        Layout.fillHeight: true
                    }

                    HGap24{}

                    OpFrameButton {
                        text: qsTr("Идти")
                        color: colors.primary_90
                        onClicked: {
                            jog("X", -DataBus.punch_tool_shift_dx)
                            jog("Y", DataBus.punch_tool_shift_dy)
                        }
                    }

                    Hspacer {}
                }
            }

            VGap20 {}

            OpHeader { text: qsTr("Переместить точку под Инструментом в точку под Камерой") }

            VGap16 {}

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: 40

                RowLayout {
                    anchors.fill: parent
                    spacing: 0

                    Image {
                        source: "images/tool_camera.svg"
                        Layout.fillHeight: true
                    }
                    HGap24{}
                    OpFrameButton {
                        text: qsTr("Идти")
                        color: colors.primary_90

                        onClicked: {
                            jog("X", DataBus.punch_tool_shift_dx)
                            jog("Y", -DataBus.punch_tool_shift_dy)
                        }
                    }

                    Hspacer {}
                }
            }

            Vspacer {}
        }
    }
}
