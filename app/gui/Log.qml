import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15

ScrollView {
    id: root
    ScrollBar.vertical.policy: ScrollBar.AlwaysOn

    function append(text) {
        log.insert(log.length, text)
    }

    function clear() {
        log.clear()
    }

    background: Rectangle {
        implicitWidth: 200
        implicitHeight: 200
        border.color: "#444"
        color: "#F4F4F4"
    }

    clip: true

    TextArea {
        id: log
        textFormat: Text.RichText
        width: 500
        selectByMouse: true
        readOnly: true
        font.family: "Consolas"

        Timer {
            interval: 500
            running: true
            repeat: true
            onTriggered: {
                const length = log.length

                if (length > 10000)
                    remove(0, length - 10000)
            }
        }
    }
}
