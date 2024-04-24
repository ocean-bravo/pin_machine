import QtQml 2.15
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

        Rectangle {
            color: "transparent"
            Layout.fillWidth: true
            Layout.preferredHeight: 40

            RowLayout {
                anchors.fill: parent

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

                CheckBox {
                    text: qsTr("Visit camera")
                    font.pixelSize: 12

                    checked: DataBus.taskpunch_visit_camera
                    onCheckedChanged: {
                        DataBus.taskpunch_visit_camera = checked
                        if (checked)
                            TaskPunch.noPunch = true
                    }

                    Layout.preferredHeight: 24
                    Layout.fillWidth: true
                }
            }
        }

        Rectangle {
            color: "transparent"
            Layout.fillWidth: true
            Layout.preferredHeight: 70

            GridLayout {
                anchors.fill: parent
                flow: GridLayout.TopToBottom
                columns: 2
                rows: 2
                columnSpacing: 5

                CheckBox {
                    text: qsTr("Проверка реперных отверстий перед установкой")
                    font.pixelSize: 12

                    Layout.preferredHeight: 24
                    Layout.fillWidth: true
                }

                CheckBox {
                    text: qsTr("Проверка всех отверстий перед установкой")
                    font.pixelSize: 12

                    checked: TaskPunch.checkEveryBlob
                    onCheckedChanged: TaskPunch.checkEveryBlob = checked

                    Layout.preferredHeight: 24
                    Layout.fillWidth: true
                }

                CheckBox {
                    text: qsTr("Без установки")
                    font.pixelSize: 12

                    checked: TaskPunch.noPunch
                    onCheckedChanged: {
                        TaskPunch.noPunch = checked
                        if (!checked)
                            DataBus.taskpunch_visit_camera = false
                    }

                    Layout.preferredHeight: 24
                    Layout.fillWidth: true
                }

                CheckBox {
                    text: qsTr("Пошагово")
                    font.pixelSize: 12

                    checked: TaskPunch.stepByStep
                    onCheckedChanged: TaskPunch.stepByStep = checked

                    Layout.preferredHeight: 24
                    Layout.fillWidth: true
                }
            }
        }

        Rectangle {
            color: "transparent"
            Layout.fillWidth: true
            Layout.preferredHeight: 40

            RowLayout {
                anchors.fill: parent

                ToolButton {
                    icon.source: "images/play.svg"
                    color: colors.success_90
                    enabled: (!TaskPunch.isRunning || TaskPunch.isPaused) && DataBus.homing_status === "Ready"
                    onClicked: {
                        if (DataBus.homing_status !== "Ready")
                            return

                        if (!TaskPunch.isRunning) {
                            let punchCode = Settings.value("punch_code")
                            //TaskPunch.run(punchCode, 800, 600, "YUYV")
                            TaskPunch.run(punchCode, selectedResolution().width, selectedResolution().height, selectedResolution().fourcc)
                            return
                        }

                        if (TaskPunch.isPaused) {
                            TaskPunch.isPaused = false
                            return
                        }
                    }

                    function selectedResolution() {
                        return sortResolutions(DataBus["camera_image_formats_" + cameraList.currentValue])[resolutionListForPunch.currentIndex]
                    }
                }

                ToolButton {
                    icon.source: "images/pause.svg"
                    color: colors.primary_50
                    enabled: TaskPunch.isRunning
                    onClicked: {
                        // Какая логика?
                        TaskPunch.isPaused = true
                    }
                }

                ToolButton {
                    icon.source: "images/stop.svg"
                    color: colors.error_80
                    enabled: TaskPunch.isRunning
                    onClicked: {
                        TaskPunch.stopProgram()
                    }
                }
            }
        }
    }

    component ToolButton : Button {
        id: toolButton

        icon.height: 40
        icon.width: 40
        checkable: true
        display: AbstractButton.IconOnly

        Layout.fillWidth: true
        Layout.preferredHeight: 40

        property color color

        background: Rectangle {
            id: bg
            color: enabled ? down ? "lightgrey" : toolButton.color : colors.disabledButton
            border.width: 0
            radius: 8
        }
    }
}
