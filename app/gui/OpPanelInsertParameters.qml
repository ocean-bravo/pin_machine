import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15


Control {
    id: root

    padding: 16

    background: OpWhitePanel { /*color: "transparent"*/ }

    contentItem: RectangleDebug {

        ColumnLayout {
            anchors.fill: parent

            spacing: 16

            OpText {
                text: qsTr("ПАРАМЕТРЫ УСТАНОВКИ")

                Layout.preferredHeight: 18
                Layout.fillWidth: true
            }

            RectangleDebug {
                Layout.fillWidth: true
                Layout.fillHeight: true
                RowLayout {
                    anchors.fill: parent

                    spacing: 8

                    RectangleDebug {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 50

                        ColumnLayout {
                            anchors.fill: parent
                            spacing: 0

                            RectangleDebug {
                                Layout.preferredHeight: 18
                                Layout.fillWidth: true

                                Text {
                                    anchors.fill: parent
                                    text: "Z РЕЗ"
                                    color: colors.black_50
                                    font.pixelSize: 13
                                }
                            }

                            OpLabel {
                                Layout.preferredHeight: 32
                                Layout.preferredWidth: 80
                                text: "20.00"
                            }
                        }
                    }

                    RectangleDebug {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 50

                        ColumnLayout {
                            anchors.fill: parent
                            spacing: 0

                            RectangleDebug {
                                Layout.preferredHeight: 18
                                Layout.fillWidth: true

                                Text {
                                    anchors.fill: parent
                                    text: "Z УСТАНОВКА"
                                    color: colors.black_50
                                    font.pixelSize: 13
                                }
                            }

                            OpLabel {
                                Layout.preferredHeight: 32
                                Layout.preferredWidth: 80
                                text: "20.00"
                            }

                        }
                    }
                }
            }

            VSpacer { }

            RectangleDebug {
                Layout.preferredHeight: 18
                Layout.fillWidth: true
                Label {
                    anchors.fill: parent
                    text: "Контакт/Pin: 01-20113"

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

    component RectangleDebug: Rectangle {
        //Layout.fillWidth: true
        visible: guiDebug
        color: "#4000FF00"
    }

    component MyLabel: OpLabel {
        Layout.preferredWidth: 80
        Layout.preferredHeight: 32
    }

    Component.onCompleted: {

    }
}
