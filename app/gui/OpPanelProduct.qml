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

        RowLayout {

            Layout.preferredHeight: 30
            Layout.fillWidth: true

            OpText {
                text: qsTr("ИМЯ ПРОДУКТА:")

                Layout.preferredHeight: 30
                Layout.fillWidth: true
            }

            OpText {
                text: "1B268-1B268-1B268"

                font.bold: true
                font.pixelSize: 24
                color: colors.black_80

                Layout.preferredHeight: 30
                Layout.fillWidth: true
            }
        }

        OpFrameButton {
            text: qsTr("Открыть продукт")
            solid: true
            font.pixelSize: 16

            color: colors.primary_70

            Layout.preferredHeight: 44
            Layout.fillWidth: true
        }

        RowLayout {

            Layout.preferredHeight: 32
            Layout.fillWidth: true


            OpText {
                text: qsTr("СЧЕТЧИК ГОТОВЫХ ПРОДУКТОВ:")

                Layout.preferredHeight: 32
                //Layout.fillWidth: true
            }

            OpText {
                text: "10000"

                font.pixelSize: 16
                color: colors.black_80

                Layout.preferredHeight: 32
                //Layout.fillWidth: true
            }

            Item {
                Layout.preferredHeight: 32
                Layout.fillWidth: true
            }

            OpFrameButton {
                text: qsTr("Сбросить")
                color: colors.primary_90
                font.pixelSize: 14


                display: AbstractButton.TextOnly

                Layout.preferredHeight: 32
                Layout.preferredWidth: 100
            }

        }




    }
}
