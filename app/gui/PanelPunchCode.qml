import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

CollapsiblePanel {
    id: root
    width: parent.width
    height: checked ? 160 : 30
    text: qsTr("Punch")
    onCheckedChanged: {
        column.visible = checked
    }
    Component.onCompleted: {
        column.visible = checked
    }

    ColumnLayout {
        id: column
        width: parent.width

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

        SmButton {
            id: punch
            text: qsTr("Punch")
            checkable: true
            onCheckedChanged: checked ? TaskPunch.run(punchCode.text) : TaskPunch.stopProgram()
            Connections { target: TaskPunch; function onFinished() { punch.checked = false } }
        }
    }
}
