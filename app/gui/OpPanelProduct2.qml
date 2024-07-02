import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.3
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
                    // verticalAlignment: Text.AlignBottom
                }

                OpTextEdit {
                    id: productName
                    text: Product.product_name

                    // font.weight: Font.Medium
                    // font.pixelSize: 24
                    //color: colors.error_80

                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    onTextChanged: {
                        Product.product_name = text
                    }
                }
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

                    onClicked: saveDialog.open()

                    FileDialog {
                        id: saveDialog
                        folder: applicationDirPath
                        fileMode: FileDialog.SaveFile
                        onAccepted: Product.save(currentFile)
                        modality: Qt.ApplicationModal
                         flags: Qt.Popup
                    }
                }

                OpFrameButton {
                    text: qsTr("Открыть продукт")
                    Layout.fillWidth: true
                    Layout.preferredWidth: 1
                    color: colors.primary_90
                    font.weight: Font.Medium

                    onClicked: loadDialog.open()

                    FileDialog {
                        id: loadDialog
                        folder: applicationDirPath
                        fileMode: FileDialog.OpenFile
                        onAccepted: Product.load(currentFile)
                        modality: Qt.ApplicationModal
                    }
                }
            }
        }
    }
}
