import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import Qt.labs.platform 1.1
import Process 1.0
import ImageItem 1.0

import "utils.js" as Utils

Item {
    id: root

    property string status: "Idle"
    property string fullStatus

    property real xPos
    property real yPos
    property real zPos

    function write(msg) {
        Serial.write(msg+"\n")
        appendLog(msg+"\n")
    }

    function jog(axis, mm, feed) {
        if (feed === undefined)
            feed = 1000
        write("$J=G91 " + axis + mm + " F" + feed)
    }

    function appendLog(message, color) {
        if (color === undefined)
            color = 'red'

        let msg = message.split('').join('') // Копирую строку фактически
        msg = msg.replace(/\r?\n/g, '<br>')

        msg = String(Date.now()).slice(-4) + ": " + msg
        logViewer.append("<font color=" + color + ">" + msg + "</font>")
    }

    function moveTo(x, y) {
        if (typeof x === "string" && typeof y === "string")
            write("G1 G90 F5000 X" + x + " Y" + y)
        else if (typeof x === "number" && typeof y === "number") {
            write("G1 G90 F5000 X" + x.toFixed(3) + " Y" + y.toFixed(3))
        }
        else {
            appendLog("error move to point " + x + " " + y + " wrong arguments")
        }
    }

    function sortResolutions(resolutions) {
        if (resolutions === undefined)
            return

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

        // Сначала MJPG разрешения
        return resMjpg.concat(resYuyv)
    }

    Connections { target: Serial;            function onMessage(msg) { appendLog(msg + '<br>', 'lightgrey') } }
    Connections { target: TaskScan;          function onMessage(msg) { appendLog(msg + '<br>') } }
    Connections { target: TaskUpdate;        function onMessage(msg) { appendLog(msg + '<br>') } }
    Connections { target: TaskCheckCamera;   function onMessage(msg) { appendLog(msg + '<br>') } }
    Connections { target: TaskPunch;         function onMessage(msg) { appendLog(msg + '<br>') } }
    Connections { target: TaskFindPixelSize; function onMessage(msg) { appendLog(msg + '<br>') } }
    // Connections { target: TaskBestPath;      function onMessage(msg) { appendLog(msg + '<br>') } }

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
                    DataBus.status = status
                    let position = statusValues[2] // третий элемент - позиция
                    let pos = position.split(":")[1].split(",") // Позиция выглядит так: MPos:0.000,121.250,0.000
                    DataBus.x_coord = pos[0]
                    DataBus.y_coord = pos[1]
                    DataBus.z_coord = pos[2]
                    fullStatus = DataBus.x_coord + " " + DataBus.y_coord + " " + DataBus.z_coord

                    xPos = parseFloat(pos[0])
                    yPos = parseFloat(pos[1])
                    zPos = parseFloat(pos[2])

                    DataBus.xPos = xPos
                    DataBus.yPos = yPos
                    DataBus.zPos = zPos

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

            //Item { height: 30; width: 10}

            Text {
                text: fullStatus + status
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                height: 30
            }

            CollapsiblePanel {
                id: machineCommandsPanel
                width: parent.width
                height: checked ? 220 : 30
                text: "Machine Commands"
                checked: false
                onCheckedChanged: machineCommands.visible = checked
                Component.onCompleted: machineCommands.visible = checked

                Column {
                    id: machineCommands
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
                    }
                }
            }


            PanelMoveToPosition {}

            // Pane {
            //     id: pane
            //     width: parent.width
            //     height: loader.item === null ? 10 : loader.item.height

            //     padding: 0
            //     spacing: 0
            //     leftInset: 0
            //     rightInset: 0
            //     topInset: 0
            //     bottomInset: 0

            //     Loader {
            //         id: loader

            //         readonly property string path: "/home/mint/devel/pin_machine/app/gui/PanelMoveToPosition.qml"  // Эта строка меняется на нужную

            //         function reload() {
            //             loader.source = ""
            //             QmlEngine.clearCache()

            //             loader.source = path
            //         }

            //         source: path

            //         onLoaded : {
            //             parent.height = loader.item.height

            //         }

            //         Connections { target: FileSystemWatcher; function onFileChanged (path) {
            //             if (path === loader.path) {
            //                 loader.reload()
            //             }

            //         }
            //         }

            //         // Костыль. Проблемы какие-то постоянно с filesystemwatcher
            //         Timer { interval: 100; running: true; repeat: true; onTriggered: {  FileSystemWatcher.addPath(loader.path) }
            //         }
            //     }
            // }


            Grid {
                width: parent.width
                //height: 100
                columns: 3

                SmTextEdit {
                    id: programParams
                    width: 200
                    text: "85 120  165  170  10  8  5000"
                }
                SmButton {
                    text: qsTr("Generate program")
                    onClicked: {
                        codeEditor.clear()
                        let p = programParams.text.split(' ').filter(e => e).map(Number) // Выкидываю нулевые строки и преобразую в массив чисел
                        codeEditor.append(Utils.generateSteps(p[0], p[1], p[2], p[3], p[4], p[5], p[6]).join("\n"))
                    }
                }
                Item { height: 20; width: 10}

                SmButton {
                    id: scan
                    text: qsTr("Fast scan")
                    checkable: true
                    onCheckedChanged: checked ?  TaskScan.run(codeEditor.text, selectedResolution().width, selectedResolution().height, selectedResolution().fourcc) : TaskScan.stopProgram()
                    Connections { target: TaskScan; function onFinished() { scan.checked = false } }
                    function selectedResolution() {
                        return sortResolutions(DataBus["camera_image_formats_" + cameraList.currentValue])[resolutionListForScan.currentIndex]
                    }
                }
                ComboBox {
                    id: resolutionListForScan
                    width: 200
                    textRole: "display"
                    model: sortResolutions(DataBus["camera_image_formats_" + cameraList.currentValue]) // Плохо, по другому выбирать откуда брать разрешения
                    onModelChanged: {
                        let res = sortResolutions(DataBus["camera_image_formats_" + cameraList.currentValue])

                        for (let i = 0; i < res.length; i++) {
                            let r = res[i]
                            if (r.width === 800 && r.height === 600 &&  r.fourcc === "YUYV")
                                currentIndex = i
                        }
                    }
                }
                Item { height: 30; width: 10}

                SmButton {
                    id: update
                    text: qsTr("Update selected")
                    checkable: true
                    onCheckedChanged: checked ? TaskUpdate.run(selectedResolution().width, selectedResolution().height, selectedResolution().fourcc) : TaskUpdate.stopProgram()
                    Connections { target: TaskUpdate; function onFinished() { update.checked = false } }
                    function selectedResolution() {
                        return sortResolutions(DataBus["camera_image_formats_" + cameraList.currentValue])[resolutionListForUpdate.currentIndex]
                    }
                }
                ComboBox {
                    id: resolutionListForUpdate
                    width: 200
                    textRole: "display"
                    model: sortResolutions(DataBus["camera_image_formats_" + cameraList.currentValue]) // Плохо, по другому выбирать откуда брать разрешения
                    onModelChanged: {
                        let res = sortResolutions(DataBus["camera_image_formats_" + cameraList.currentValue])

                        for (let i = 0; i < res.length; i++) {
                            let r = res[i]
                            if (r.width === 1280 && r.height === 960 && r.fourcc === "YUYV")
                                currentIndex = i
                        }
                    }
                }
                Item { height: 30; width: 10}

                SmButton {
                    id: save
                    text: qsTr("Save")
                    onClicked: saveDialog.open()

                    FileDialog {
                        id: saveDialog
                        folder: applicationDirPath
                        fileMode: FileDialog.SaveFile
                        onAccepted: Engine.save(currentFile)
                        modality: Qt.ApplicationModal
                    }
                }
                SmButton {
                    id: load
                    text: qsTr("Load")
                    //onClicked: loadDialog.open()

                    onClicked: Engine.load(Qt.resolvedUrl("/home/mint/Desktop/123"))
                    // FileDialog {
                    //     id: loadDialog
                    //     folder: applicationDirPath
                    //     fileMode: FileDialog.OpenFile
                    //     onAccepted: Engine.load(currentFile)
                    //     modality: Qt.ApplicationModal
                    // }
                }

                Item { height: 30; width: 10}

                SmButton {
                    id: checkCamera
                    text: qsTr("Check camera")
                    checkable: true
                    onCheckedChanged: checked ? TaskCheckCamera.run() : TaskCheckCamera.stopProgram()
                    Connections { target: TaskCheckCamera; function onFinished() { checkCamera.checked = false } }
                }
            }

            PanelJogControl {}
            PanelPunchCode {}
            PanelStartPoint {}
            PanelToolShift {}
            PanelDebug {}
            PanelCameraSettings {}


        }

        SplitView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Item {
                SplitView.minimumWidth: 50
                SplitView.preferredWidth: expandVideo.checked ? 50 : root.width / 3
                SplitView.maximumWidth: 800

                SplitView {
                    anchors.fill: parent
                    orientation: Qt.Vertical

                    Item {
                        SplitView.minimumHeight: 50
                        SplitView.preferredHeight: parent.height / 2
                        Log {
                            id: logViewer
                            anchors.fill:parent
                        }
                        Button {
                            anchors.top: parent.top
                            anchors.left: parent.left
                            width: 25
                            height:25
                            text: ("🗑")
                            onClicked: { logViewer.clear() }

                            ToolTip.visible: hovered
                            ToolTip.text: qsTr("Clear log")
                        }
                    }
                    CodeEditor2 {
                        id: codeEditor
                        SplitView.minimumHeight: 50
                        SplitView.preferredHeight: parent.height / 2
                    }
                }
            }

            Item {
                ImageItem {
                    id: image
                    anchors.fill: parent
                    image: DataBus["image_" + DataBus.mode]
                }

                Item {
                    anchors.fill: parent

                    Button {
                        id: expandVideo
                        width: 25
                        height: 25
                        checkable: true
                        anchors.top: parent.top
                        anchors.right: parent.right
                        text: checked ? "→" : "←";
                    }

                    Text {
                        anchors.top: parent.top
                        anchors.right: parent.right
                        text: DataBus.blob_info
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
                                Video4.reloadDevices()
                            }
                        }

                        // Button {
                        //     id: captureFrame
                        //     width: 200
                        //     text: qsTr("Capture frame")
                        //     onPressed: {
                        //         Engine.capture()
                        //     }
                        // }

                        ComboBox {
                            id: imgType
                            width: 200
                            model: ["raw", "circle", "blob", "raw_captured", "small_blob_captured", "adapt_threshold_1", "adapt_threshold_2", "corr"]
                            onActivated: DataBus.mode = currentText
                            Component.onCompleted: DataBus.mode = "raw"
                            MouseArea {
                                anchors.fill: parent
                                onWheel: {
                                    if (wheel.angleDelta.y > 0) {
                                        imgType.decrementCurrentIndex()
                                    }
                                    else if (wheel.angleDelta.y < 0) {
                                        imgType.incrementCurrentIndex()
                                    }
                                }
                                onPressed: {
                                    // propogate to ComboBox
                                    mouse.accepted = false;
                                }
                                onReleased: {
                                    // propogate to ComboBox
                                    mouse.accepted = false;
                                }
                            }
                        }

                        ComboBox {
                            width: 200
                            id: cameraList
                            valueRole: "id"
                            textRole: "name"
                            model: DataBus.cameras
                        }

                        ComboBox {
                            id: resolutionList
                            width: 200
                            textRole: "display"
                            model: sortResolutions(DataBus["camera_image_formats_" + cameraList.currentValue])
                            onActivated: {
                                setCurrentFormat()
                            }
                            onModelChanged: {
                                setCurrentFormat()
                            }

                            function setCurrentFormat() {
                                if (model === undefined)
                                    return
                                let resolution = model[currentIndex]
                                Video4.changeCamera(cameraList.currentValue, resolution.width, resolution.height, resolution.fourcc)

                                //                            if (resolution.width === 800)
                                //                                DataBus.pixel_size = 0.017
                                //                            else if (resolution.width === 1280)
                                //                                DataBus.pixel_size = 0.0107
                                //                            else
                                //                                DataBus.pixel_size = 0.00524 * Math.floor(2592 / resolution.width)

                                DataBus.resolution_width = resolution.width
                                DataBus.resolution_height = resolution.height
                            }

                            function sortResolutions(resolutions) {
                                if (resolutions === undefined)
                                    return

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

                                // Сначала MJPG разрешения
                                return resMjpg.concat(resYuyv)
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
                        visible: imgType.currentText === "blob" || imgType.currentText === "adapt_threshold_1" || imgType.currentText === "adapt_threshold_2"
                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                        anchors.right: parent.right
                    }
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

    Component.onCompleted: {
        Video4.reloadDevices()
    }
}
