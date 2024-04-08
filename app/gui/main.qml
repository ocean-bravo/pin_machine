import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: root

    property bool guiDebug: true

    property var appWin: operatorUser

    property string status: "Idle"
    property string fullStatus

    property real xPos
    property real yPos
    property real zPos

    width: MainWindow.width
    height: MainWindow.height

    OperatorUser {
        id: operatorUser

        Layout.fillHeight: true
        Layout.fillWidth: true
        //    anchors.fill: parent
        // objectName: "operatoruser"
        visible: true
    }

    SuperUser {
        id: superUser
        objectName: "superuser"
        Layout.fillHeight: true
        Layout.fillWidth: true
        visible: false
    }

    OpenSerialPromise { id: openSerialPromise }

    Component.onCompleted: {
        console.log("root item completed")
        operatorUser.visible = true
        //loader.sourceComponent = operatorComponent

        DataBus.live_preview_mode = "raw"

        openSerialPromise.runAsync()
    }

    // Timer {
    //     interval: 5000

    //     repeat: false

    //     running: true

    //     onTriggered: {
    //         loader.sourceComponent = operatorComponent
    //     }
    // }


    // Loader {
    //     id: loader
    //     visible: true
    //     sourceComponent: superComponent
    //     onLoaded: {
    //         appWin = item
    //     }
    // }

    // TestUser {
    //     id: testUser
    //     visible: false
    // }

    Shortcut {
        sequence: "F1"
        context: Qt.ApplicationShortcut
        onActivated: {
            operatorUser.visible = true
            appWin = operatorUser
            superUser.visible = false

            //loader.sourceComponent = operatorComponent
            //appWin = operatorUser
        }
    }

    Shortcut {
        sequence: "F2"
        context: Qt.ApplicationShortcut
        onActivated: {
            superUser.visible = true
            appWin = superUser
            operatorUser.visible = false
            //loader.sourceComponent = superComponent
            //appWin = loader.item
        }
    }

    // Shortcut {
    //     sequence: "F3"
    //     context: Qt.ApplicationShortcut
    //     onActivated: {
    //         superUser.visible = false
    //         operatorUser.visible = false
    //         appWin = testUser
    //         testUser.visible = true
    //     }
    // }



    Shortcut {
        sequence: "F5"
        context: Qt.ApplicationShortcut
        onActivated: {
            // console.log("reload")
            // loader.reload()

            DataBus.messagebox = "dsafasdf"
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

    MessageBoxLoader {
        id: splash
        text: DataBus.splash
        backgroundColor: "green"
        noButtons: true
        onTextChanged: {
            if (text.length > 0)
                show()
            else
                hide()
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

    Shortcut {
        sequence: "F9"
        context: Qt.WindowShortcut
        onActivated: {
            QmlEngine.clearCache()
            appWin.close()
            Engine.reload()
        }
    }

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
