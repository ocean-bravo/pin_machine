import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: root

    property bool guiDebug: false

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

        let msg = String(message) // Копирую строку фактически
        // msg = msg.replace(/\r?\n/g, '<br>')
        // msg = String(Date.now()).slice(-4) + ": " + msg
        msg = "<font color=" + color + ">" + msg + "</font>"
        return msg
    }

    Connections {
        target: Serial

        function onData(msg) {
            console.log("_________________")
            console.log(String(msg))
            console.log("^^^^^^^^^^^^^^^^^")

            const [stat, showLines] = parseSerialMessage(msg)

            if (stat !== undefined) {
                superUser.appendToCommandsLog(stat, 'darkblue')
                operatorUser.appendLog(stat, 'darkblue')
            }

            for (let line of showLines) {
                superUser.appendToCommandsLog(line, 'darkblue')
                operatorUser.appendLog(line, 'darkblue')
            }
        }
    }

    //property bool messageFinished: false

    //property string prevMsg: ""

    // В сообщении строки иногда заканчиваютс \r\n иногда \n. Системы не понял. После ок идет \r\n всегда.
    // При хоуминге статус отвечает. Сообщение имеет вид: статус\r\n Но в конце нет ok\r\n. Потом, вдогонку, наваливает кучу ok\r\n
    // Статус всегда помещается в одно сообщение (пока).
    function parseSerialMessage(msg) {
        let lines = msg.split(/\r?\n/)

        let showLines = []

        let gpioMessage = false

        for (let line of lines) {
            // Буду считать, что статус:
            // - ограничен символами < и >
            // - эти символы только в статусе
            // - всегда неделим, т.е. не разбивается переносом строк
            // - в одной строке может быть только статус, без добавлений частей других команд
            if (line.match(/^[<].+[>]$/)) {
                line =  parseStatus(line)
                var status = line
                continue
            }

            // Проверяю каждую строку не GPIO ли
            // Бывают пины вида 19 GPIO19 I0 O0 HSPIQ_out - что с ними делать не знаю

            if (line.match(/[\d]{1,2} GPIO[\d]{1,2} [IO][01]/)) {
                let pin =  parseGpioPin(line)

                gpioMessage = true // Не показывать в логе всё сообщение

                if (pin === null)
                    continue

                let gpio = DataBus.gpio
                gpio[pin.number] = pin
                DataBus.gpio = gpio
                continue
            }

            // Строку из одного ok выбрасываю
            if (line.match(/^ok$/)) {
                continue
            }

            showLines.push(line)
        }

        // Если в сообщение попало что-то похожее на статус GPIO - всё сообщение не показываем. Кроме статуса. Его надо показать.
        if (gpioMessage)
            showLines = []

        return [status, showLines]

        // Если во время хоуминга накидывать многострочные команды вроде GPIO/Dump, то после окончания хоуминга
        // все эти комнды придут, вместе с ok\r\n вперемешку.
        // Т.е. в одном сообщении может быть окончание предыдущей команды, куча ok\r\n (ответов на статус) и начало ответа на след команду.
    }

    function parseStatus(msg) {
        // Симвлы < и > есть во входящих данных. Они интерпретируются как Html. Надо заменить на другие.
        msg = msg.replace(/</g, '')
        msg = msg.replace(/>/g, '')

        let statusValues = msg.split("|")

        status = statusValues[0]
        let position = statusValues[1] // второй элемент - позиция
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

        return msg
    }

    function parseGpioPin(msg) {
        let pinInfo = msg.split(' ')

        let number = pinInfo[0]
        let name = pinInfo[1]
        let dirVal = pinInfo[2]

        // Строка должна состоять из 3 частей. Если их больше - такая строка нас не интересует.
        // Ищем строку вида "15 GPIO15 I0"
        if (pinInfo[3] !== undefined)
            return null

        if (dirVal === 'I0') { var dir = 'input'; var value = '0' }
        if (dirVal === 'I1') { dir = 'input'; value = '1' }
        if (dirVal === 'O0') { dir = 'output'; value = '0' }
        if (dirVal === 'O1') { dir = 'output'; value = '1' }

        let pin = { number: number, name: name, dir: dir, value: value }

        return pin
    }

    function parseGpio(msg) {
        let lines = msg.split(/\r?\n/)

        let pins = new Array(32)

        for (let i = 0; i < pins.length; ++i)
            pins[i] = {};

        while (lines.length > 0) {
            let line = lines.shift()

            let pinInfo = line.split(' ')

            let number = pinInfo[0]

            // Дальше идет раздел Input Matrix. Он не нужен
            if (number === "Input")
                break

            let name = pinInfo[1]
            let dirVal = pinInfo[2]

            // Для пина нет информации о выходе или выходе
            if (dirVal === undefined)
                continue

            if (dirVal === 'I0') { var dir = 'input'; var value = '0' }
            if (dirVal === 'I1') { dir = 'input'; value = '1' }
            if (dirVal === 'O0') { dir = 'output'; value = '0' }
            if (dirVal === 'O1') { dir = 'output'; value = '1' }

            let pin = { number: number, name: name, dir: dir, value: value }

            pins[number] = pin
        }

        DataBus.gpio = pins
    }

    DebugLog { }

    OpenSerialPromise { id: openSerialPromise }

    Component.onCompleted: {
        operatorUser.visible = false
        superUser.visible = true
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

    function parseAlarms(msg) {
        console.log("parse Alarms")

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
    }
}
