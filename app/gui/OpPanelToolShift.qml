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
            text: qsTr("Расстояние между осью Камеры и осью Инструмента")
            font.capitalization: Font.AllUppercase

            Layout.preferredHeight: 18
            Layout.fillWidth: true
        }

        Item {

            Layout.fillWidth: true

            RowLayout {
                anchors.fill: parent

            }
        }

        OpText {
            text: qsTr("Переместить точку под Камерой в точку под Инструментом")
            font.capitalization: Font.AllUppercase

            Layout.preferredHeight: 18
            Layout.fillWidth: true
        }

        Item {
            Layout.fillWidth: true
            RowLayout {
                anchors.fill: parent
                OpFrameButton {
                    text: qsTr("Идти")
                    color: colors.primary_90
                    font.pixelSize: 14

                    display: AbstractButton.TextOnly

                    Layout.preferredHeight: 32
                }
            }
        }

        OpText {
            text: qsTr("Переместить точку под Инструментом в точку под Камерой")
            font.capitalization: Font.AllUppercase

            Layout.preferredHeight: 18
            Layout.fillWidth: true
        }

        Item {
            Layout.fillWidth: true
            RowLayout {
                anchors.fill: parent
                OpFrameButton {
                    text: qsTr("Идти")
                    color: colors.primary_90
                    font.pixelSize: 14

                    display: AbstractButton.TextOnly

                    Layout.preferredHeight: 32
                }
            }
        }
    }
}
