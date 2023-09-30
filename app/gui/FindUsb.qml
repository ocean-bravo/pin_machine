import QtQml 2.15
import QtQuick 2.15

import Process 1.0

Item {
    id: root

    signal usbFound (string device)

    Process {
        id: findUsbProcess
        onReadyRead: {
            const result = readAll()
            root.usbFound("/dev/" + result).replace(/\s/g, "")
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
            root.usbFound("")
        }
    }
}
