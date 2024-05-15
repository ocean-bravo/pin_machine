import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15


Control {
    id: root

    padding: 16

    background: OpWhitePanel {}

    contentItem: ColumnLayout {

        spacing: 8

        OpHeader {
            text: qsTr("Параметры установки контактов")
        }

        Item {
            Layout.fillWidth: true

            Layout.fillHeight: true

            GridLayout {
                anchors.fill: parent
                columns: 3
                columnSpacing: 4

                OpText {
                    Layout.fillWidth: true
                    Layout.preferredWidth: 1
                    text: "Z рез"
                }

                OpText {
                    Layout.fillWidth: true
                    Layout.preferredWidth: 1
                    text: "Z установка"
                }

                OpText {
                    Layout.fillWidth: true
                    Layout.preferredWidth: 1
                    text: "Z парковка"
                }

                OpDoubleSpinbox {
                    editable: true
                    Layout.fillWidth: true
                    Layout.preferredWidth: 1

                }

                OpDoubleSpinbox {
                    editable: true
                    Layout.fillWidth: true
                    Layout.preferredWidth: 1

                }
                OpDoubleSpinbox {
                    editable: true
                    Layout.fillWidth: true
                    Layout.preferredWidth: 1

                }
            }
        }

        OpHeader {
            text: qsTr("Параметры подпора платы")
        }

        Item {
            Layout.fillWidth: true

            Layout.fillHeight: true

            GridLayout {
                anchors.fill: parent
                columns: 3
                columnSpacing: 4

                OpText {
                    Layout.fillWidth: true
                    text: "W подпор"
                    Layout.preferredWidth: 1
                }

                OpText {
                    Layout.fillWidth: true
                    text: "W парковка"
                    Layout.preferredWidth: 1
                }

                Item {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 10
                    Layout.preferredWidth: 1

                }


                OpDoubleSpinbox {
                    editable: true
                    Layout.fillWidth: true
                    Layout.preferredWidth: 1

                }

                OpDoubleSpinbox {
                    editable: true
                    Layout.fillWidth: true
                    Layout.preferredWidth: 1

                }

                Item {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 10
                    Layout.preferredWidth: 1

                }
            }


        }


        OpHeader {
            text: qsTr("Артикул контакта")
        }

        CustomSwitch {
            Layout.fillWidth: true
            text: "Отобразить путь установки"
        }

        OpFrameButton {
            text: "Автоматический поиск оптимального пути"
        }

    }
}
