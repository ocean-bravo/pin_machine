import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.platform 1.1
import "utils.js" as Utils

CollapsiblePanel {
    id: root

    width: parent.width
    height: checked ? 270 : 25

    Layout.preferredWidth: 400
    Layout.preferredHeight: checked ? 270 : 25

    checked: true

    text: qsTr("Task")

    onCheckedChanged: {
        grid.visible = checked
    }
    Component.onCompleted: {
        grid.visible = checked
    }

    property var currentOptions

    GridLayout {
        id: grid
        anchors.top:parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        columns: 4

        // 1.
        SmTextEdit {
            id: programParams
            Layout.preferredWidth: 180
            text: "85 120  165  170  10  8  5000"
            Layout.columnSpan: 2
        }

        SmButton {
            text: qsTr("Generate program")
            Layout.preferredWidth: 180
            Layout.columnSpan: 2

            onClicked: {
                codeEditor.clear()
                let p = programParams.text.split(' ').filter(e => e).map(Number) // Выкидываю нулевые строки и преобразую в массив чисел
                codeEditor.append(Utils.generateSteps(p[0], p[1], p[2], p[3], p[4], p[5], p[6]).join("\n"))
            }
        }
        // Item { height: 20; width: 10}

        // 2.
        SmButton {
            id: scan
            text: qsTr("Fast scan")
            checkable: true
            onCheckedChanged: checked ? TaskScan.run(codeEditor.text, selectedResolution().width, selectedResolution().height, selectedResolution().fourcc)
                                      : TaskScan.stopProgram()
            Connections { target: TaskScan; function onFinished() { scan.checked = false } }
            function selectedResolution() {
                return sortResolutions(DataBus["camera_image_formats_" + cameraList.currentValue])[resolutionListForScan.currentIndex]
            }
            Layout.preferredWidth: 120
        }

        Item { height: 20; width: 10}
        ComboBox {
            id: resolutionListForScan
            //width: 200
            Layout.preferredWidth: 180
            Layout.columnSpan: 2
            textRole: "display"
            model: sortResolutions(DataBus["camera_image_formats_" + cameraList.currentValue]) // Плохо, по другому выбирать откуда брать разрешения
            onModelChanged: {
                let res = sortResolutions(DataBus["camera_image_formats_" + cameraList.currentValue])

                for (let i = 0; i < res.length; i++) {
                    let r = res[i]
                    if (r.width === 800 && r.height === 600 &&  r.fourcc === "YUYV")
                        currentIndex = i
                }
            }
        }

        // 3.
        SmButton {
            id: update
            text: qsTr("Update selected")
            checkable: true
            onCheckedChanged: checked ? TaskUpdate.run(selectedResolution().width, selectedResolution().height, selectedResolution().fourcc, currentOptions)
                                      : TaskUpdate.stopProgram()
            Connections { target: TaskUpdate; function onFinished() { update.checked = false } }
            function selectedResolution() {
                return sortResolutions(DataBus["camera_image_formats_" + cameraList.currentValue])[resolutionListForUpdate.currentIndex]
            }

            Layout.preferredWidth: 120
        }
        Item { height: 30; width: 10}

        ComboBox {
            id: resolutionListForUpdate
            //width: 200
            Layout.preferredWidth: 180
            Layout.columnSpan: 2
            textRole: "display"
            model: sortResolutions(DataBus["camera_image_formats_" + cameraList.currentValue]) // Плохо, по другому выбирать откуда брать разрешения
            onModelChanged: {
                let res = sortResolutions(DataBus["camera_image_formats_" + cameraList.currentValue])

                for (let i = 0; i < res.length; i++) {
                    let r = res[i]
                    if (r.width === 1280 && r.height === 960 && r.fourcc === "YUYV")
                        currentIndex = i
                }
            }
        }
        //Item { height: 30; width: 10}

        // 4.
        SmButton {
            id: save
            text: qsTr("Save")
            onClicked: saveDialog.open()

            Layout.preferredWidth: 120

            FileDialog {
                id: saveDialog
                folder: applicationDirPath
                fileMode: FileDialog.SaveFile
                onAccepted: Engine.save(currentFile)
                modality: Qt.ApplicationModal
            }
        }
        Item { height: 30; width: 10}
        SmButton {
            id: load
            text: qsTr("Load")
            Layout.preferredWidth: 120

            onClicked: loadDialog.open()

            //onClicked: Engine.load(Qt.resolvedUrl("/home/mint/Desktop/12"))
            FileDialog {
                id: loadDialog
                folder: applicationDirPath
                fileMode: FileDialog.OpenFile
                onAccepted: Engine.load(currentFile)
                modality: Qt.ApplicationModal
            }
        }
        Item { height: 30; width: 10}

        // 5.
        SmButton {
            id: checkCamera
            text: qsTr("Check camera")
            checkable: true
            onCheckedChanged: checked ? TaskCheckCamera.run(currentOptions) : TaskCheckCamera.stopProgram()
            Connections { target: TaskCheckCamera; function onFinished() { checkCamera.checked = false } }
            Layout.preferredWidth: 120
        }

        Item { height: 30; width: 10}
        Item { height: 30; width: 10}
        Item { height: 30; width: 10}

        // 6.
        SmButton {
            id: findBlobs
            text: qsTr("Find blobs")
            checkable: true
            onCheckedChanged: checked ? TaskFindBlob.run(currentOptions, false)
                                      : TaskFindBlob.stopProgram()
            Connections { target: TaskFindBlob; function onFinished() { findBlobs.checked = false } }
            Layout.preferredWidth: 120
        }

        Item { height: 30; width: 10}
        Item { height: 30; width: 10}
        Item { height: 30; width: 10}

        // 7.
        Text {
            text: qsTr("Scene")
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft
            Layout.preferredHeight: 30
            Layout.preferredWidth: 60
        }

        ComboBox {
            id: findBlobScenes
            model: Engine.filesInDirectory("find_blob_scenes")
            Layout.preferredWidth: 180
            onActivated: {
                currentOptions = Engine.readFile("find_blob_scenes" + "/" + currentText)
            }
            onModelChanged: {
                currentOptions = Engine.readFile("find_blob_scenes" + "/" + currentText)
            }

            Component.onCompleted: {
                currentOptions = Engine.readFile("find_blob_scenes" + "/" + currentText)
            }

            Layout.columnSpan: 2
        }

        Button {
            Layout.preferredHeight: 25
            Layout.preferredWidth: 25
            text: ("🖍")
            onClicked: sceneEditor.show()

            ToolTip.visible: hovered
            ToolTip.text: qsTr("Edit scene")

            SceneEditor {
                id: sceneEditor
                options: currentOptions
                onClosed: {
                    Engine.saveFile("find_blob_scenes" + "/" + findBlobScenes.currentText, optionsModified)
                }
            }
        }


        // CheckBox {
        //     id: slowFindBlobs
        //     text: "find blobs slowly"

        //     Layout.preferredHeight: 30
        //     Layout.preferredWidth: 20
        // }
    }
}
