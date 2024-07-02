import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15


Control {
    id: root

    padding: 16

    background: OpWhitePanel {}

    property int zwFeedRate: zwFeedRateMax * zwFeedRateSlider.value / 100

    component MyText : OpText {
        font.capitalization: Font.AllUppercase
        font.weight: Font.Medium
        font.pixelSize: 16
        color: colors.black_90

        Layout.preferredHeight: 18
        Layout.fillWidth: true
    }

    component VGap3 : Item { width: 10; DebugRect {} Layout.preferredHeight: 3}
    component VGap8 : Item { width: 10; DebugRect {} Layout.preferredHeight: 8}
    // component VGap16 : Item { width: 10; DebugRect {} Layout.preferredHeight: 16}
    // component VGap20 : Item { width: 10; DebugRect {} Layout.preferredHeight: 20}
    component VGap24 : Item { width: 10; DebugRect {} Layout.preferredHeight: 24}

    // component HGap8 : Item { height: 10; DebugRect {} Layout.preferredWidth: 8}
    component HGap16 : Item { height: 10; DebugRect {} Layout.preferredWidth: 16}
    component HGap24 : Item { height: 10; DebugRect {} Layout.preferredWidth: 24}
    component HGap40 : Item { height: 10; DebugRect {} Layout.preferredWidth: 40}

    contentItem: Item {

        ColumnLayout {
            anchors.fill: parent
            spacing: 16

            Item {
                Layout.preferredHeight: 58
                Layout.fillWidth: true
                DebugRect {}

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 4

                    OpHeader { text: qsTr("Скорость осей Z/W") }

                    OpSliderMinMax {
                        id: zwFeedRateSlider

                        Layout.preferredHeight: 36
                        Layout.fillWidth: true
                    }
                }
            }

            Item {
                Layout.fillHeight: true
                Layout.fillWidth: true

                RowLayout {
                    anchors.fill: parent
                    spacing: 0

                    //HGap16 {}

                    OpJogZW {
                        zwFeedRate: root.zwFeedRate

                        Layout.preferredWidth: 160
                        Layout.fillHeight: true
                    }

                    HGap40 {}

                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        DebugRect { color: "#22FF0FF0" }

                        ColumnLayout {
                            anchors.fill: parent
                            spacing: 0

                            Vspacer {}

                            OpHeader { text: qsTr("Параметры установки контактов") }
                            VGap8 {}

                            MyText { text: qsTr("Z рез") }
                            VGap3 {}
                            Item {
                                Layout.preferredHeight: 40
                                Layout.fillWidth: true
                                RowLayout {
                                    anchors.fill: parent

                                    OpDoubleSpinbox {
                                        id: zCut
                                        from: 0
                                        to: 20
                                        value: Product.z_cut
                                        editable: true
                                    }

                                    OpSolidButton {
                                        Layout.fillWidth: true
                                        //Layout.preferredWidth: implicitWidth * 2
                                        text: qsTr("Идти")
                                        onClicked: write("G1 G90 F" + zwFeedRate + " Z" + zCut.text)
                                    }

                                    OpFrameButton {
                                        Layout.fillWidth: true
                                        text: qsTr("Сохранить в продукт")
                                        onClicked: Product.z_cut = zCut.value
                                    }
                                }
                            }

                            VGap8 {}

                            MyText { text: qsTr("Z установка") }
                            VGap3 {}
                            Item {
                                Layout.preferredHeight: 40
                                Layout.fillWidth: true
                                RowLayout {
                                    anchors.fill: parent

                                    OpDoubleSpinbox {
                                        id: zSet
                                        from: 0
                                        to: 20
                                        value: Product.z_set
                                        editable: true
                                    }

                                    OpSolidButton {
                                        Layout.fillWidth: true
                                        //Layout.preferredWidth: implicitWidth * 2
                                        text: qsTr("Идти")
                                        onClicked: write("G1 G90 F" + zwFeedRate + " Z" + zSet.text)
                                    }

                                    OpFrameButton {
                                        Layout.fillWidth: true
                                        //Layout.preferredWidth: implicitWidth
                                        text: qsTr("Сохранить в продукт")
                                        onClicked: Product.z_set = zSet.value
                                    }
                                }
                            }

                            VGap8 {}

                            MyText { text: qsTr("Z парковка") }
                            VGap3 {}
                            Item {
                                Layout.preferredHeight: 40
                                Layout.fillWidth: true
                                RowLayout {
                                    anchors.fill: parent

                                    OpDoubleSpinbox {
                                        id: zPark
                                        from: 0
                                        to: 20
                                        value: Product.z_park
                                        editable: true
                                    }

                                    OpSolidButton {
                                        Layout.fillWidth: true
                                        //Layout.preferredWidth: implicitWidth * 2
                                        text: qsTr("Идти")
                                        onClicked: write("G1 G90 F" + zwFeedRate + " Z" + zPark.text)
                                    }

                                    OpFrameButton {
                                        Layout.fillWidth: true
                                        text: qsTr("Сохранить в продукт")
                                        onClicked: Product.z_park = zPark.value
                                    }
                                }
                            }

                            VGap24 {}

                            OpHeader { text: qsTr("Параметры Подпора платы") }
                            VGap8 {}

                            MyText { text: qsTr("W подпор") }
                            VGap3 {}
                            Item {
                                Layout.preferredHeight: 40
                                Layout.fillWidth: true

                                RowLayout {
                                    anchors.fill: parent

                                    OpDoubleSpinbox {
                                        id: wPodpor
                                        from: 0
                                        to: 20
                                        value: Product.w_podpor
                                        editable: true
                                    }

                                    OpSolidButton {
                                        Layout.fillWidth: true
                                        //Layout.preferredWidth: implicitWidth * 2
                                        text: qsTr("Идти")
                                        onClicked: write("G1 G90 F" + zwFeedRate + " W" + wPodpor.text)
                                    }

                                    OpFrameButton {
                                        Layout.fillWidth: true
                                        Layout.preferredWidth: 40
                                        text: qsTr("Сохранить в продукт")
                                        onClicked: Product.w_podpor = wPodpor.value
                                    }
                                }
                            }

                            VGap8 {}

                            MyText { text: qsTr("W парковка") }
                            VGap3 {}
                            Item {
                                Layout.preferredHeight: 40
                                Layout.fillWidth: true
                                RowLayout {
                                    anchors.fill: parent

                                    OpDoubleSpinbox {
                                        id: wPark
                                        from: 0
                                        to: 20
                                        value: Product.w_park
                                        editable: true
                                    }

                                    OpSolidButton {
                                        Layout.fillWidth: true
                                        //Layout.preferredWidth: implicitWidth * 2
                                        text: qsTr("Идти")
                                        onClicked: write("G1 G90 F" + zwFeedRate + " W" + wPark.text)
                                    }

                                    OpFrameButton {
                                        Layout.fillWidth: true
                                        text: qsTr("Сохранить в продукт")
                                        onClicked: Product.w_park = wPark.value
                                    }
                                }
                            }

                            Vspacer {}

                        }
                    }
                }
            }

        }
    }
}
