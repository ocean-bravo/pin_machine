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

    component VGap4 : Item { width: 10; DebugRect {} Layout.preferredHeight: 4}
    component VGap16 : Item { width: 10; DebugRect {} Layout.preferredHeight: 16}

    // component VGap20 : Item { width: 10; DebugRect {} Layout.preferredHeight: 20}

    component HGap8 : Item { height: 10; DebugRect {} Layout.preferredWidth: 8}
    component HGap16 : Item { height: 10; DebugRect {} Layout.preferredWidth: 16}
    // component HGap24 : Item { height: 10; DebugRect {} Layout.preferredWidth: 24}

    contentItem:  Item {
        DebugRect { color: "#22FF0FF0";}

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            Item {
                Layout.preferredHeight: 170
                Layout.fillWidth: true

                RowLayout {
                    anchors.fill: parent
                    spacing: 0
                    anchors.margins: 0

                    Hspacer {}

                    Item {
                        Layout.preferredHeight: 172
                        Layout.preferredWidth: 520

                        ColumnLayout {
                            anchors.fill: parent
                            spacing: 0

                            OpHeader { text: qsTr("Движение в заданные координаты") }

                            VGap16 {}

                            Item {
                                Layout.preferredHeight: 40
                                Layout.fillWidth: true

                                RowLayout {
                                    anchors.fill: parent
                                    spacing: 0

                                    MyText { text: "X"; }
                                    HGap8 {}
                                    OpDoubleSpinbox {
                                        id: moveX
                                        decimals: 2
                                        from: 0
                                        to: 300
                                        value: 200
                                        editable: true
                                    }
                                    HGap16 {}
                                    OpFrameButton {
                                        text: qsTr("Идти")
                                        color: colors.primary_90
                                        onClicked: write("G1 G90 F" + xyFeedRateSlider.value + " X" + moveX.text)
                                    }

                                    Hspacer {}

                                    MyText { text: "Y"; }
                                    HGap8 {}
                                    OpDoubleSpinbox {
                                        id: moveY
                                        decimals: 2
                                        from: 0
                                        to: 300
                                        value: 10.05
                                        editable: true
                                    }
                                    HGap16 {}
                                    OpFrameButton {
                                        text: qsTr("Идти")
                                        color: colors.primary_90
                                        onClicked: write("G1 G90 F" + xyFeedRateSlider.value + " Y" + moveY.text)
                                    }
                                }
                            }

                            VGap16 {}

                            Item {
                                Layout.preferredHeight: 58
                                Layout.fillWidth: true

                                ColumnLayout {
                                    anchors.fill: parent

                                    spacing: 0

                                    OpHeader { text: qsTr("Скорость осей X/Y") }

                                    VGap4 {}


                                    OpSliderMinMax {
                                        id: xyFeedRateSlider
                                        Layout.preferredHeight: 36
                                        Layout.fillWidth: true

                                        from: 10
                                        value: 1000
                                        to: 5000
                                        stepSize: 10
                                    }

                                }
                            }
                        }
                    }

                    Hspacer {}
                }
            }

            OpJogXY {
                xyFeedRate: xyFeedRateSlider.value
                Layout.preferredHeight: 700
                Layout.fillWidth: true
            }

            Vspacer {}
        }
    }

}
