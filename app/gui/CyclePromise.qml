import QtQml 2.15
import QtQuick 2.15


QMLPromises {
    id: cycle

    property var codeLines: []
    property int lineToSend: 0

    property real xTarget
    property real yTarget

    function sendNextLine() {
        let line = codeLines[lineToSend]
        let lineNumber = lineToSend+1

        // Пропускаю пустые строки
        if (line.length === 0) {
            var msg = "" + lineNumber + ": " + "skip..." + "\n"
        }
        else {
            xTarget = extractFromGcodeX(line)
            yTarget = extractFromGcodeY(line)

            //                console.log( " x target " + xTarget)
            //                console.log( " y target " + yTarget)

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

            const waitForGetPosition = function* () {
                yield waitForCondition(() => root.status === "Idle" &&
                                             Math.abs(xTarget - xPos) <= 0.003 &&
                                             Math.abs(yTarget - yPos) <= 0.003)
            }

            while (true) {
                if (sendNextLine()) { // Если строка пустая, никаких действий после нее не надо делать

                    yield* waitForGetPosition()

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
