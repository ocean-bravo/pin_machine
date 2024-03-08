import QtQml 2.12
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12

ApplicationWindow {
    id: appWin
    visible: true
    width: Screen.width/2
    height: Screen.height
    x: 0
    y: 0
    flags: Qt.Window

    Commands {
        id: commands
        anchors.fill: parent
    }

    // Loader {
    //     id: loader

    //     function reload() {
    //         loader.source = ""
    //         QmlEngine.clearCache()

    //         loader.source = "Commands.qml"

    //     }

    //     source: "Commands.qml"

    //     Timer { interval: 5000; repeat: true; triggeredOnStart: true; running: true; onTriggered: loader.reload(); }
    // }

    Shortcut {
        sequence: "F5"
        context: Qt.ApplicationShortcut
        onActivated: {
            console.log("reload")
            loader.reload()


        }
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
            const record = DataBus.punchpath_auto_record
            const solved = DataBus.punchpath_auto_solved

            if (solved) {
                return qsTr("Search finished.") + "\n" + qsTr("Found path with length ") + record.toFixed(2) + " mm"
            }

            if (isNaN(record)) // Этим значением перезаряжаю параметр. Без перезарядки не сработает
                return ""

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
            DataBus.punchpath_auto_stop = true
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

    Shortcut {
        sequence: "F9"
        context: Qt.WindowShortcut
        onActivated: {
            QmlEngine.clearCache()
            appWin.close()
            Engine.reload()
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

    // Выполняет заданную функцию через интервал времени
    function execAfterDelay(func, interval, ...params) {
        return setTimeoutComponent.createObject(null, { func, interval, params} );
    }

    // Однократный таймер с самоудалением
    Component {
        id: setTimeoutComponent
        Timer {
            property var func
            property var params
            running: true
            repeat: false
            onTriggered: {
                func(...params);
                destroy();
            }
        }
    }
}
