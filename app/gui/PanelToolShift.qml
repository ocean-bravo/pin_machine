import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

CollapsiblePanel {
    id: root

    width: parent.width
    height: checked ? 200 : 25

    Layout.preferredWidth: 400
    Layout.preferredHeight: checked ? 200 : 25

    text: qsTr("Tool Shift")

    property point zeroPoint: Qt.point(0,0)

    onCheckedChanged: {
        column.visible = checked
    }
    Component.onCompleted: {
        column.visible = checked

        DataBus.punch_tool_shift_dx = Settings.value("punch_tool_shift_dx", 85.0);
        DataBus.punch_tool_shift_dy = Settings.value("punch_tool_shift_dy", -90.0);
    }

    ColumnLayout {
        id: column
        anchors.top:parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        Rectangle {
            color: "lightgrey"
            Layout.fillWidth: true
            Layout.preferredHeight: 30

            RowLayout {
                anchors.fill: parent

                Text {
                    text: qsTr("Tool shift")
                    Layout.fillWidth: true
                    verticalAlignment: Text.AlignVCenter
                }

                Text {
                    text: qsTr("dx")
                    verticalAlignment: Text.AlignVCenter
                    Layout.preferredHeight: 30
                    Layout.preferredWidth: 20
                }

                DoubleSpinBox {
                    value: DataBus.punch_tool_shift_dx
                    onValueModified: DataBus.punch_tool_shift_dx = value
                }

                Text {
                    text: qsTr("dy")

                    verticalAlignment: Text.AlignVCenter
                    Layout.preferredHeight: 30
                    Layout.preferredWidth: 20

                }
                DoubleSpinBox {
                    value: DataBus.punch_tool_shift_dy
                    onValueModified: DataBus.punch_tool_shift_dy = value
                }

                SaveButton {
                    acceptFunc: function() {
                        Settings.setValue("punch_tool_shift_dx", DataBus.punch_tool_shift_dx)
                        Settings.setValue("punch_tool_shift_dy", DataBus.punch_tool_shift_dy)
                    }
                }
            }
        }

        SmButton {
            text: qsTr("Save current position as zero")
            Layout.preferredWidth: 200
            onClicked: zeroPoint = Qt.point(xPos, yPos)
        }

        Rectangle {
            color: "lightgrey"
            Layout.fillWidth: true
            Layout.preferredHeight: 30
            RowLayout {
                anchors.fill: parent

                Text {
                    text: qsTr("Delta zero")
                    Layout.fillWidth: true
                    verticalAlignment: Text.AlignVCenter
                }

                Text {
                    text: qsTr("dx0")
                    verticalAlignment: Text.AlignVCenter
                    Layout.preferredHeight: 30
                    Layout.preferredWidth: 20
                }

                DoubleSpinBox {
                    id: deltaZeroX
                    value: xPos - zeroPoint.x
                }

                Text {
                    text: qsTr("dy0")

                    verticalAlignment: Text.AlignVCenter
                    Layout.preferredHeight: 30
                    Layout.preferredWidth: 20

                }
                DoubleSpinBox {
                    id: deltaZeroY
                    value: yPos - zeroPoint.y
                }

                Item {Layout.preferredWidth: 30}
            }
        }

        SmButton {
            text: qsTr("Move to toolshift value")
            Layout.preferredWidth: 200

            onClicked: {
                jog("X", -DataBus.punch_tool_shift_dx)
                jog("Y", -DataBus.punch_tool_shift_dy)
            }
        }

        SaveButton {
            text: qsTr("Save delta zero as tool shift")
            Layout.preferredWidth: 200

            acceptFunc: function() {
                DataBus.punch_tool_shift_dx = deltaZeroX.value
                DataBus.punch_tool_shift_dy = deltaZeroY.value

                Settings.setValue("punch_tool_shift_dx", DataBus.punch_tool_shift_dx)
                Settings.setValue("punch_tool_shift_dy", DataBus.punch_tool_shift_dy)
            }
        }
    }
}
