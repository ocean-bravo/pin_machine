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
                    text: DataBus.x_coord === undefined ? "" : DataBus.x_coord
                }

                Item {
                    Layout.preferredWidth: 8
                }


                MyText {
                    text: "Y"
                }

                MyLabel {
                    text: DataBus.y_coord === undefined ? "" : DataBus.y_coord
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
                    text: DataBus.z_coord === undefined ? "" : DataBus.z_coord
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

    component MyLabel: OpLabel {
        Layout.preferredWidth: 80
        Layout.preferredHeight: 32
    }
}
