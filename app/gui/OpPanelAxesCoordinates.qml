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

        OpText {
            text: qsTr("КООРДИНАТЫ ОСЕЙ")

            Layout.preferredHeight: 18
            Layout.fillWidth: true
        }


        Item {
            Layout.preferredHeight: 18
            Layout.fillWidth: true
            RowLayout {
                anchors.fill: parent
                OpText {
                    text: "X"
                }


            }
        }

        Item {
            Layout.preferredHeight: 18
            Layout.fillWidth: true
            RowLayout {
                anchors.fill: parent

            }
        }

        OpText {
            text: qsTr("СКОРОСТЬ ОСЕЙ")

            Layout.preferredHeight: 18
            Layout.fillWidth: true
        }


        RowLayout {

            Layout.preferredHeight: 36
            Layout.fillWidth: true

            OpText {
                text: "0"
                color: colors.black_80
                font.pixelSize: 16

                Layout.preferredHeight: 18
                //Layout.fillWidth: true
            }

            OpSlider {
                from: 1
                value: 25
                to: 100

                Layout.preferredHeight: 18
                Layout.fillWidth: true
            }

            OpText {
                text: "100%"
                color: colors.black_80
                font.pixelSize: 16

                Layout.preferredHeight: 18
                //Layout.fillWidth: true
            }

        }
    }
}
