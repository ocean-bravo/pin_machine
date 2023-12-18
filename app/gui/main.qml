import QtQml 2.12
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12

ApplicationWindow {
    id: appWin
    visible: true
    width: 1920
    height: 800
    flags: Qt.Window

    Commands {
        id: commands
        anchors.fill: parent
    }

    FlashMessageBox {
        id: message
        text: DataBus.messagebox
        backgroundColor: "maroon"
        interval: 2000
        onTextChanged: {
            if (text.length > 0)
                open()
        }
        onClosed: DataBus.messagebox = "" // чтобы одно и тоже сообщение могло показываться
    }

    Button {
        id: stepNext
        text: "Next"
        width: 50
        height: 30
        visible: DataBus.next === "wait"
        onClicked: DataBus.next = "ok"
        anchors.left: parent.left
        anchors.bottom: parent.bottom
    }

//    MyCamera {
//        id: camera
//        anchors.fill: parent
//    }
//    OpenCV {
//        id: opencv
//        anchors.fill: parent
//    }

//    Shortcut {
//        sequence: "F1"
//        context: Qt.ApplicationShortcut
//        onActivated: {
//            commands.visible = true
//            camera.visible = false
//            opencv.visible = false
//        }
//    }
    Shortcut {
        sequence: "F2"
        context: Qt.ApplicationShortcut
        onActivated: {
            DataBus.next = "ok"
        }
    }

//    Shortcut {
//        sequence: "F3"
//        context: Qt.ApplicationShortcut
//        onActivated: {
//            commands.visible = false
//            camera.visible = false
//            opencv.visible = true
//        }
//    }
}
