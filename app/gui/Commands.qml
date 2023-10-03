import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import Process 1.0

import "utils.js" as Utils

Item {
    id: root

    property string status: "Idle"
    property string fullStatus

    function write(msg) {
        Serial.write(msg+"\n")
        appendLog(msg+"\n")
    }

    function appendLog(msg) {
        msg = msg.replace(/\r?\n/g, '<br>')

        msg = String(Date.now()).slice(-4) + ": " + msg
        logViewer.append("<font color='red'>" + msg + "</font>")
    }

    function moveTo(x, y) {
        write("G1 G90 F5000 X" + x + " Y" + y)
    }


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
            let currentTime = String(Date.now()).slice(-4)

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
                msg = msg.replace(/</g, '|')
                msg = msg.replace(/>/g, '|')

                // Если это статус - разделить его на части
                if (msg.match(/^[|].+[|]$/)) {
                    let statusValues = msg.split("|")
                    status = statusValues[1] // первый элемент будет пустой. Второй как раз статус
                    let position = statusValues[2] // третий элемент - позиция
                    let pos = position.split(":")[1].split(",") // Позиция выглядит так: MPos:0.000,121.250,0.000
                    DataBus.x_coord = pos[0]
                    DataBus.y_coord = pos[1]
                    fullStatus = "[" + DataBus.x_coord + " " + DataBus.y_coord + "]"
                }

                //                for (let k = 0; k < modes.length; ++k) {
                //                    let stat = modes[k]
                //                    if (msg.includes(stat)) {
                //                        status = stat
                //                        fullStatus = msg
                //                        //continue nextMessage
                //                    }
                //                }

                //            for (let i = 1; i < 11; ++i) {
                //                let alrm = "ALARM:" + i
                //                if (msg.includes(alrm))
                //                    msg = msg.replace(new RegExp(alrm,'g'), alrm +  ' [' + alarms[i] + ']')
                //            }
                //                for (let j = 1; j < 100; ++j) {
                //                    let err = "error:" + j
                //                    if (msg.includes(err))
                //                        msg = msg.replace(new RegExp(err,'g'), err +  ' [' + errors[j] + ']')
                //                }


                msg = currentTime + ": " + msg
                logViewer.append("<font color='darkblue'>" + msg + "</font><br>")
            }
        }
    }

    QMLPromises {
        id: cycle

        property var codeLines: []
        property int lineToSend: 0

        function sendNextLine() {
            let line = codeLines[lineToSend]
            let lineNumber = lineToSend+1

            // Пропускаю пустые строки
            if (line.length === 0) {
                var msg = "" + lineNumber + ": " + "skip..." + "\n"
            }
            else {
                Serial.write(line)
                msg = "" + lineNumber + ": " + line + "\n"
            }

            appendLog(msg)
            ++lineToSend

            return line.length > 0
        }

        function startProgram() {
            cycle.runAsync()
        }

        function pauseProgram() {
            statusTimer.stop()
            playPauseProgram.text = qsTr("Resume program")
        }

        function stopProgram() {
            cycle.abort()

            statusTimer.stop()
            playPauseProgram.checked = false
            playPauseProgram.text = qsTr("Run program")

            codeEditor.readOnly = false
        }

        function runAsync() {
            asyncToGenerator( function* () {

                lineToSend = 0
                codeEditor.readOnly = true
                codeLines = codeEditor.text.split("\n")

                statusTimer.interval = 100
                statusTimer.start()

                DataBus.capture_number = 0
                ImagesStorage.clearCaptured()
                OpenCv.resetFoundBlobs()

                yield sleep(200)

                while (true) {
                    if (sendNextLine()) { // Если строка пустая, никаких действий после нее не надо делать
                        yield sleep(200)
                        status = "Wait"
                        yield waitUntil({target: root, property: "status", value: "Idle"})

                        appendLog("capturing ...\n")
                        Video4.capture()
                        yield waitForSignal(Video4.captured)
                        appendLog("captured\n")

                        // Дать обработаться захвату, получить номер capture_number и потом его инкрементировать
                        yield sleep(1)
                        DataBus.capture_number += 1
                    }

                    if (lineToSend >= codeLines.length) {
                        stopProgram()
                        appendLog("program finished\n")
                        return
                    }
                }
            } )();
        }
    }

