import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

CollapsiblePanel2 {
    id: root

    width: parent.width

    text: qsTr("Machine Commands")

    Column {
        id: column
        width: parent.width

        Grid {
            width: parent.width
            columns: 4
            columnSpacing: 5
            rowSpacing: 5

            SmButton { text: qsTr("$H");  onClicked: { write("$H" )} }
            SmButton { text: qsTr("$HX"); onClicked: { write("$HX" ) } }
            SmButton { text: qsTr("$HY"); onClicked: { write("$HY" ) } }
            SmButton { text: qsTr("$HZ"); onClicked: { write("$HZ" ) } }
        }

        Item { height: 30; width: 10}

        Grid {
            width: parent.width
            columns: 3
            columnSpacing: 5
            rowSpacing: 5

            SmButton { text: qsTr("Unlock($X)"); onClicked: { write("$X" )     } }
            SmButton { text: qsTr("Jog cancel");   onClicked: {  write("\x85" )    } }
            SmButton { text: qsTr("Feed Hold(!)");   onClicked: { write("!" )     } }
            SmButton { text: qsTr("Start/Resume(~)");   onClicked: { write("~" )     } }

            SmButton {
                text: qsTr("Status(?)")
                tooltipText: "F5";
                checkable: true
                checked: true
                onCheckedChanged: checked ? statusTimer.start() : statusTimer.stop()

                Timer {
                    id: statusTimer
                    interval: 500
                    repeat: true
                    triggeredOnStart: true
                    running: true
                    onTriggered: Serial.write("?\n")
                }
            }

            ComboBox {
                height: 30
                model: ["$Alarm/Disable", "$Alarms/List", "$Build/Info", "$Bye", "$Commands/List", "$Limits/Show", "$Errors/List ",
                    "$Firmware/Info", "$GCode/Modes", "$Heap/Show", "$Help", "$Settings/List", "$Startup/Show"]
                onActivated: write(currentText)
            }

            SmButton { text: qsTr("Soft Reset(ctrl+x)"); onClicked: { write("\x18" )       } }
        }
    }
}
