import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import "utils.js" as Utils


Item {
    id: root

    RowLayout {
        anchors.fill: parent
        spacing: 8

        Item {
            Layout.preferredWidth: 816
            Layout.fillHeight: true

            ColumnLayout {
                id: tools1
                anchors.fill: parent

                spacing: 8

                Item {
                    Layout.preferredWidth: 816
                    Layout.fillHeight: true

                    RowLayout {
                        id: row1
                        anchors.fill: parent
                        spacing: 8

                        Item {
                            Layout.fillHeight: true
                            Layout.preferredWidth: 404

                            ColumnLayout  {
                                id: column1
                                anchors.fill: parent

                                spacing: 8

                                Item {
                                    id: logo

                                    Layout.preferredHeight: 112
                                    Layout.fillWidth: true

                                    Image {
                                        anchors.left: parent.left
                                        anchors.verticalCenter: parent.verticalCenter
                                        source: "images/logo.png"
                                    }
                                }

                                OpPanelCameraView {
                                    id: camera

                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                }
                            }
                        }

                        Item {
                            Layout.fillHeight: true
                            Layout.preferredWidth: 404
                            ColumnLayout {
                                id: column2

                                spacing: 8

                                anchors.fill: parent

                                OpPanelProduct {
                                    id: product

                                    Layout.preferredHeight: 172
                                    Layout.fillWidth: true
                                }

                                OpPanelAxesHoming {
                                    id: axesHoming
                                    Layout.preferredHeight: 124
                                    Layout.fillWidth: true
                                }

                                OpPanelAxesCoordinates {
                                    id: axes
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                }
                            }
                        }
                    }
                }

                Item {
                    Layout.preferredWidth: 816
                    Layout.preferredHeight: 186

                    RowLayout {
                        id: row2
                        anchors.fill: parent
                        spacing: 8

                        OpPanelCameraOptions {
                            id: cameraOption
                            Layout.fillHeight: true
                            Layout.preferredWidth: 336
                        }

                        OpPanelWorkModes {
                            id: workModes
                            Layout.fillHeight: true
                            Layout.preferredWidth: 236
                        }

                        OpPanelInsertParameters {
                            id: insertParameters
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                        }
                    }
                }

                OpWhitePanel {
                    id: buttons

                    Layout.fillWidth: true
                    Layout.preferredHeight: 124

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 16
                        spacing: 32

                        ToolButton {
                            icon.source: "images/play.svg"
                            color: colors.success_90
                            enabled: (!TaskPunch.isRunning || TaskPunch.isPaused) && DataBus.homing_status === "Ready"
                            onClicked: {
                                if (DataBus.homing_status !== "Ready")
                                    return

                                if (!TaskPunch.isRunning) {
                                    let punchCode = Settings.value("punch_code")
                                    TaskPunch.run(punchCode, 800, 600, "YUYV")
                                    return
                                }

                                if (TaskPunch.isPaused) {
                                    TaskPunch.isPaused = false
                                    return
                                }
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
        }
        OpPanelProductView {
            id: productVisualisation
            objName: "placeholderForGraphicsView1"

            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }

    component ToolButton : Button {
        icon.height: 80
        icon.width: 80
        checkable: true
        display: AbstractButton.IconOnly

        Layout.fillWidth: true
        Layout.preferredHeight: 100

        property color color

        background: Rectangle {
            id: bg
            color: enabled ? down ? "lightgrey" : color : colors.disabledButton
            border.width: 0
            radius: 8
        }
    }

}




