import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15


Control {
    id: root

    padding: 16

    background: OpWhitePanel {}


    component MyText : OpText {
        font.capitalization: Font.AllUppercase
        font.weight: Font.Medium
        font.pixelSize: 16
        color: colors.black_90

        Layout.preferredHeight: 18
        Layout.fillWidth: true
    }

    contentItem: Item {

        ColumnLayout {
            anchors.fill: parent
            spacing: 16

            Item {
                Layout.preferredHeight: 58
                Layout.fillWidth: true

                ColumnLayout {
                    anchors.fill: parent

                    spacing: 4

                    OpText {
                        text: qsTr("Скорость осей Z/W")
                        font.capitalization: Font.AllUppercase
                        font.weight: Font.Medium

                        Layout.preferredHeight: 18
                        Layout.fillWidth: true
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

            Item {
                Layout.fillHeight: true
                Layout.fillWidth: true

                RowLayout {
                    anchors.fill: parent
                    spacing: 24

                    OpJogZW {
                        Layout.preferredWidth: 192
                        Layout.fillHeight: true
                    }

                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        ColumnLayout {
                            anchors.fill: parent
                            spacing: 16

                            Vspacer {}

                            OpHeader { text: qsTr("Параметры установки контактов") }
                            MyText { text: qsTr("Z рез") }

                            Item {
                                Layout.preferredHeight: 40
                                Layout.fillWidth: true
                                RowLayout {
                                    anchors.fill: parent

                                    OpDoubleSpinbox {

                                        from: 0
                                        to: 20
                                        value: 10
                                        editable: true

                                    }

                                    OpSolidButton {
                                        Layout.fillWidth: true
                                        //Layout.preferredWidth: implicitWidth * 2
                                        text: qsTr("Идти")
                                    }

                                    OpFrameButton {
                                        Layout.fillWidth: true
                                        text: qsTr("Сохранить в продукт")
                                    }
                                }
                            }
                            MyText { text: qsTr("Z установка") }

                            Item {
                                Layout.preferredHeight: 40
                                Layout.fillWidth: true
                                RowLayout {
                                    anchors.fill: parent

                                    OpDoubleSpinbox {
                                        from: 0
                                        to: 20
                                        value: 10
                                        editable: true
                                    }

                                    OpSolidButton {
                                        Layout.fillWidth: true
                                        //Layout.preferredWidth: implicitWidth * 2
                                        text: qsTr("Идти")
                                    }

                                    OpFrameButton {
                                        Layout.fillWidth: true
                                        //Layout.preferredWidth: implicitWidth
                                        text: qsTr("Сохранить в продукт")
                                    }
                                }
                            }

                            MyText { text: qsTr("Z парковка") }

                            Item {
                                Layout.preferredHeight: 40
                                Layout.fillWidth: true
                                RowLayout {
                                    anchors.fill: parent

                                    OpDoubleSpinbox {
                                        from: 0
                                        to: 20
                                        value: 10
                                        editable: true
                                    }

                                    OpSolidButton {
                                        Layout.fillWidth: true
                                        //Layout.preferredWidth: implicitWidth * 2
                                        text: qsTr("Идти")
                                    }

                                    OpFrameButton {
                                        Layout.fillWidth: true
                                        text: qsTr("Сохранить в продукт")
                                    }
                                }
                            }

                            OpHeader { text: qsTr("Параметры Подпора платы") }
                            MyText { text: qsTr("W подпор") }

                            Item {
                                Layout.preferredHeight: 40
                                Layout.fillWidth: true
                                RowLayout {
                                    anchors.fill: parent

                                    OpDoubleSpinbox {
                                        from: 0
                                        to: 20
                                        value: 10
                                        editable: true
                                    }

                                    OpSolidButton {
                                        Layout.fillWidth: true
                                        //Layout.preferredWidth: implicitWidth * 2
                                        text: qsTr("Идти")
                                    }

                                    OpFrameButton {
                                        Layout.fillWidth: true
                                        Layout.preferredWidth: 40
                                        text: qsTr("Сохранить в продукт")
                                    }
                                }
                            }
                            MyText { text: qsTr("W парковка") }

                            Item {
                                Layout.preferredHeight: 18
                                Layout.fillWidth: true
                                RowLayout {
                                    anchors.fill: parent

                                    OpDoubleSpinbox {
                                        from: 0
                                        to: 20
                                        value: 10
                                        editable: true
                                    }

                                    OpSolidButton {
                                        Layout.fillWidth: true
                                        //Layout.preferredWidth: implicitWidth * 2
                                        text: qsTr("Идти")
                                    }

                                    OpFrameButton {
                                        Layout.fillWidth: true
                                        text: qsTr("Сохранить в продукт")
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
