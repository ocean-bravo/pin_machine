import QtQml 2.12
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import Process 1.0


Item {
    id: root

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
        1: "G-code words consist of a letter and a value. Letter was not found.",
        2: "Numeric value format is not valid or missing an expected value.",
        3: "",
        4: "",
        5: "",
        6: "",
        7: "",
        8: "Grbl '$' command cannot be used unless Grbl is IDLE. Ensures smooth operation during a job.",
        9: "G-code locked out during alarm or jog state",
        10: "",
        11: "",
        12: "",
        13: "",
        14: "",
        15: "",
        16: "",
        17: "",
        18: ""
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

    function write(msg) {
        Serial.write(msg)
        msg = msg.replace(new RegExp('\r?\n','g'), '<br />')
        log.append("<font color='red'>" + msg + "</font>")
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
                    }
                    Timer {
                        interval: 1000
                        repeat: true
                        triggeredOnStart: true
                        running: true
                        onTriggered: {
                            serialPortName.text = ""
                            findUsbProcess.start("/bin/sh", ["-c", "ls /dev | grep ttyUSB"]);
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

            Grid {
                width: parent.width
                columns: 3
                columnSpacing: 5
                rowSpacing: 5

                SmButton { text: qsTr("Unlock($X)"); onClicked: { write("$X\n" )     } }
                SmButton { text: qsTr("Info($I)");   onClicked: { write("$I\n" )     } }
                SmButton { text: qsTr("Feed Hold(!)");   onClicked: { write("!\n" )     } }
                SmButton { text: qsTr("Start/Resume(~)");   onClicked: { write("~\n" )     } }
                SmButton { text: qsTr("Reset($RST=#)");   onClicked: { write("$RST=#\n" )     } }

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
                            write("?\n")
                        }
                    }

                }
                SmButton { text: qsTr("Params($$)"); onClicked: { write("$$\n" )      } }
                SmButton { text: qsTr("Soft Reset(ctrl+x)"); onClicked: { write("\x18\n" )       } }


                Item { height: 20; width: 10}
                Item { height: 20; width: 10}
                Item { height: 20; width: 10}



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
                SmButton { text: qsTr("Clear log");  onClicked: { log.clear() } }

            }

            JogControl {
                width: parent.width
                height: width

            }
        }

        Log {
            id: log
            Layout.preferredWidth: parent.width / 3
            Layout.fillHeight: true
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Process {
                id: cameraCapture

                function startCamera() {
                    cameraCapture.start("/bin/sh", ["-c", cameraCapture.script]);
                }

                property string script:
                    "ffmpeg -y -f v4l2 \
                           -framerate 30 \
                           -video_size 640x480 \
                           -input_format yuyv422 \
                           -i /dev/video2 \
                           -vf fps=2 \
                           -update 1 \
                           /dev/shm/cap.png"

                onStandardErrorChanged: {
                    console.log("error: ", standardError)
                }
                onStandardOutputChanged: {
                    console.log("output: ", standardOutput)
                }
            }

            Process {
                id: capChanged
                property bool soft: false
                onReadyRead: {
                    if (soft)
                        image.source = "/dev/shm/cap_soft.png"
                    else
                        image.source = "/dev/shm/cap.png"

                    soft = !soft
                }

                function startWatch() {
                    start("/bin/sh", ["-c", "inotifywait --monitor --event close_write /dev/shm/cap.png"]);
                }
            }


            Timer {
                interval: 500
                repeat: true
                triggeredOnStart: true
                running: true
                property bool soft: false
                onTriggered: {
                    if (soft)
                        image.setSource("/dev/shm/cap_soft.png")
                    else
                        image.setSource("/dev/shm/cap.png")

                    soft = !soft
                }
            }

//            Image {
//                id: image
//                x: 0
//                y: 0
//                fillMode:Image.PreserveAspectFit
//                anchors.fill: parent
//            }

            ImageDoubleBuff {
                id: image
                anchors.fill: parent
            }

            Button {
                x: 0
                y: 0
                text: "start"

                onClicked: {
                    image.setSource("/dev/shm/cap.png")
                    cameraCapture.startCamera()
                    capChanged.startWatch()
                }
            }


        }



//        ColumnLayout {

//            TabBar {
//                id: bar
//                Layout.fillWidth: true
//                height: 20

//                TabButton {
//                    text: "Console"
//                    width: 100
//                }
//                TabButton {
//                    text: "Camera"
//                    width: 100
//                }
//            }

//            StackLayout {
//                //            anchors.left: parent.left
//                //            anchors.right: parent.right
//                //            anchors.top: barRow.bottom
//                //            anchors.bottom: parent.bottom

//                currentIndex: bar.currentIndex


//            }
//        }

    }



    Connections {
        target: Serial
        function onMessage(msg) {
            log.append("<font color='darkgrey'>" + msg + "</font><br>")
        }
    }

    Connections {
        target: Serial
        function onData(msg) {
            // Симвлы < и > есть во входящих данных. Они интерпретируются как Html. Надо заменить на другие.
            msg = msg.replace(new RegExp('<','g'), '[')
            msg = msg.replace(new RegExp('>','g'), ']')
            msg = msg.replace(new RegExp('\r?\n','g'), '<br />')

            for (let i = 1; i < 11; ++i) {
                let alrm = "ALARM:" + i
                if (msg.includes(alrm))
                    msg = msg.replace(new RegExp(alrm,'g'), alrm +  ' [' + alarms[i] + ']')
            }
            for (let j = 1; j < 100; ++j) {
                let err = "error:" + j
                if (msg.includes(err))
                    msg = msg.replace(new RegExp(err,'g'), err +  ' [' + errors[j] + ']')
            }
            log.append("<font color='darkblue'>" + msg + "</font>")
        }
    }

    Shortcut {
        sequence: "F5"
        context: Qt.ApplicationShortcut
        onActivated: write("?\n")
    }
}

