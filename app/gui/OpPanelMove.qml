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

    // component VGap16 : Item { width: 10; DebugRect {} Layout.preferredHeight: 16}
    // component VGap20 : Item { width: 10; DebugRect {} Layout.preferredHeight: 20}

    // component HGap8 : Item { height: 10; DebugRect {} Layout.preferredWidth: 8}
    // component HGap24 : Item { height: 10; DebugRect {} Layout.preferredWidth: 24}

    contentItem: ColumnLayout {

        spacing: 0

        Item {
            Layout.preferredHeight: 172
            Layout.fillWidth: true

            Rectangle { anchors.fill: parent; visible: guiDebug; color: "#22FF0FF0"; }

            RowLayout {
                anchors.fill: parent

                Hspacer {}

                Item {
                    Layout.preferredHeight: 172
                    Layout.preferredWidth: 518

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 16

                        OpHeader { text: qsTr("Движение в заданные координаты") }

                        Item {
                            Layout.preferredHeight: 40
                            Layout.fillWidth: true

                            RowLayout {
                                anchors.fill: parent
                                spacing: 16

                                MyText { text: "X"; }
                                OpLabel {
                                    text: "900.00"
                                    Layout.fillHeight: true
                                    Layout.preferredWidth: 80
                                }
                                OpFrameButton {
                                    Layout.fillHeight: true
                                    text: qsTr("Идти")
                                    color: colors.primary_90
                                }

                                Hspacer {}

                                MyText { text: "Y"; }
                                // OpLabel {
                                //     text: "1.01"
                                //     Layout.fillHeight: true
                                //     Layout.preferredWidth: 80
                                // }

                                OpDoubleSpinbox {
                                    decimals: 2
                                    from: 0
                                    to: 990

                                    value: 1.01
                                    Layout.fillHeight: true
                                    Layout.preferredWidth: 80
                                }

                                OpFrameButton {
                                    Layout.fillHeight: true
                                    text: qsTr("Идти")
                                    color: colors.primary_90
                                }
                            }
                        }

                        Item {
                            Layout.preferredHeight: 58
                            Layout.fillWidth: true

                            ColumnLayout {
                                anchors.fill: parent

                                spacing: 4

                                OpHeader { text: qsTr("Скорость осей X/Y") }

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
                                        }

                                        OpSlider {
                                            from: 1
                                            value: 25
                                            to: 100

                                            Layout.fillWidth: true
                                        }

                                        OpText {
                                            text: "100%"
                                            color: colors.black_80
                                            font.pixelSize: 16
                                            font.weight: Font.Medium
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                Hspacer {}
            }
        }

        OpJogXY {
            Layout.preferredHeight: 700
            Layout.fillWidth: true
        }

        Vspacer {}

    }

}
