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
        color: colors.black_100
    }

    component Gap16 : Vspacer {Layout.preferredHeight: 16}
    component Gap20 : Vspacer {Layout.preferredHeight: 20}

    contentItem: ColumnLayout {

        spacing: 0

        OpText {
            text: qsTr("Расстояние между осью Камеры и осью Инструмента")
            font.capitalization: Font.AllUppercase

            Layout.preferredHeight: 18
            Layout.fillWidth: true
        }

        Gap16 {}

        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: 40

            RowLayout {
                anchors.fill: parent

                MyText {
                    text: "dX"

                }

                OpLabel {
                    text: "120.00"
                    Layout.fillHeight: true
                    Layout.preferredWidth: 80


                }

                MyText {
                    text: "dY"
                }
                OpLabel{
                    text: "150.00"
                    Layout.fillHeight: true
                    Layout.preferredWidth: 80
                }
                Hspacer {}

            }
        }

        Gap20 {}

        OpText {
            text: qsTr("Переместить точку под Камерой в точку под Инструментом")
            font.capitalization: Font.AllUppercase

            Layout.preferredHeight: 18
            Layout.fillWidth: true
        }

        Gap16 {}

        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: 40

            RowLayout {
                anchors.fill: parent
                spacing: 24

                Image {
                    source: "images/camera_tool.svg"
                    Layout.fillHeight: true
                }

                OpFrameButton {
                    text: qsTr("Идти")
                    color: colors.primary_90
                    font.weight: Font.Medium

                    Layout.fillHeight: true
                }

                Hspacer {}
            }
        }

        Gap20 {}

        OpText {
            text: qsTr("Переместить точку под Инструментом в точку под Камерой")
            font.capitalization: Font.AllUppercase

            Layout.preferredHeight: 18
            Layout.fillWidth: true
        }

        Gap16 {}

        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: 40

            RowLayout {
                anchors.fill: parent
                spacing: 24

                Image {
                    source: "images/tool_camera.svg"
                    Layout.fillHeight: true
                }

                OpFrameButton {
                    text: qsTr("Идти")
                    color: colors.primary_90
                    font.weight: Font.Medium

                    Layout.fillHeight: true
                }

                Hspacer {}
            }
        }

        Vspacer {}
    }
}
