import QtQml 2.12
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12

ApplicationWindow {
    id: appWin
    visible: true
    width: 800
    height: 480
    flags: Qt.Window

    Commands {
        id: commands
        anchors.fill: parent
    }
    MyCamera {
        id: camera
        anchors.fill: parent
    }
    OpenCV {
        id: opencv
        anchors.fill: parent
    }

    Shortcut {
        sequence: "F1"
        context: Qt.ApplicationShortcut
        onActivated: {
            commands.visible = true
            camera.visible = false
            opencv.visible = false
        }
    }
    Shortcut {
        sequence: "F2"
        context: Qt.ApplicationShortcut
        onActivated: {
            commands.visible = false
            camera.visible = true
            opencv.visible = false
        }
    }

    Shortcut {
        sequence: "F3"
        context: Qt.ApplicationShortcut
        onActivated: {
            commands.visible = false
            camera.visible = false
            opencv.visible = true
        }
    }
}
