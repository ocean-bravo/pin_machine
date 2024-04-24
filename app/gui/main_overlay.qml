import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15

Item {
    id: root

    width: MainWindow.width
    height: MainWindow.height

    enabled: false

    QtObject {
        id: colors

        readonly property color primary_10: "#F6FAFF"
        readonly property color primary_20: "#DBEAFE"
        readonly property color primary_50: "#60A5FA"
        readonly property color primary_70: "#2563EB"
        readonly property color primary_90: "#1E40AF"

        readonly property color black_20: "#CCCCCC"
        readonly property color black_30: "#B3B3B3"
        readonly property color black_50: "#808080"
        readonly property color black_80: "#333333"
        readonly property color black_90: "#1A1A1A"

        readonly property color error_80: "#FF5E5C"

        readonly property color accent_90: "#FFC800"

        readonly property color success_90: "#14B8A6"

        readonly property color green: "#14B842"

        readonly property color disabledButton: "#E5E5E5"
    }





    // FlashMessageBox {
    //     id: message
    //     text: DataBus.messagebox
    //     backgroundColor: "maroon"
    //     interval: 2000
    //     anchors.centerIn: parent
    //     onTextChanged: if (text.length > 0) open()
    //     onClosed: DataBus.messagebox = "" // перезарядка, чтобы одно и тоже сообщение могло показываться
    //     Component.onCompleted: DataBus.messagebox = ""  // Для убирания warninga "Unable to assign [undefined] to QString"
    // }

    MessageBoxLoader {
        id: messageWithOk
        width: 500
        height: 250
        text: DataBus.messagebox
        backgroundColor: "maroon"
        okButtonText: qsTr("Close")
        anchors.centerIn: parent
        onTextChanged: {
            if (text.length > 0) {
                //QuickWidget2.enabled = true
                //root.enabled = true
                Engine.setOverlayWidgetTransparent(false)
                show()
            }
        }
        onAccept: {
            //QuickWidget2.enabled = false
            //root.enabled = false

            Engine.setOverlayWidgetTransparent(true)
            hide()
            DataBus.messagebox = "" // перезарядка, чтобы одно и тоже сообщение могло показываться
        }
        Component.onCompleted: DataBus.messagebox = ""  // Для убирания warninga "Unable to assign [undefined] to QString"
    }

    MessageBoxLoader {
        id: splash
        text: DataBus.splash
        backgroundColor: "green"
        noButtons: true
        onTextChanged: text.length > 0 ? show() : hide()
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