//    Item {
//        id: sendCodeObj

//        property var codeLines: []
//        property int lineToSend: 0

//        function sendNextLine() {
//            let line = codeLines[lineToSend]

//            let lineNumber = lineToSend+1

//            // Пропускаю пустые строки
//            if (line.length === 0) {
//                var msg = "" + lineNumber + ": " + "skip..." + "\n"
//            }
//            else {
//                Serial.write(line)
//                msg = "" + lineNumber + ": " + line + "\n"
//            }

//            appendLog(msg)
//            ++lineToSend

//            return line.length > 0
//        }

//        function startProgram() {
//            cycle.runAsync()
//        }

//        function pauseProgram() {
//            statusTimer.stop()
//            playPauseProgram.text = qsTr("Resume program")
//        }

//        function stopProgram() {
//            cycle.abort()

//            statusTimer.stop()
//            playPauseProgram.checked = false
//            playPauseProgram.text = qsTr("Run program")

//            codeEditor.readOnly = false
//        }
//    }

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

                    FindUsb {
                        onUsbFound: serialPortName.text = device
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

                SmButton { text: qsTr("$H");  onClicked: { write("$H" )} }
                SmButton { text: qsTr("$HX"); onClicked: { write("$HX" ) } }
                SmButton { text: qsTr("$HY"); onClicked: { write("$HY" ) } }
                SmButton { text: qsTr("$HZ"); onClicked: { write("$HZ" ) } }
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

                SmButton { text: qsTr("Unlock($X)"); onClicked: { write("$X" )     } }
                SmButton { text: qsTr("Jog cancel");   onClicked: {  write("\x85" )    } }
                SmButton { text: qsTr("Feed Hold(!)");   onClicked: { write("!" )     } }
                SmButton { text: qsTr("Start/Resume(~)");   onClicked: { write("~" )     } }
                SmButton { text: qsTr("");   onClicked: {      } }

                SmButton {
                    text: qsTr("Status(?)")
                    tooltipText: "F5";
                    checkable: true
                    onCheckedChanged: checked ? statusTimer.start() : statusTimer.stop()

                    Timer {
                        id: statusTimer
                        interval: 500
                        repeat: true
                        triggeredOnStart: true
                        running: false
                        onTriggered: Serial.write("?\n")
                    }
                }

                ComboBox {
                    height: 30
                    model: ["$Alarm/Disable", "$Alarms/List", "$Build/Info", "$Bye", "$Commands/List", "$Errors/List ",
                        "$Firmware/Info", "$GCode/Modes", "$Heap/Show", "$Help", "$Settings/List", "$Startup/Show"]
                    onActivated: write(currentText)
                }

                SmButton { text: qsTr("Soft Reset(ctrl+x)"); onClicked: { write("\x18" )       } }
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

                DoubleSpinBox { id: moveX; }
                SmButton { text: qsTr("Move X");
                    onClicked: {
                        write("G1 " + (relAbsX.text === "Abs" ? "G90" : "G91") + " F1000 X" + moveX.text) }
                }
                SmButton { id: relAbsX; checkable: true; text: checked ? "Rel" : "Abs"; width: 50 }

                DoubleSpinBox { id: moveY; }
                SmButton { text: qsTr("Move Y");
                    onClicked: {
                        write("G1 " + (relAbsY.text === "Abs" ? "G90" : "G91") + " F1000 Y" + moveY.text) }
                }
                SmButton { id: relAbsY; checkable: true; text: checked ? "Rel" : "Abs"; width: 50 }

                DoubleSpinBox { id: moveZ; }
                SmButton { text: qsTr("Move Z");
                    onClicked: {
                        write("G1 " + (relAbsZ.text === "Abs" ? "G90" : "G91") + " F1000 Z" + moveZ.text) }
                }
                SmButton { id: relAbsZ; checkable: true; text: checked ? "Rel" : "Abs"; width: 50  }

                //Item { height: 20; width: 10}


                SmTextEdit { id: sendText;  GridLayout.columnSpan: 2; Layout.fillWidth: true}
                SmButton { text: qsTr("Send");       onClicked: { write(sendText.text) } }

                Item { height: 30; width: 10}

                SmButton { text: qsTr("Idle");       onClicked: { status = "Idle" } }
                SmButton { text: qsTr("Wait");       onClicked: { status = "Wait" } }
                SmButton { text: qsTr("Clear log");  onClicked: { logViewer.clear() } }
                SmButton {
                    id: playPauseProgram
                    text: checked ? qsTr("Pause program") : qsTr("Run program")
                    onCheckedChanged: checked ? cycle.startProgram() : cycle.pauseProgram()
                    checkable: true
                }
                SmButton {
                    text: qsTr("Stop program")
                    onClicked: {
                        playPauseProgram.checked = false
                        cycle.stopProgram()
                    }
                }

            }

            JogControl {
                width: parent.width
            }
            Grid {
                width: parent.width
                //height: 100
                columns: 3
                SmButton {
                    text: qsTr("Print blobs1");
                    onClicked: {
                        let foundBlobs = DataBus.found_blobs

                        if (foundBlobs === undefined)
                            return

                        foundBlobs = foundBlobs.replace(/\t/g, '&nbsp;&nbsp;&nbsp;&nbsp;').replace(/\n/g, '<br>')
                        logViewer.append(foundBlobs)
                    }
                }
                SmButton {
                    text: qsTr("Print blobs2");
                    onClicked: {
                        let foundBlobs = DataBus.found_blobs2

                        if (foundBlobs === undefined)
                            return

                        foundBlobs = foundBlobs.replace(/\t/g, '&nbsp;&nbsp;&nbsp;&nbsp;').replace(/\n/g, '<br>')
                        logViewer.append(foundBlobs)
                    }
                }

                SmButton {
                    text: qsTr("Print blobs3");
                    onClicked: {
                        let foundBlobs = DataBus.found_blobs3

                        if (foundBlobs === undefined)
                            return

                        logViewer.append(foundBlobs.join('<br>'))
                    }
                }

                SmTextEdit {
                    id: programParams
                    width: 200
                    text: "6  140  16  160  6.25  4.25  5000"
                }
                SmButton {
                    text: qsTr("Generate program")
                    onClicked: {
                        codeEditor.clear()
                        let p = programParams.text.split(' ').filter(e => e).map(Number) // Выкидываю нулевые строки и преобразую в массив чисел
                        codeEditor.append(Utils.generateSteps(p[0], p[1], p[2], p[3], p[4], p[5], p[6]).join("\n"))
                    }
                }

                DoubleSpinBox {
                    decimals: 5
                    value: DataBus.pixel_size
                    onValueModified: DataBus.pixel_size = Number(text)
                }

//                SmButton {
//                    text: qsTr("Visit blob")

//                    onClicked: {
//                        var blobInfo = DataBus.blob_info3
//                        var points = blobInfo.split(" ");
//                        moveTo(points[0], points[1]);
//                    }
//                }

                SmButton {
                    id: blobVisitor
//                    property int idx: 0
//                    property var blobs: DataBus.found_blobs3

                    text: qsTr("Visit next blob")
                    checkable: true

                    onCheckedChanged: {
                        checked ? blobVisitorPromise.runAsync() : blobVisitorPromise.abort()
                    }

//                    onClicked: {
//                        var blobInfo = blobs[idx]
//                        var points = blobInfo.split(" ")
//                        moveTo(points[0], points[1])

//                        ++idx

//                        if (idx >= blobs.length)
//                            idx = 0
//                    }
                }

//                SmButton {
//                    text: qsTr("Reset blob visitor")

//                    onClicked: {
//                        blobVisitor.idx = 0
//                    }
//                }

                QMLPromises {
                    id: blobVisitorPromise

                    onRunningChanged: {
                        if (running === false)
                            blobVisitor.checked = false
                    }

                    function runAsync() {
                        asyncToGenerator( function* () {

                            let blobs = DataBus.found_blobs3

                            if (blobs === undefined) {
                                appendLog("no blobs to visit\n")
                                return
                            }

                            let updatedBlobs = []

                            statusTimer.interval = 100
                            statusTimer.start()

                            for (let blob of blobs) {
                                let point = blob.split(" ")
                                console.log (point)
                                console.log (point[0])
                                console.log (point[1])

                                moveTo(point[0], point[1])

                                yield sleep(200)
                                status = "Wait"
                                yield waitUntil({target: root, property: "status", value: "Idle"})

                                // Ждать пока позиция не станет той, что нужно

//                                let pos = "[${point[0]} ${point[1]}]"
//                                console.log(pos)
                                //yield waitUntil({target: root, property: "fullStatus", value: pos })

                                Video4.captureSmallRegion()
                                yield waitForSignal(Video4.capturedSmallRegion)

                                var smallRegion = Video4.smallRegion()


                                OpenCv.blobDetectorUpdated(smallRegion)
                                yield waitForSignal(OpenCv.smallRegionBlobChanged)

                                let coordBlob = OpenCv.smallRegionBlob()

                                point = coordBlob.split(" ")
                                moveTo(point[0], point[1])

                                yield sleep(200)
                                status = "Wait"
                                yield waitUntil({target: root, property: "status", value: "Idle"})


                                // Найти координаты блоба по центру
                                // Занести его координаты в список
                                yield sleep(1000)
                            }
                        } )();
                    }
                }


            }
        }

        SplitView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Item {
                SplitView.minimumWidth: 50
                SplitView.preferredWidth: root.width / 3
                SplitView.maximumWidth: 800

                SplitView {
                    anchors.fill: parent
                    orientation: Qt.Vertical

                    Log {
                        id: logViewer
                        SplitView.minimumHeight: 50
                        SplitView.preferredHeight: parent.height / 2
                    }
                    CodeEditor2 {
                        id: codeEditor
                        SplitView.minimumHeight: 50
                        SplitView.preferredHeight: parent.height / 2
                    }
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

                Text {
                    anchors.top: image.top
                    anchors.right: image.right
                    text: DataBus.blob_info + "\n" + DataBus.blob_info2 + "\n" + DataBus.blob_info3 + "\n" + DataBus.blob_info4
                    //text: DataBus.blob_info3
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
                                resolutionList.setCurrentFormat()
                                Video4.start()
                            }
                            else
                                Video4.stop()
                        }
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
                            Video4.capture()
                            //cycle.runAsync()
                        }
                    }

                    ComboBox {
                        id: imgType
                        width: 200
                        model: ["raw", "circle", "blob", "raw captured" ]
                        onActivated: {
                            image.setSource("image://camera/" + currentText)
                            DataBus.mode = currentText
                        }
                        Component.onCompleted: {
                            image.setSource("image://camera/raw")
                            DataBus.mode = "raw"
                        }
                    }

                    ComboBox {
                        id: captureNumber
                        width: 200
                        model: DataBus.capture_number
                        onActivated: {
                            image.setSource("image://camera/captured_" + currentText)
                        }
                    }

                    ComboBox {
                        width: 200
                        id: cameraList
                        model: DataBus.cameras
                    }

                    ComboBox {
                        id: resolutionList
                        width: 200
                        textRole: "display"
                        model: sortResolutions(DataBus["camera" + cameraList.currentIndex])
                        onActivated: {
                            setCurrentFormat()
                        }
                        onModelChanged: {
                            setCurrentFormat()
                        }

                        function setCurrentFormat() {
                            let resolution = model[currentIndex]
                            Video4.changeCamera(cameraList.currentIndex*2, resolution.width, resolution.height, resolution.fourcc)
                        }

                        function sortResolutions(resolutions) {
                            let resYuyv = resolutions.filter(e => e.fourcc === "YUYV")
                            let resMjpg = resolutions.filter(e => e.fourcc === "MJPG")

                            // Функция сортировки по возрастанию разрешения
                            let sortFunction = function (a,b) {
                                if (a.width * a.height === b.width * b.height)
                                    return 0
                                return a.width * a.height > b.width * b.height ? 1 : -1
                            }

                            // Меняю порядок сортировки - большие разрешения вперед
                            resYuyv.sort(sortFunction).reverse()
                            resMjpg.sort(sortFunction).reverse()

                            // Сначала YUYV разрешения
                            return resYuyv.concat(resMjpg)
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
        onActivated: write("?")
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
