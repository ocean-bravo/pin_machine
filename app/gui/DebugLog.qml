import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15


Window {
    id: root
    title: "Debug log"
    flags: Qt.FramelessWindowHint

    width: MainWindow.width / 3
    height: MainWindow.height / 2

    x: Screen.width - width
    y: Screen.height - height

    Log2 {
        id: debugLog
        anchors.fill: parent
        loadOnCompleted: true
        selectionEnabled: true
        Connections { target: Logger; function onNewMessage(message) { debugLog.append(message); } }
    }

    Shortcut {
        sequence: "F12"
        context: Qt.ApplicationShortcut
        onActivated: {
            root.visible = !root.visible
            if (root.visible)
                root.raise()
        }
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.RightButton
        onClicked: {
            contextMenu.popup()
        }
        onPressAndHold: {
            if (mouse.source === Qt.MouseEventNotSynthesized)
                contextMenu.popup()
        }

        Menu {
            id: contextMenu
            MenuItem {
                text: "Clear"
                onTriggered: debugLog.clear()
            }

            MenuItem {
                text: "Stay on top"
                onTriggered: root.flags = Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
            }

            MenuItem {
                text: "Not stay on top"
                onTriggered: root.flags = Qt.FramelessWindowHint
            }
        }
    }
}
