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
            text: qsTr("Пошаговый цикл установки контакта")
            font.capitalization: Font.AllUppercase

            Layout.preferredHeight: 18
            Layout.fillWidth: true
        }

        Item {
            Layout.fillWidth: true
            RowLayout {
                anchors.fill: parent
                OpFrameButton {
                    text: qsTr("Шаг")
                    color: colors.primary_90
                    font.pixelSize: 14

                    display: AbstractButton.TextOnly

                    Layout.preferredHeight: 32
                }

                OpFrameButton {
                    text: qsTr("Стоп")
                    color: colors.primary_90
                    font.pixelSize: 14

                    display: AbstractButton.TextOnly

                    Layout.preferredHeight: 32
                }

                OpText {
                    text: "Парковка Z и W"
                }
            }
        }




    }
}
