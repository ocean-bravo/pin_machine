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
    }

    SuperUser {
        id: superUser


        Layout.fillHeight: true
        Layout.fillWidth: true
    }



    function coloredMessage(message, color) {
        if (color === undefined)
            color = 'red'

        let msg = message.split('').join('') // Копирую строку фактически
        msg = msg.replace(/\r?\n/g, '<br>')
        msg = String(Date.now()).slice(-4) + ": " + msg
        msg = "<font color=" + color + ">" + msg + "</font>"
        return msg
    }

    Connections {
        target: Serial

        function onData(msg) {
            msg = parseSerialMessage(msg)
            // superUser.appendToLog(msg, 'darkblue')
            // operatorUser.appendLog(msg, 'darkblue')
        }
    }

    property bool messageFinished: false

    property string prevMsg: ""



    // В сообщении строки иногда заканчиваютс \r\n иногда \n. Системы не понял. После ок идет \r\n всегда.
    function parseSerialMessage(msg) {

        if (0) {
            let m1 = String(msg)
            m1 = m1.replace(/</g, '|')
            m1 = m1.replace(/>/g, '|')
            //m1 = m1.replace(/\r?\n/g, '<br>')
            m1 = m1.replace(/\r/g, 'RRRRRR<br>')
            m1 = m1.replace(/\n/g, 'NNNNNN<br>')
            console.log("m1: ", m1)
        }

        msg = String(prevMsg + msg)

        // Ищу текст "ok" окруженный переводами строк
        let found = msg.match(/\r?\nok\r?\n/g)
        if (!found) {
            prevMsg = String(msg)
            return
        }

        prevMsg = ""

        if (0) {
            let m2 = String(msg)
            m2 = m2.replace(/</g, '|')
            m2 = m2.replace(/>/g, '|')
            m2 = m2.replace(/\r?\n/g, '<br>')
            console.log("m2: ", m2)
        }

        var lines = msg.split(/\r?\n/)

        // Симвлы < и > есть во входящих данных. Они интерпретируются как Html. Надо заменить на другие.
        // line = line.replace(/</g, '|')
        // line = line.replace(/>/g, '|')

        // Выбираю строки с конца, пока не встретится ok. ok тоже не нужен
        // pop() модифицирует массив
        while (lines.pop() !== "ok");

        msg = lines.join('\n')

        if (msg.match(/^[<].+[>]$/)) {
            parseStatus(msg)
        }

        if (msg.match(/Input Matrix/g)) {
            parseGpio(msg)
        }



            //                for (let k = 0; k < modes.length; ++k) {
            //                    let stat = modes[k]
            //                    if (msg.includes(stat)) {
            //                        status = stat
            //                        fullStatus = msg
            //                        //continue nextMessage
            //                    }
            //                }

            //            for (let i = 1; i < 11; ++i) {
            //                let alrm = "ALARM:" + i
            //                if (msg.includes(alrm))
            //                    msg = msg.replace(new RegExp(alrm,'g'), alrm +  ' [' + alarms[i] + ']')
            //            }
            //                for (let j = 1; j < 100; ++j) {
            //                    let err = "error:" + j
            //                    if (msg.includes(err))
            //                        msg = msg.replace(new RegExp(err,'g'), err +  ' [' + errors[j] + ']')
            //                }


            //msg = currentTime + ": " + msg
        return msg

    }

    function parseStatus(msg) {
        msg = msg.replace('/</g', '')
        msg = msg.replace('/>/g', '')
        let statusValues = msg.split("|")

        console.log(" satu s ", statusValues)
        return

        status = statusValues[1] // первый элемент будет пустой. Второй как раз статус
        let position = statusValues[2] // третий элемент - позиция
        let pos = position.split(":")[1].split(",") // Позиция выглядит так: MPos:0.000,121.250,0.000
        DataBus.x_coord = pos[0]
        DataBus.y_coord = pos[1]
        DataBus.z_coord = pos[2]
        DataBus.status = status // Позиция этого присваивания влияет на другой код, хреново
        fullStatus = DataBus.x_coord + " " + DataBus.y_coord + " " + DataBus.z_coord

        xPos = parseFloat(pos[0])
        yPos = parseFloat(pos[1])
        zPos = parseFloat(pos[2])

        DataBus.xPos = xPos
        DataBus.yPos = yPos
        DataBus.zPos = zPos
    }

    function parseGpio(msg) {

    }


    DebugLog { }

    OpenSerialPromise { id: openSerialPromise }

    Component.onCompleted: {
        operatorUser.visible = true
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
