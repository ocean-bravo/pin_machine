import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15


Control {
    id: root

    padding: 16

    background: OpWhitePanel {}

    contentItem: ColumnLayout {

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

                        OpText {
                            text: qsTr("Параметры установки контактов")
                            font.capitalization: Font.AllUppercase
                            font.weight: Font.Medium

                            Layout.preferredHeight: 18
                            Layout.fillWidth: true
                        }
                        OpText {
                            text: qsTr("Z РЕЗ")
                            font.capitalization: Font.AllUppercase
                            font.weight: Font.Medium

                            Layout.preferredHeight: 18
                            Layout.fillWidth: true
                        }

                        Item {
                            Layout.preferredHeight: 18
                            Layout.fillWidth: true
                            RowLayout {
                                anchors.fill: parent

                                OpDoubleSpinbox {

                                }

                                OpSolidButton {
                                    Layout.preferredHeight: 40
                                    Layout.fillWidth: true
                                    text: qsTr("Идти")
                                }

                                OpFrameButton {
                                    Layout.preferredHeight: 40
                                    Layout.fillWidth: true
                                    text: qsTr("Сохранить в продукт")
                                }
                            }
                        }
                        OpText {
                            text: qsTr("Z УСТАНОВКА")
                            font.capitalization: Font.AllUppercase
                            font.weight: Font.Medium

                            Layout.preferredHeight: 18
                            Layout.fillWidth: true
                        }

                        Item {
                            Layout.preferredHeight: 18
                            Layout.fillWidth: true
                            RowLayout {
                                anchors.fill: parent

                                OpDoubleSpinbox {

                                }

                                OpSolidButton {
                                    Layout.preferredHeight: 40
                                    Layout.fillWidth: true
                                    text: qsTr("Идти")
                                }

                                OpFrameButton {
                                    Layout.preferredHeight: 40
                                    Layout.fillWidth: true
                                    text: qsTr("Сохранить в продукт")
                                }
                            }
                        }

                        OpText {
                            text: qsTr("Z ПАРКОВКА")
                            font.capitalization: Font.AllUppercase
                            font.weight: Font.Medium

                            Layout.preferredHeight: 18
                            Layout.fillWidth: true
                        }

                        Item {
                            Layout.preferredHeight: 18
                            Layout.fillWidth: true
                            RowLayout {
                                anchors.fill: parent

                                OpDoubleSpinbox {

                                }

                                OpSolidButton {
                                    Layout.preferredHeight: 40
                                    Layout.fillWidth: true
                                    text: qsTr("Идти")
                                }

                                OpFrameButton {
                                    Layout.preferredHeight: 40
                                    Layout.fillWidth: true
                                    text: qsTr("Сохранить в продукт")
                                }
                            }
                        }

                        OpText {
                            text: qsTr("Параметры Подпора платы")
                            font.capitalization: Font.AllUppercase
                            font.weight: Font.Medium

                            Layout.preferredHeight: 18
                            Layout.fillWidth: true
                        }
                        OpText {
                            text: qsTr("W подпор")
                            font.capitalization: Font.AllUppercase
                            font.weight: Font.Medium

                            Layout.preferredHeight: 18
                            Layout.fillWidth: true
                        }

                        Item {
                            Layout.preferredHeight: 18
                            Layout.fillWidth: true
                            RowLayout {
                                anchors.fill: parent

                                OpDoubleSpinbox {

                                }

                                OpSolidButton {
                                    Layout.preferredHeight: 40
                                    Layout.fillWidth: true
                                    text: qsTr("Идти")
                                }

                                OpFrameButton {
                                    Layout.preferredHeight: 40
                                    Layout.fillWidth: true
                                    text: qsTr("Сохранить в продукт")
                                }
                            }
                        }
                        OpText {
                            text: qsTr("W парковка")
                            font.capitalization: Font.AllUppercase
                            font.weight: Font.Medium

                            Layout.preferredHeight: 18
                            Layout.fillWidth: true
                        }

                        Item {
                            Layout.preferredHeight: 18
                            Layout.fillWidth: true
                            RowLayout {
                                anchors.fill: parent

                                OpDoubleSpinbox {

                                }

                                OpSolidButton {
                                    Layout.preferredHeight: 40
                                    Layout.fillWidth: true
                                    text: qsTr("Идти")
                                }

                                OpFrameButton {
                                    Layout.preferredHeight: 40
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
