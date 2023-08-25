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
    }
}
