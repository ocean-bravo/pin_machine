import QtQml 2.15
import QtQuick 2.15

Item {
    id: root

    Connections {
        id: camerasInfo
        target: DataBus
        function onValueChanged (key, value) {
            if (key !== "cameras")
                return

            let ourCamera = Settings.value("camera_name") // Имя нужной нам камеры из настроек

            for (let cameraInfo of value) {
                if (cameraInfo.name.includes(ourCamera)) {
                    findCameraTimer.stop()
                    camerasInfo.enabled = false
                }
            }
        }
    }

    Timer {
        id: findCameraTimer
        interval: 1000
        repeat: true
        triggeredOnStart: true
        running: true
        onTriggered: {
            Video4.reloadDevices()
        }
    }
}
