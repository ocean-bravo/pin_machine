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
        onClosed: DataBus.messagebox = "" // перезарядка, чтобы одно и тоже сообщение могло показываться
        Component.onCompleted: DataBus.messagebox = ""  // Для убирания warninga "Unable to assign [undefined] to QString"
    }

    MessageBox {
        id: splash
        text: DataBus.splash
        backgroundColor: "green"
        noButtons: true
        onTextChanged: {
            if (text.length > 0)
                open()
            else
                close()
        }
        Component.onCompleted: DataBus.splash = ""  // Для убирания warninga "Unable to assign [undefined] to QString"
    }

    MessageBoxLoader {
        id: bestPath
        text: {
            const record = DataBus.best_path_record
            const solved = DataBus.punchpath_auto_solved

            if (isNaN(record)) // Этим значением перезаряжаю параметр. Без перезарядки не сработает
                return ""

            if (solved) {
                DataBus.punchpath_auto_solved = false // перезарядка
                return qsTr("Search finished.") + "\n" + qsTr("Found path with length ") + record.toFixed(2) + " mm"
            }

            if (record === 0.0) {
                show()
                return qsTr("Searching best path...")
            }
            else {
                return qsTr("Found path with length ") + record.toFixed(2) + " mm" + "\n" + qsTr("Searching best path in progress...")
            }



        }
        onAccept: {
            hide()
            DataBus.best_path_stop = true
        }
        backgroundColor: "green"
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
