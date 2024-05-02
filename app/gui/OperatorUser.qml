import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

import "utils.js" as Utils

Control {
    id: root

    background: Rectangle {
        color: colors.primary_10
    }

    function write(msg) {
        // Во время хоуминга разрешаю только команду запроса статуса
        if (status === "Home" && msg !== "?")
            return

        Serial.write(msg + "\n")
        appendLog(msg)
    }

    function appendLog(message, color) {
        if (message.length === 0)
            return

        // Подготовил к показу
        message = message.replace(/</g, '|')
        message = message.replace(/>/g, '|')
        message = message.replace(/\r?\n/g, '<br>')
        message = message.replace(/<br>$/g, '') // Убираю последний перенос строки

        machineLog.append(coloredMessage(message, color))
    }

    Connections { target: Serial;            function onMessage(msg) { appendLog(msg, 'lightgrey') } }
    Connections { target: TaskScan;          function onMessage(msg) { appendLog(msg) } }
    Connections { target: TaskUpdate;        function onMessage(msg) { appendLog(msg) } }
    Connections { target: TaskCheckCamera;   function onMessage(msg) { appendLog(msg) } }
    Connections { target: TaskPunch;         function onMessage(msg) { appendLog(msg) } }
    Connections { target: TaskFindPixelSize; function onMessage(msg) { appendLog(msg) } }
    // Connections { target: TaskBestPath;      function onMessage(msg) { appendLog(msg + '<br>') } }
    Connections { target: TaskFindBlob;      function onMessage(msg) { appendLog(msg) } }

    Connections {
        target: DataBus
        property string prevValue: ""
        function onValueChanged (key, value) {
            if (key !== "status")
                return

            if (value === "Alarm")
                DataBus.homing_status = "Not ready"

            if (value === "Home")
                DataBus.homing_status = "In progress"

            if (value === "Idle" && prevValue === "Home" && DataBus.x_coord === "0.000" && DataBus.y_coord === "0.000" && DataBus.z_coord === "0.000")
                DataBus.homing_status = "Ready"

            prevValue = value
        }
    }

    Connections {
        target: Serial
        function onConnected()    { statusText.text = "Порт открыт" }
        function onDisconnected() { statusText.text = "Порт закрыт" }
    }


    Window {
        id: machineLogWindow
        title: "Machine log"
        flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint

        width: MainWindow.width / 3
        height: MainWindow.height / 2

        x: Screen.width - width
        y: 0

        Log2 {
            id: machineLog
            anchors.fill: parent
            loadOnCompleted: true
        }

        Shortcut {
            sequence: "F11"
            context: Qt.ApplicationShortcut
            onActivated: {
                machineLogWindow.visible = !machineLogWindow.visible
                if (machineLogWindow.visible)
                    machineLogWindow.raise()
            }
        }
    }

    Control {
        anchors.fill: parent
        padding: 8

        contentItem: ColumnLayout {

            spacing: 8

            StackLayout {
                id: stackLayout
                Layout.fillWidth: true
                Layout.fillHeight: true

                PanelWork {
                    id: panelWork
                    //anchors.fill: parent
                    //visible: true
                }

                PanelManualControl {
                    id: panelManualControl
                    //anchors.fill: parent
                    //visible: false
                }
            }

            Rectangle {
                id: statusBar

                OpText {
                    id: statusText
                    anchors.fill: parent
                    anchors.margins: 16
                    text: "Порт не открыт"
                    font.pixelSize: 16
                    font.weight: Font.Medium
                    color: "white"
                }

                Layout.fillWidth: true
                Layout.preferredHeight: 56

                color: colors.primary_70
                radius: 8
            }
        }
    }

    Shortcut {
        sequence: "F3"
        context: Qt.ApplicationShortcut
        onActivated: {
            if (stackLayout.currentIndex === 0)
                stackLayout.currentIndex = 1
            else
                stackLayout.currentIndex = 0
        }
    }

    property variant modes: ["Idle", "Alarm", "Check", "Home", "Run", "Jog", "Hold:0", "Hold:1",
        "Door", "Door:0", "Door:1", "Door:2", "Door:3", "Sleep" ]


    property variant alarms :  {
        1: "Hard limit triggered. Machine position is likely lost due to sudden and immediate halt. Re-homing is highly recommended.",
        2: "G-code motion target exceeds machine travel. Machine position safely retained. Alarm may be unlocked.",
        3: "Reset while in motion. Grbl cannot guarantee position. Lost steps are likely. Re-homing is highly recommended.",
        4: "Probe fail. The probe is not in the expected initial state before starting probe cycle, where G38.2 and G38.3 is not triggered and G38.4 and G38.5 is triggered.",
        5: "Probe fail. Probe did not contact the workpiece within the programmed travel for G38.2 and G38.4.",
        6: "Homing fail. Reset during active homing cycle.",
        7: "Homing fail. Safety door was opened during active homing cycle.",
        8: "Homing fail. Cycle failed to clear limit switch when pulling off. Try increasing pull-off setting or check wiring.",
        9: "Homing fail. Could not find limit switch within search distance. Defined as 1.5 * max_travel on search and 5 * pulloff on locate phases",
        10: "Homing fail. On dual axis machines, could not find the second limit switch for self-squaring."
    }

    property variant errors :  {
        0: "No error",
        1: "Expected GCodecommand letter",
        2: "Bad GCode number format",
        3: "Invalid $ statement",
        4: "Negative value",
        5: "Setting disabled",
        6: "Step pulse too short",
        7: "Failed to read settings",
        8: "Command requires idle state",
        9: "GCode cannot be executed in lock or alarm state",
        10: "Soft limit error",
        11: "Line too long",
        12: "Max step rate exceeded",
        13: "Check door",
        14: "Startup line too long",
        15: "Max travel exceeded during jog",
        16: "Invalid jog command",
        17: "Laser mode requires PWM output",
        18: "No Homing/Cycle defined in settings",
        19: "Single axis homing not allowed",
        20: "Unsupported GCode command",
        21: "Gcode modal group violation",
        22: "Gcode undefined feed rate",
        23: "Gcode command value not integer",
        24: "Gcode axis command conflict",
        25: "Gcode word repeated",
        26: "Gcode no axis words",
        27: "Gcode invalid line number",
        28: "Gcode value word missing",
        29: "Gcode unsupported coordinate system",
        30: "Gcode G53 invalid motion mode",
        31: "Gcode extra axis words",
        32: "Gcode no axis words in plane",
        33: "Gcode invalid target",
        34: "Gcode arc radius error",
        35: "Gcode no offsets in plane",
        36: "Gcode unused words",
        37: "Gcode G43 dynamic axis error",
        38: "Gcode max value exceeded",
        39: "P param max exceeded",
        40: "Check control pins",
        60: "Failed to mount device",
        61: "Read failed",
        62: "Failed to open directory",
        63: "Directory not found",
        64: "File empty",
        65: "File not found",
        66: "Failed to open file",
        67: "Device is busy",
        68: "Failed to delete directory",
        69: "Failed to delete file",
        70: "Failed to rename file",
        80: "Number out of range for setting",
        81: "Invalid value for setting",
        82: "Failed to create file",
        83: "Failed to format filesystem",
        90: "Failed to send message",
        100: "Failed to store setting",
        101: "Failed to get setting status",
        110: "Authentication failed!",
        111: "End of line",
        112: "End of file",
        120: "Another interface is busy",
        130: "Jog Cancelled",
        150: "Bad Pin Specification",
        152: "Configuration is invalid. Check boot messages for ERR's.",
        160: "File Upload Failed",
        161: "File Download Failed",
    }


    property variant settings :  {
        0:	"Step pulse, microseconds",
        1:	"Step idle delay, milliseconds",
        2:	"Step port invert, mask",
        3:	"Direction port invert, mask",
        4:	"Step enable invert, boolean",
        5:	"Limit pins invert, boolean",
        6:	"Probe pin invert, boolean",
        10:	"Status report, mask",
        11:	"Junction deviation, mm",
        12:	"Arc tolerance, mm",
        13:	"Report inches, boolean",
        20:	"Soft limits, boolean",
        21:	"Hard limits, boolean",
        22:	"Homing cycle, boolean",
        23:	"Homing dir invert, mask",
        24:	"Homing feed, mm/min",
        25:	"Homing seek, mm/min",
        26:	"Homing debounce, milliseconds",
        27:	"Homing pull-off, mm",
        30:	"Max spindle speed, RPM",
        31:	"Min spindle speed, RPM",
        32:	"Laser mode, boolean",
        100:	"Axis 1 (X) steps/unit (mm or deg)",
        101:	"Axis 2 (Y) steps/unit",
        102:	"Axis 3 (Z) steps/unit",
        103:	"Axis 4 (A) steps/unit",
        104:	"Axis 5 (B) steps/unit",
        105:	"Axis 6 (C) steps/unit",
        110:	"Axis 1 (X) Max rate, unit/min",
        111:	"Axis 2 (Y) Max rate, unit/min",
        112:	"Axis 3 (Z) Max rate, unit/min",
        113:	"Axis 4 (A) Max rate, unit/min",
        114:	"Axis 5 (B) Max rate, unit/min",
        115:	"Axis 6 (C) Max rate, unit/min",
        120:	"Axis 1 (X) Acceleration, unit/sec^2",
        121:	"Axis 2 (Y) Acceleration, unit/sec^2",
        122:	"Axis 3 (Z) Acceleration, unit/sec^2",
        123:	"Axis 4 (A) Acceleration, unit/sec^2",
        124:	"Axis 5 (B) Acceleration, unit/sec^2",
        125:	"Axis 6 (c) Acceleration, unit/sec^2",
        130:	"Axis 1 (X) Max travel, unit",
        131:	"Axis 2 (Y) Max travel, unit",
        132:	"Axis 3 (Z) Max travel, unit",
        133:	"Axis 4 (A) Max travel, unit",
        134:	"Axis 5 (B) Max travel, unit",
        135:	"Axis 6 (C) Max travel, unit"
    }

    Component.onCompleted: {
        Video4.reloadDevices()
    }
}
