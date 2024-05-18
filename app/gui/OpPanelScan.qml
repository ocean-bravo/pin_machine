import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import "utils.js" as Utils


Control {
    id: root

    padding: 16

    background: OpWhitePanel {}

    property var currentScene

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

                OpDoubleSpinbox { id: x1; editable: true; Layout.fillWidth: true }
                OpDoubleSpinbox { id: y1; editable: true; Layout.fillWidth: true }
                OpDoubleSpinbox { id: x2; editable: true; Layout.fillWidth: true }
                OpDoubleSpinbox { id: y2; editable: true; Layout.fillWidth: true }

                OpFrameButton {
                    Layout.columnSpan: 2
                    Layout.fillWidth: true
                    text: qsTr("Взять текущие координаты")
                    onClicked: {
                        x1.value = xPos
                        y1.value = yPos
                    }
                }
                OpFrameButton {
                    Layout.columnSpan: 2
                    Layout.fillWidth: true
                    text: qsTr("Взять текущие координаты")
                    onClicked: {
                        x2.value = xPos
                        y2.value = yPos
                    }
                }
            }
        }

        OpSolidButton {
            id: fastScan
            Layout.fillWidth: true
            text: qsTr("Быстрое сканирование")
            checkable: true
            onCheckedChanged: {
                if (checked) {
                    let stepX = 10
                    let stepY = 8
                    let speed = 5000
                    let scanProgram = Utils.generateSteps(x1.value, y1.value, x2.value, y2.value, stepX, stepY, speed).join("\n")
                    TaskScan.run(scanProgram, 800, 600, "YUYV")
                }
                else {
                    TaskScan.stopProgram()
                }
            }
            Connections { target: TaskScan; function onFinished() { fastScan.checked = false } }
        }

        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: 40

            RowLayout {
                anchors.fill: parent

                OpCombobox{
                    id: findBlobScenes
                    Layout.fillWidth: true
                    Layout.preferredWidth: 1
                    //text: "Выбор сцены"
                    model: Engine.filesInSceneDirectory()

                    onActivated: {
                        currentScene = Engine.readSceneFile(currentText)
                    }
                    onModelChanged: {
                        currentScene = Engine.readSceneFile(currentText)
                    }
                    onDownChanged: {
                        let currText = currentText
                        model = Engine.filesInSceneDirectory()
                        currentIndex = model.indexOf(currText)
                    }

                    Component.onCompleted: {
                        currentScene = Engine.readSceneFile(currentText)
                    }
                }

                OpFrameButton {
                    id: findBlobs
                    Layout.fillWidth: true
                    Layout.preferredWidth: 1
                    text: qsTr("Поиск отверстий")

                    checkable: true
                    onCheckedChanged: checked ? TaskFindBlob.run(currentScene, false)
                                              : TaskFindBlob.stopProgram()
                    Connections { target: TaskFindBlob; function onFinished() { findBlobs.checked = false } }
                }
            }
        }

        OpSolidButton {
            id: updateSelected
            Layout.fillWidth: true
            text: qsTr("Уточнить координаты выбранных точек")

            checkable: true
            onCheckedChanged: checked ? TaskUpdate.run(800, 600, "YUYV")
                                      : TaskUpdate.stopProgram()
            Connections { target: TaskUpdate; function onFinished() { updateSelected.checked = false } }
        }
    }
}
