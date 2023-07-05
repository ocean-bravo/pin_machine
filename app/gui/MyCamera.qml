import QtQml 2.12
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtMultimedia 5.12


Item {
    id: root

    property var supportedResolutions

    Camera {
        id: camera
        captureMode: Camera.CaptureViewfinder //CaptureVideo //CaptureViewfinder

        onCaptureModeChanged: {
            console.log(captureMode)
        }

        onCameraStateChanged: {
            if (cameraState === 0) var stateString = "UnloadedState"
            if (cameraState === 1) stateString = "LoadedState"
            if (cameraState === 2) stateString = "ActiveState"

            console.log("\t" + stateString + " " + cameraState)

            if (cameraState === Camera.LoadedState /* || cameraState === Camera.ActiveState*/) {
                supportedResolutions = camera.supportedViewfinderResolutions()
            }
        }

        onCameraStatusChanged: {
            if (cameraStatus === 0) var statusString = "ActiveStatus"
            if (cameraStatus === 1) statusString = "StartingStatus"
            if (cameraStatus === 2) statusString = "StoppingStatus"
            if (cameraStatus === 3) statusString = "StandbyStatus"
            if (cameraStatus === 4) statusString = "LoadedStatus"
            if (cameraStatus === 5) statusString = "LoadingStatus"
            if (cameraStatus === 6) statusString = "UnloadingStatus"
            if (cameraStatus === 7) statusString = "UnloadedStatus"
            if (cameraStatus === 8) statusString = "UnavailableStatus"

            console.log(statusString + " " + cameraStatus)
        }
    }

    VideoOutput {
        visible: true

        x: 0
        y: 0
        width: 300 //parent.width
        height: 300//parent.height

        source: camera
        autoOrientation: true
    }

    Text {
        x: 120
        y:0
        height: 15
        width: 300
        text: camera.displayName
        color: "white"
    }

    Text {
        x: 120
        y: 15
        height: 15
        width: 300
        text: camera.deviceId
        color: "white"
    }

    Column {
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        spacing: 5

        width: 120

        Text {
            id: currentResolution
            height: 30
            width: 120
            text: camera.viewfinder.resolution.width + " " + camera.viewfinder.resolution.height
            color: "white"
        }

        Text {
            id: frameRate
            height: 30
            width: 120
            text: "frame rate: " + camera.viewfinder.minimumFrameRate.toFixed(2) + "-" + camera.viewfinder.maximumFrameRate.toFixed(2)
            color: "white"
        }

        Item {
            height: {
                if (typeof cameraList.model === "undefined")
                    return 30

                return cameraList.model.length * 30
            }
            width: parent.width

            ListView {
                id: cameraList
                anchors.fill: parent

                currentIndex: 0

                model: QtMultimedia.availableCameras

                delegate: Button {
                    width: 120
                    height: 30
                    text: modelData.displayName

                    onClicked: {
                        camera.viewfinder.resolution = Qt.size(0,0) // для Linux это надо

                        console.log("camera before stop")
                        camera.stop()
                        console.log("camera after stop")

                        console.log("camera set befor ", modelData.deviceId)
                        camera.deviceId = modelData.deviceId
                        console.log("camera set after ", modelData.deviceId)

                        console.log("camera before start")
                        camera.start()
                        console.log("camera after start")
                    }
                }
            }
        }

        Button {
            width: 120
            height: 30
            text: "Reload"
            onClicked: {
                var cameras = QtMultimedia.availableCameras
                cameraList.model = cameras

                for (var i = 0; i < cameras.length; ++i) {
                    console.log(cameras[i].displayName + "\t" + cameras[i].deviceId)
                }
                console.log("\n")
            }
        }
    }

    Column {
        x: 0
        y: 50
        spacing: 10

        Button {
            width: 100
            height: 30
            text: qsTr("stop")
            onPressed: {
                camera.stop()
            }
        }

        Button {
            width: 100
            height: 30
            text: qsTr("start")
            onPressed: {
                camera.start()
            }
        }

        Item {
            height: {
                if (typeof resList.model === 'undefined')
                    return 30

                return resList.model.length * 30
            }
            width: parent.width

            ListView {
                id: resList
                anchors.fill: parent

                currentIndex: 0

                model: supportedResolutions

                delegate: Button {
                    width: 100
                    height: 30
                    text: modelData.width + " " + modelData.height

                    onClicked: {
                        camera.stop()
                        camera.viewfinder.resolution = Qt.size(modelData.width, modelData.height)
                        camera.start()
                    }
                }
            }
        }

        Button {
            width: 100
            height: 30
            text: "Reload"
            onClicked: {
                supportedResolutions = camera.supportedViewfinderResolutions()

                for (var i = 0; i < supportedResolutions.length; ++i) {
                    console.log(supportedResolutions[i].width, ",", supportedResolutions[i].height)
                }
                console.log("\n")
            }
        }
    }
}
