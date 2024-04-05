import QtQml 2.15
import QtQuick 2.15

import Process 1.0

Item {
    id: root

    signal usbFound (string device)

    property string device: ""

    function stop() {
        findUsbTimer.stop()
    }

    function start() {
        findUsbTimer.start()
    }

    Process {
        id: findUsbProcess
        onReadyRead: {
            const result = readAll()
            let device = ("/dev/" + result).replace(/\s/g, "")
            root.device = device
            root.usbFound(device)
        }
        onFinished: {
            //findUsbTimer.start()
        }
    }
    Timer {
        id: findUsbTimer
        interval: 1000
        repeat: true
        triggeredOnStart: false
        running: false
        onTriggered: {
            root.usbFound("")
            root.device = ""
            findUsbProcess.start("/bin/sh", ["-c", "ls /dev | grep ttyUSB"]);
        }
    }
}
