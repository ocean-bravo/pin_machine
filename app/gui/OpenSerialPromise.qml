import QtQml 2.15
import QtQuick 2.15

import Process 1.0

QMLPromises {
    id: root

    property string device: ""

    Process {
        id: process

        function findUsb() {
            start("/bin/sh", ["-c", "ls /dev | grep ttyUSB"])
        }

        onReadyRead: {
            const result = readAll()
            device = ("/dev/" + result).replace(/\s/g, "")
        }
    }

    function runAsync() {
        asyncToGenerator( function* () {
            while (true) {
                while (true) {

                    if (Serial.isOpen) {
                        //console.log("Serial is open")
                        yield sleep(1000)
                        continue
                    }
                    console.log("Serial is not open")
                    break
                }

                while (true) {
                    //console.log("find usb process started...")

                    process.findUsb()

                    yield sleep(100)

                    if (device === "") {
                        console.log("USB device not found!")
                        yield sleep(1000)
                        continue
                    }

                    console.log("USB device name: ", device)
                    console.log("Serial opening ...")

                    Serial.setPortName(device)
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
