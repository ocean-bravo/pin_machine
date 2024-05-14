import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15

Item {
    id: root

    property bool guiDebug: false
    readonly property string mainFont : "DINPro"

    //property var appWin: operatorUser

    property string status: "Idle"
    property string fullStatus

    property real xPos
    property real yPos
    property real zPos

    width: MainWindow.width
    height: MainWindow.height

    StackLayout {
        id: stackLayout
        anchors.fill: parent

        SuperUser {
            id: superUser
        }

        OperatorUser {
            id: operatorUser
        }
    }

    FindCamera {

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

    QtObject {
        id: colors

        readonly property color primary_10: "#F6FAFF"
        readonly property color primary_20: "#DBEAFE"
        readonly property color primary_50: "#60A5FA"
        readonly property color primary_60: "#3B82F6"
        readonly property color primary_70: "#2563EB"
        readonly property color primary_80: "#1D4ED8"
        readonly property color primary_90: "#1E40AF"

        readonly property color black_20: "#CCCCCC"
        readonly property color black_30: "#B3B3B3"
        readonly property color black_40: "#999999"
        readonly property color black_50: "#808080"
        readonly property color black_80: "#333333"
        readonly property color black_90: "#1A1A1A"
        readonly property color black_100: "#000000"

        readonly property color error_80: "#FF5E5C"

        readonly property color accent_90: "#FFC800"

        readonly property color success_90: "#14B8A6"

        readonly property color green: "#14B842"

        readonly property color disabledButton: "#E5E5E5"
    }

    Connections {
        target: Serial

        function onData(msg) {
            // console.log("___________________________________________________")
            // console.log(String(msg))
            // console.log("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^")

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

    function write(msg) {
        // Во время хоуминга разрешаю только команду запроса статуса
        if (status === "Home" && msg !== "?")
            return

        Serial.write(msg + "\n")

        operatorUser.appendLog(msg)
        superUser.appendToCommandsLog(msg)
    }

    function jog(axis, mm, feed) {
        if (feed === undefined)
            feed = 1000
        write("$J=G91 " + axis + mm + " F" + feed)
    }

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

            // Просто по набору признаков в сообщении, определяю, является ли оно GPIO статусом.
            // Пока настройка по 2 признакам:
            // 1. "15 GPIO" (к примеру)
            // 2. "Input Matrix"
            // Возможно понадобится расширить список признаков.
            if (line.match(/[\d]{1,2} GPIO[\d]{1,2}|Input Matrix/)) {
                gpioMessage = true // Не показывать в логе всё сообщение
            }

            // Проверяю каждую строку не GPIO ли
            // Бывают пины вида 19 GPIO19 I0 O0 HSPIQ_out - что с ними делать не знаю
            if (line.match(/[\d]{1,2} GPIO[\d]{1,2} [IO][01]/)) {
                let pin =  parseGpioPin(line)

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

    LogMachine {
        id: logMachine

        Shortcut { sequence: "F11"; context: Qt.ApplicationShortcut;
            onActivated: {
                logMachine.visible = !logMachine.visible
                if (logMachine.visible)
                    logMachine.raise()
            }
        }
    }

    LogDebug {
        id: logDebug

        Shortcut { sequence: "F12"; context: Qt.ApplicationShortcut;
            onActivated: {
                logDebug.visible = !logDebug.visible
                if (logDebug.visible)
                    logDebug.raise()
            }
        }
    }

    OpenSerialPromise { id: openSerialPromise }

    Component.onCompleted: {
        DataBus.live_preview_mode = "raw"
        openSerialPromise.runAsync()
    }

    Shortcut { sequence: "F1"; context: Qt.ApplicationShortcut;
        onActivated: {
            stackLayout.currentIndex = 0

            //appWin = operatorUser
        }
    }

    Shortcut { sequence: "F2"; context: Qt.ApplicationShortcut;
        onActivated: {
            stackLayout.currentIndex = 1
            operatorUser.panelIndex = 0
            // appWin = superUser
        }
    }

    Shortcut {
        sequence: "F3"
        context: Qt.ApplicationShortcut
        onActivated: {
            stackLayout.currentIndex = 1
            operatorUser.panelIndex = 1
        }
    }

    Shortcut {
        sequence: "F4"
        context: Qt.ApplicationShortcut
        onActivated: {
            stackLayout.currentIndex = 1
            operatorUser.panelIndex = 2
        }
    }

    Shortcut { sequence: "F5"; context: Qt.ApplicationShortcut;
        onActivated: {
            const messageBox = {
                headerText: qsTr("Ошибка осей/Аварийная остановка"),
                mainText: qsTr("Работа остановлена.\nДля продолжения необходимо выполнить инициализацию осей.\nУдостоверьтесь, что продукт извлечен и нет помех для движения."),
                buttonText1: qsTr("Ok"),
            }

            DataBus.messageBox = JSON.stringify(messageBox)
        }
    }

    Shortcut { sequence: "F6"; context: Qt.ApplicationShortcut;
        onActivated: {
            const messageBox = {
                headerText: qsTr("Предыдущий продукт не закончен"),
                mainText: qsTr("Вы хотите продолжить установку с места окончания предыдущего цикла?\nИли начать сначала?"),
                buttonText1: qsTr("Продолжить"),
                buttonText2: qsTr("Начать сначала"),
            }

            DataBus.messageBox = JSON.stringify(messageBox)
        }
    }

    Shortcut { sequence: "F7"; context: Qt.ApplicationShortcut;
        onActivated: {
            const messageBox = {
                headerText: qsTr("Пауза. Нет линии пинов."),
                mainText: qsTr("Необходимо заправить пины в установочное устройство."),
                buttonText1: qsTr("Прервать установку"),
                buttonText2: qsTr("Продолжить в пошаговом режиме"),
                buttonText3: qsTr("Продолжить"),
                bgColor1: "white"
            }

            DataBus.messageBox = JSON.stringify(messageBox)
        }
    }

    Shortcut { sequence: "F8"; context: Qt.ApplicationShortcut;
        onActivated: {
            guiDebug = !guiDebug
        }
    }

    Shortcut { sequence: "F9"; context: Qt.ApplicationShortcut;
        onActivated: {
            //QmlEngine.clearCache()
            //appWin.close()
            //ApplicationWindow.close()
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
