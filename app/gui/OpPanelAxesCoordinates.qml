import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15


Control {
    id: root

    padding: 16

    background: OpWhitePanel {}

    component HGap8 : Item { height: 10; DebugRect {} Layout.preferredWidth: 8}

    contentItem: ColumnLayout {
        spacing: 0

        OpHeader { text: qsTr("КООРДИНАТЫ ОСЕЙ") }

        Vspacer {}

        Item {
            Layout.preferredHeight: 32
            Layout.fillWidth: true
            RowLayout {
                anchors.fill: parent

                spacing: 8

                MyText {
                    text: "X"
                }

                OpDoubleSpinbox {
                    value: DataBus.xPos === undefined ? 0 : DataBus.xPos
                    Layout.preferredHeight: 32
                }

                HGap8 {}

                MyText {
                    text: "Y"
                }

                OpDoubleSpinbox {
                    value: DataBus.yPos === undefined ? 0 : DataBus.yPos
                    Layout.preferredHeight: 32
                }

                Hspacer {}
            }
        }

        Vspacer {}

        Item {
            Layout.preferredHeight: 32
            Layout.fillWidth: true
            RowLayout {
                anchors.fill: parent

                spacing: 8

                MyText {
                    text: "Z"
                }
                OpDoubleSpinbox {
                    value: DataBus.zPos === undefined ? 0 : DataBus.zPos
                    Layout.preferredHeight: 32
                }

                HGap8 {}

                MyText {
                    text: "W"
                }

                OpDoubleSpinbox {
                    value: DataBus.wPos === undefined ? 0 : DataBus.wPos
                    Layout.preferredHeight: 32
                }

                Hspacer {}
            }
        }

        Vspacer {}

        Item {
            Layout.preferredHeight: 58
            Layout.fillWidth: true

            ColumnLayout {
                anchors.fill: parent

                spacing: 4

                OpHeader {
                    text: qsTr("СКОРОСТЬ ОСЕЙ")
                }

                Item {
                    Layout.preferredHeight: 36
                    Layout.fillWidth: true

                    RowLayout {
                        anchors.fill: parent

                        OpText {
                            text: "0"
                            color: colors.black_80
                            font.pixelSize: 16
                            font.weight: Font.Medium

                            //Layout.preferredHeight: 28
                        }

                        OpSlider {
                            from: 1
                            value: 25
                            to: 100

                            //Layout.preferredHeight: 28
                            Layout.fillWidth: true
                        }

                        OpText {
                            text: "100%"
                            color: colors.black_80
                            font.pixelSize: 16
                            font.weight: Font.Medium

                            //Layout.preferredHeight: 28
                        }
                    }
                }
            }
        }
    }

    component MyText: OpText {
        Layout.preferredWidth: 24
        Layout.preferredHeight: 32

        font.weight: Font.Medium
        font.pixelSize: 24
        color: colors.black_90
    }
}
