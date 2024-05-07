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
            Layout.preferredWidth: 480
            Layout.fillHeight: true

            ColumnLayout {
                id: column1
                anchors.fill: parent

                spacing: 8

                OpPanelAxesCoordinates2 {
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                }

                OpPanelAxesHoming2 {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }

                OpPanelCameraView2 {
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                }

                OpPanelToolShift {
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                }
            }
        }

        OpPanelMove {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }

        Item {
            Layout.preferredWidth: 550
            Layout.fillHeight: true

            ColumnLayout {
                anchors.fill: parent

                spacing: 8

                OpPanelPunchParameters {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }

                OpPanelStepByStepPunch {
                    Layout.preferredHeight: 218
                    Layout.fillWidth: true
                }

            }
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

