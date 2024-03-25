import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

CollapsiblePanel2 {
    id: root

    width: parent.width

    text: qsTr("Punch")

    ColumnLayout {
        id: column
        width: parent.width
        spacing: 3

        Rectangle {
            color: "lightgrey"
            Layout.fillWidth: true
            Layout.preferredHeight: 70

            RowLayout {
                anchors.fill: parent

                Text {
                    text: qsTr("Code")
                    Layout.preferredWidth: 80
                    verticalAlignment: Text.AlignVCenter
                }

                CodeEditor2 {
                    id: punchCode

                    Layout.fillWidth: true
                    Layout.preferredHeight: 70

                    text: Settings.value("punch_code", "G1 G90 F4000 Z20\nG1 G90 F4000 Z-8.0\nG1 G90 F4000 Z0")
                }

                SaveButton {
                    acceptFunc: function() { Settings.setValue("punch_code", punchCode.text) }
                }
            }
        }

        RowLayout {

            SmButton {
                id: punch
                text: qsTr("Punch")
                checkable: true
                Layout.preferredWidth: 70
                onCheckedChanged: checked ? TaskPunch.run(punchCode.text, selectedResolution().width, selectedResolution().height, selectedResolution().fourcc)
                                          : TaskPunch.stopProgram()
                function selectedResolution() {
                    return sortResolutions(DataBus["camera_image_formats_" + cameraList.currentValue])[resolutionListForPunch.currentIndex]
                }
                Connections { target: TaskPunch; function onFinished() { punch.checked = false } }
            }

            ComboBox {
                id: resolutionListForPunch
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
        }

        CheckBox {
            id: slowFindBlobs
            text: "step by step (F8)"
            onCheckedChanged: DataBus.punch_step_by_step = checked
            checked: false

            //Layout.preferredHeight: 30
            //Layout.preferredWidth: 20
        }
    }

    Shortcut {
        sequence: "F8"
        context: Qt.ApplicationShortcut
        onActivated: {
            DataBus.punch_next = "ok"
        }
    }
}
