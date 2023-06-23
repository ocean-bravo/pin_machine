import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15

ScrollView {
    id: root
    ScrollBar.vertical.policy: ScrollBar.AlwaysOn
    property alias text: editor.text
    property alias readOnly: editor.readOnly

    function highlightLine(line) {
        editor.cursorPosition = line
        editor.select(0, 20)

    }

    background: Rectangle {
        implicitWidth: root.width
        implicitHeight: root.height
        border.color: "red"
        color: editor.readOnly ? "#FAFAFA" : "#F4F4F4"
    }

    clip: true

    TextArea {
        id: editor
        width: root.width
        height: 100
        selectByMouse: true
    }
}
