import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import Process 1.0

import "utils.js" as Utils

Item {
    id: root

    function write(msg) {
        Serial.write(msg)
        appendLog(msg)
    }

    function appendLog(msg) {
        msg = msg.replace(/\r?\n/g, '<br>')
        logViewer.append("<font color='red'>" + msg + "</font>")
    }

    property string status: "Idle"
    property string fullStatus

    Connections {
        target: Serial
        function onMessage(msg) {
            logViewer.append("<font color='darkgrey'>" + msg + "</font><br>")
        }
    }

    Connections {
        target: Serial

        property string prevMsg: ""

        function onData(msg) {
            msg = prevMsg + msg

            // Нет перевода строки - копим сообщения дальше
            if (!msg.match(/\r?\n/)) {
                prevMsg = msg
                return
            }

            // Перевод строки есть, разбираем сообщение
            prevMsg = ""

            var messages = msg.split(/\r?\n/)

            // Если последняя стрка пустая - значит сообщение закончилось переводом строки.
            // pop() модифицирует массив
            var last = messages.pop()
            // Если последняя стрка непустая - значит сообщение не закончилось (перевода строки не было). Сохраняем его.
            if (last !== '') {
                prevMsg = last
            }

            nextMessage: while (messages.length > 0) {
                msg = messages.shift()

                // Не выводим ответ ok
                if (msg === 'ok')
                    continue

                // Симвлы < и > есть во входящих данных. Они интерпретируются как Html. Надо заменить на другие.
                msg = msg.replace(/</g, '[')
                msg = msg.replace(/>/g, ']')

                for (let k = 0; k < modes.length; ++k) {
                    let stat = modes[k]
                    if (msg.includes(stat)) {
                        status = stat
                        fullStatus = msg
                        //continue nextMessage
                    }
                }

                //            for (let i = 1; i < 11; ++i) {
                //                let alrm = "ALARM:" + i
                //                if (msg.includes(alrm))
                //                    msg = msg.replace(new RegExp(alrm,'g'), alrm +  ' [' + alarms[i] + ']')
                //            }
                for (let j = 1; j < 100; ++j) {
                    let err = "error:" + j
                    if (msg.includes(err))
                        msg = msg.replace(new RegExp(err,'g'), err +  ' [' + errors[j] + ']')
                }
                logViewer.append("<font color='darkblue'>" + msg + "</font><br>")
            }
        }
    }

    QMLPromises {
        id: cycle
        function runAsync() {
            asyncToGenerator( function* () {
                while (true) {
                    sendCodeObj.sendNextLine()
                    status = "Wait"
                    yield waitUntil({target: root, property: "status", value: "Idle"})
                    appendLog("capturing ...\n")
                    Video4.capture()
                    yield waitForSignal(Video4.captured)
                    appendLog("captured\n")

                    if (sendCodeObj.lineToSend >= sendCodeObj.codeLines.length) {
                        sendCodeObj.stopProgram()
                        return
                    }
                }
            } )();
        }
    }

    Item {
        id: sendCodeObj

        property var codeLines: []
        property int lineToSend: 0

        function sendNextLine() {
            var line = codeLines[lineToSend]
            console.log(line)
            Serial.write(line)
            let lineNumber = lineToSend+1
            var msg = "" + lineNumber + ": " + line + "\n"
            appendLog(msg)
            ++lineToSend
        }

        function startProgram() {
            lineToSend = 0
            status = "Wait"
            codeEditor.readOnly = true
            sendCodeObj.codeLines = codeEditor.text.split("\n")

            statusTimer.interval = 100
            statusTimer.start()

            cycle.runAsync()
        }

        function startResumeProgram() {
            if (lineToSend === 0) {
                status = "Wait"
                codeEditor.readOnly = true
                sendCodeObj.codeLines = codeEditor.text.split("\n")

            }

//            sendCodeTimer.interval = 20 // Замедление программы
//            sendCodeTimer.start()

            statusTimer.interval = 100
            statusTimer.start()

            playPauseProgram.text = qsTr("Pause program")
        }

        function pauseProgram() {
            //sendCodeTimer.stop()
            statusTimer.stop()
            playPauseProgram.text = qsTr("Resume program")
        }

        function stopProgram() {
            pauseProgram()
            lineToSend = 0
            codeEditor.readOnly = false
            playPauseProgram.checked = false
            playPauseProgram.text = qsTr("Run program")
        }
    }

    RowLayout {
        anchors.fill: parent
        Column {
            Layout.preferredWidth: 400
            Layout.fillHeight: true
            Grid {
                width:parent.width
                columns: 4
                columnSpacing: 5
                rowSpacing: 5

                SmTextEdit {
                    id: serialPortName
                    span: 2

                    Process {
                        id: findUsbProcess
                        onReadyRead: {
                            const result = readAll()
                            serialPortName.text = ("/dev/" + result).replace(/\s/g, "")
                        }
                        onFinished: {
                            findUsbTimer.start()
                        }
                    }
                    Timer {
                        id: findUsbTimer
                        interval: 1000
                        repeat: false
                        triggeredOnStart: false
                        running: true
                        onTriggered: {
                            findUsbProcess.start("/bin/sh", ["-c", "ls /dev | grep ttyUSB"]);
                            serialPortName.text = ""
                        }
                    }
                }

                SmButton {
                    id: openPort
                    text: qsTr("Open")
                    onClicked: {
                        Serial.close()
                        let serPort = serialPortName.text
                        console.log(serPort)
                        Serial.setPortName(serPort)
                        Serial.setBaudRate("115200")
                        Serial.setDataBits("8")
                        Serial.setParity("N")
                        Serial.setStopBits("1")
                        Serial.open()
                    }
                }

                SmButton { text: qsTr("Close");  onClicked: { Serial.close() } }
            }

            Item { height: 30; width: 10}

            Grid {
                width: parent.width
                columns: 4
                columnSpacing: 5
                rowSpacing: 5

                SmButton { text: qsTr("$H");  onClicked: { write("$H\n" )} }
                SmButton { text: qsTr("$HX"); onClicked: { write("$HX\n" ) } }
                SmButton { text: qsTr("$HY"); onClicked: { write("$HY\n" ) } }
                SmButton { text: qsTr("$HZ"); onClicked: { write("$HZ\n" ) } }
            }

            Item { height: 30; width: 10}
            Text {
                text: fullStatus
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }


            Grid {
                width: parent.width
                columns: 3
                columnSpacing: 5
                rowSpacing: 5

                SmButton { text: qsTr("Unlock($X)"); onClicked: { write("$X\n" )     } }
                SmButton { text: qsTr("Jog cancel");   onClicked: {  write("\x85\n" )    } }
                SmButton { text: qsTr("Feed Hold(!)");   onClicked: { write("!\n" )     } }
                SmButton { text: qsTr("Start/Resume(~)");   onClicked: { write("~\n" )     } }
                SmButton { text: qsTr("");   onClicked: {      } }

                SmButton {
                    text: qsTr("Status(?)")
                    tooltipText: "F5";
                    checkable: true
                    onCheckedChanged: {
                        if (checked)
                            statusTimer.start()
                        else
                            statusTimer.stop()
                    }

                    Timer {
                        id: statusTimer
                        interval: 500
                        repeat: true
                        triggeredOnStart: true
                        running: false
                        onTriggered: {
                            Serial.write("?\n")
                        }
                    }

                }

                ComboBox {
                    height: 30
                    model: ListModel {
                        ListElement { text: "$Alarm/Disable" }
                        ListElement { text: "$Alarms/List" }
                        ListElement { text: "$Build/Info" }
                        ListElement { text: "$Bye" }
                        ListElement { text: "$Commands/List" }
                        ListElement { text: "$Errors/List " }
                        ListElement { text: "$Firmware/Info" }
                        ListElement { text: "$GCode/Modes" }
                        ListElement { text: "$Heap/Show" }
                        ListElement { text: "$Help" }
                        ListElement { text: "$Settings/List" }
                        ListElement { text: "$Startup/Show" }
                    }

                    onActivated: {
                        write(textAt(index) + "\n")
                    }
                }

                SmButton { text: qsTr("Soft Reset(ctrl+x)"); onClicked: { write("\x18\n" )       } }
                Text {
                    text: status
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                //                Item { height: 20; width: 10}
                //                Item { height: 20; width: 10}

                Item { height: 20; width: 10}
                Item { height: 20; width: 10}
                Item { height: 20; width: 10}

                SmTextEdit { id: moveX}
                SmButton { text: qsTr("Move X");     onClicked: {write("G1 G90 F2000 X" + moveX.text + "\n") } }
                SmButton {  }


                SmTextEdit { id: moveY}
                SmButton { text: qsTr("Move Y");     onClicked: {write("G1 G90 F2000 Y" + moveY.text + "\n") } }
                Item { height: 20; width: 10}

                SmTextEdit { id: moveZ}
                SmButton { text: qsTr("Move Z");     onClicked: {write("G1 G90 F2000 Z" + moveZ.text + "\n") } }
                Item { height: 20; width: 10}


                SmTextEdit { id: sendText;  GridLayout.columnSpan: 2; Layout.fillWidth: true}
                SmButton { text: qsTr("Send");       onClicked: { write(sendText.text + "\n") } }

                Item { height: 30; width: 10}
                Item { height: 30; width: 10}

                Item { height: 30; width: 10}
                SmButton { text: qsTr("Clear log");
                    onClicked:
                    {
                        logViewer.clear()
                    }
                }
                SmButton {
                    id: playPauseProgram
                    text: checked ? qsTr("Pause program") : qsTr("Run program")
                    onCheckedChanged: checked ? sendCodeObj.startProgram() : sendCodeObj.pauseProgram()
                    checkable: true
                }
                SmButton {
                    text: qsTr("Stop program")
                    onClicked: {
                        playPauseProgram.checked = false
                        sendCodeObj.stopProgram()
                    }
                }
                SmButton {
                    text: qsTr("Generate program")
                    onClicked: {
                        codeEditor.clear()
                        codeEditor.append(Utils.generateSteps(0, 100, 200, 200, 6.25, 4.25, 5000).join("\n"))
                    }
                }
            }

            JogControl {
                width: parent.width
            }
        }

        SplitView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Column {
                SplitView.minimumWidth: 50
                SplitView.preferredWidth: root.width / 3
                SplitView.maximumWidth: 800

                Log {
                    id: logViewer
                    height: parent.height / 2
                    width: parent.width
                }
                CodeEditor2 {
                    id: codeEditor
                    height: parent.height / 2
                    width: parent.width
                }
            }

            ImageDoubleBuff2 {
                id: image
                width: parent.width
                height: parent.height

                Connections {
                    target: Engine
                    function onImageChanged(id) {
                        if (id !== imgType.currentText)
                            return
                        image.setSource("image://camera/" +  id)
                    }
                }

                Column {
                    spacing: 5
                    Button {
                        id: startStopUpdate
                        width: 200
                        text: checked ? qsTr("Stop update") : qsTr("Start update")
                        checkable: true
                        checked: false
                        onCheckedChanged: {
                            if (checked) {
                                resList.setCurrentFormat()
                                Video4.start()
                            }
                            else
                                Video4.stop()
                        }
                    }

                    Text {
                        anchors.top: startStopUpdate.top
                        anchors.left: reloadDevices.right
                        text: DataBus.blob_info
                    }

                    Button {
                        id: reloadDevices
                        width: 200
                        text: qsTr("Reload devices")
                        onPressed: {
                            Video3.reloadDevices()
                        }
                    }

                    Button {
                        id: captureFrame
                        width: 200
                        text: qsTr("Capture frame")
                        onPressed: {
                            //Video4.capture()
                            cycle.runAsync()
                        }
                    }

                    ComboBox {
                        id: imgType
                        width: 200
                        model: ListModel {
                            ListElement { text: "raw" }
                            ListElement { text: "circle" }
                            ListElement { text: "blob" }
                            ListElement { text: "raw captured" }
                        }
                        onActivated: {
                            image.setSource("image://camera/" + textAt(index))
                            DataBus.mode = textAt(index)
                        }
                    }

                    ComboBox {
                        width: 200
                        id: cameraList
                        model: DataBus.cameras
                        onActivated: {
                            resList.model = DataBus["camera" + index]
                            let md = resList.model[0]
                            Video4.changeCamera(index*2, md.width, md.height, md.fourcc)
                        }
                        onModelChanged: {
                            resList.model = DataBus["camera" + 0]
                            //Video4.changeCamera(0, 640, 480, "YUYV")
                        }
                    }

                    ComboBox {
                        id: resList
                        width: 200
                        textRole: "display"
                        onActivated: {
                            setCurrentFormat()
                        }
                        function setCurrentFormat() {
                            let md = model[currentIndex]
                            Video4.changeCamera(cameraList.currentIndex*2, md.width, md.height, md.fourcc)
                        }
                    }
                }
                CircleSettings {
                    visible: imgType.currentText === "circle"
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
                }
                BlobSettings {
                    visible: imgType.currentText === "blob"
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
                }
            }
        }
    }



    Shortcut {
        sequence: "F5"
        context: Qt.ApplicationShortcut
        onActivated: write("?\n")
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
}
