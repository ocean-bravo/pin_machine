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

        OpText {
            text: qsTr("КООРДИНАТЫ ОСЕЙ")

            Layout.preferredHeight: 18
            Layout.fillWidth: true
        }

        VSpacer {}

        Item {
            Layout.preferredHeight: 32
            Layout.fillWidth: true
            RowLayout {
                anchors.fill: parent

                spacing: 8

                MyText {
                    text: "X"
                }

                MyLabel {
                    text: DataBus.x_coord
                }

                Item {
                    Layout.preferredWidth: 8
                }


                MyText {
                    text: "Y"
                }

                MyLabel {
                    text: DataBus.y_coord
                }

                Item {
                    Layout.fillWidth: true
                }
            }
        }

        VSpacer {}

        Item {
            Layout.preferredHeight: 32
            Layout.fillWidth: true
            RowLayout {
                anchors.fill: parent

                spacing: 8

                MyText {
                    text: "Z"
                }
                MyLabel {
                    text: DataBus.z_coord
                }

                Item {
                    Layout.preferredWidth: 8
                }

                MyText {
                    text: "W"
                }
                MyLabel {
                    text: "0.000"
                }
                Item {
                    Layout.fillWidth: true
                }
            }
        }

        VSpacer {}

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


    component VSpacer: Item {
        Layout.fillHeight: true
        width: 20

        Rectangle {
            anchors.fill: parent
            visible: guiDebug
            color: "#400000FF"
        }
    }

    component MyText: OpText {
        Layout.preferredWidth: 24
        Layout.preferredHeight: 32

        font.weight: Font.Medium
        font.pixelSize: 24
        color: colors.black_90
    }

    component MyLabel: Label {
        Layout.preferredWidth: 80
        Layout.preferredHeight: 32

        font.pixelSize: 16
        font.family: mainFont
        font.weight: Font.Medium
        leftPadding: 12

        verticalAlignment: Text.AlignVCenter
        background: Rectangle {
            radius: 8
            border.width: 1
            border.color: colors.black_20
        }
    }

}
