import QtQml 2.15
import QtQuick 2.15

import Process 1.0

QMLPromises {
    id: openSerialPromise

    Process {
        id: fup
        property string device: ""

        onReadyRead: {
            const result = readAll()
            fup.device= ("/dev/" + result).replace(/\s/g, "")
        }
        onFinished: {
            //findUsbTimer.start()
        }
    }

    function runAsync() {
        asyncToGenerator( function* () {
            console.log("started!")

            while (true) {
                Logger.log = "while true"

                checkSerial: while (true) {
                    console.log ("check serial started...")

                    if (Serial.isOpen) {
                        Logger.log = "Serial is open"
                        yield sleep(1000)
                        continue checkSerial
                    }
                    Logger.log = "Serial is not open"
                    break
                }

                findUsbProcess: while (true) {
                    Logger.log = "find usb process started..."

                    fup.start("/bin/sh", ["-c", "ls /dev | grep ttyUSB"]);

                    yield sleep(1000)

                    if (fup.device === "") {
                        Logger.log = "devicse not found!"
                        yield sleep(1000)

                        continue findUsbProcess
                    }

                    Logger.log = "devicse name " + fup.device
                    Logger.log = "Serial opening ..."

                    Serial.setPortName(fup.device)
                    Serial.setBaudRate("115200")
                    Serial.setDataBits("8")
                    Serial.setParity("N")
                    Serial.setStopBits("1")
                    Serial.open()

                    yield sleep(1000)

                    break
                }
            }

            Logger.log = "finished!"
        } )();
    }
}
