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

        OpHeader { text: qsTr("Область сканирования продукта") }


        Item {
            Layout.fillWidth: true

            Layout.fillHeight: true

            GridLayout {
                anchors.fill: parent
                columns: 5
                columnSpacing: 4

                OpHeader {
                    Layout.columnSpan: 2
                    Layout.column: 0
                    Layout.row: 0

                    text: qsTr("Левый нижний угол:")
                }

                Rectangle {
                    color: "lightgrey"
                    Layout.preferredWidth: 1
                    Layout.fillHeight: true
                    Layout.rowSpan: 4
                    Layout.column: 2
                    Layout.row: 0
                }

                OpHeader {
                    Layout.columnSpan: 2
                    Layout.column: 3
                    Layout.row: 0
                    text: qsTr("Правый верхний угол:")
                }

                OpText {

                    text: "X1"
                }

                OpText {
                    text: "Y1"
                }

                OpText {
                    text: "X2"
                }

                OpText {
                    text: "Y2"
                }

                OpDoubleSpinbox {
                    editable: true
                    Layout.fillWidth: true

                }

                OpDoubleSpinbox {
                    editable: true
                    Layout.fillWidth: true

                }
                OpDoubleSpinbox {
                    editable: true
                    Layout.fillWidth: true

                }
                OpDoubleSpinbox {
                    editable: true
                    Layout.fillWidth: true

                }

                OpFrameButton {
                    Layout.columnSpan: 2
                    Layout.fillWidth: true

                    text:  "Взять текущие координаты"

                }
                OpFrameButton {
                    Layout.columnSpan: 2
                    Layout.fillWidth: true
                    text:  "Взять текущие координаты"
                }
            }


        }



        OpSolidButton {
            Layout.fillWidth: true
            text: "Быстрое сканирование"
        }

        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            RowLayout {
                anchors.fill: parent
                OpFrameButton {
                    Layout.fillWidth: true
                    Layout.preferredWidth: 1
                    text: "Выбор сцены"

                    onClicked: {
                        pop.open()
                    }
                    Popup {
                        id: pop
                        width: 100
                        height: 100
                        ListView {
                            anchors.fill: parent
                            model: [1,2,3,4,5]
                            delegate: Text {text: modelData}
                        }
                    }
                }

                OpFrameButton {
                    Layout.fillWidth: true
                    Layout.preferredWidth: 1
                    text: "Поиск отверстий"
                }
            }
        }

        OpSolidButton {
            Layout.fillWidth: true
            text: "Уточнить координаты выбранных точек"
        }

    }



}
