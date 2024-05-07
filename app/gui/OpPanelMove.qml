import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15


Control {
    id: root

    padding: 16

    background: OpWhitePanel {}

    contentItem: ColumnLayout {

        spacing: 0

        Item {
            Layout.preferredHeight: 172
            Layout.fillWidth: true

            RowLayout {
                anchors.fill: parent

                Hspacer {}

                Item {
                    Layout.preferredHeight: 172
                    Layout.preferredWidth: 518

                    ColumnLayout {
                        anchors.fill: parent

                        OpText {
                            Layout.preferredHeight: 18
                            Layout.fillWidth: true

                            text: qsTr("Движение в заданные координаты")
                            font.capitalization: Font.AllUppercase
                        }

                        Item {
                            Layout.preferredHeight: 58
                            Layout.fillWidth: true

                            RowLayout {
                                anchors.fill: parent

                            }
                        }

                        Item {
                            Layout.preferredHeight: 58
                            Layout.fillWidth: true

                            ColumnLayout {
                                anchors.fill: parent

                                spacing: 4

                                OpText {
                                    text: qsTr("СКОРОСТЬ ОСЕЙ")

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
                }

                Hspacer {}
            }
        }

        OpJog {
            Layout.preferredHeight: 700
            Layout.fillWidth: true
        }

        Vspacer {}

    }

}
