import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.platform 1.1

Control {
    id: root

    padding: 16

    background: OpWhitePanel {}

    contentItem: ColumnLayout {

        spacing: 16

        Item {
            Layout.preferredHeight: 36
            Layout.fillWidth: true

            RowLayout {
                anchors.fill: parent

                OpHeader {
                    text: qsTr("Имя продукта:")

                    Layout.fillHeight: true
                    Layout.fillWidth: false
                    Layout.preferredWidth: implicitWidth
                    verticalAlignment: Text.AlignBottom
                }

                OpText {
                    text: "Не задано"

                    font.weight: Font.Medium
                    font.pixelSize: 24
                    color: colors.error_80

                    Layout.fillHeight: true
                    verticalAlignment: Text.AlignBottom

                    Layout.preferredWidth: implicitWidth
                    //Layout.fillWidth: true
                }
                Hspacer {}
            }
        }

        Item {
            Layout.preferredHeight: 40
            Layout.fillWidth: true

            RowLayout {
                anchors.fill: parent
                spacing: 16

                OpSolidButton {
                    text: qsTr("Сохранить продукт")
                    Layout.fillWidth: true
                    Layout.preferredWidth: 1
                    bgcolor: colors.primary_70
                    font.weight: Font.Medium
                }


                OpFrameButton {
                    text: qsTr("Открыть продукт")

                    color: colors.primary_90


                    Layout.fillWidth: true
                    Layout.preferredWidth: 1

                    onClicked: loadDialog.open()

                    FileDialog {
                        id: loadDialog
                        folder: applicationDirPath
                        fileMode: FileDialog.OpenFile
                        onAccepted: Engine.load(currentFile)
                        modality: Qt.ApplicationModal
                    }
                }


            }
        }
    }
}
