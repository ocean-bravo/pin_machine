import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtGraphicalEffects 1.15

import ImageItem 1.0

import "utils.js" as Utils


Item {
    id: root

    RowLayout {
        anchors.fill: parent
        spacing: 8


        Item {
            Layout.preferredWidth: 400
            Layout.fillHeight: true

            ColumnLayout {
                anchors.fill: parent

                spacing: 8

                OpPanelAxesCoordinates2 {
                    Layout.preferredHeight: 480
                    Layout.fillWidth: true
                }

                OpPanelAxesControl {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

            }
        }
        Item {
            Layout.preferredWidth: 400
            Layout.fillHeight: true

            ColumnLayout {
                anchors.fill: parent

                spacing: 8

                OpPanelProduct2 {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }

                OpPanelScan {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }

                OpPanelPunchParameters2 {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }
            }
        }

        OpPanelProductView {
            id: productVisualisation

            Layout.preferredWidth: 1080
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

