import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

CollapsiblePanel {
    id: root
    width: parent.width
    height: checked ? 230 : 30

    text: qsTr("Tool Shift")

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
        width: parent.width

        Rectangle {
            color: "lightgrey"
            Layout.fillWidth: true
            Layout.preferredHeight: 30

            RowLayout {
                anchors.fill: parent

                Text {
                    text: qsTr("Tool shift")
                    Layout.preferredWidth: 80
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
                    Layout.preferredWidth: 100
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
                    Layout.preferredWidth: 100
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
            text: qsTr("1. Make mark by tool")
            checkable: true
        }

        SmButton {
            text: qsTr("2. Remember position XY")
            checkable: true
        }

        SmButton {
            text: qsTr("3. Move to punch delta XY")
            checkable: true
        }
    }
}
