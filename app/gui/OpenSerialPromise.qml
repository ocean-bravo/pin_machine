import QtQml 2.15
import QtQuick 2.15

import Process 1.0

QMLPromises {
    id: openSerialPromise

    Process {
        id: process

        property string device: ""

        function findUsb() {
            start("/bin/sh", ["-c", "ls /dev | grep ttyUSB"]);
        }

        onReadyRead: {
            const result = readAll()
            process.device = ("/dev/" + result).replace(/\s/g, "")
        }
    }

    function runAsync() {
        asyncToGenerator( function* () {
            while (true) {
                while (true) {
                    console.log("check serial started...")

                    if (Serial.isOpen) {
                        console.log("Serial is open")
                        yield sleep(1000)
                        continue
                    }
                    console.log("Serial is not open")
                    break
                }

                while (true) {
                    console.log("find usb process started...")

                    process.findUsb()

                    yield sleep(100)

                    if (process.device === "") {
                        console.log("devicse not found!")
                        yield sleep(1000)
                        continue
                    }

                    console.log("devicse name ", process.device)
                    console.log("Serial opening ...")

                    Serial.setPortName(process.device)
                    Serial.setBaudRate("115200")
                    Serial.setDataBits("8")
                    Serial.setParity("N")
                    Serial.setStopBits("1")
                    Serial.open()

                    yield sleep(1000)

                    break
                }
            }
        } )();
    }
}
