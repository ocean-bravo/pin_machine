import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15


Window {
    id: root
    title: "Debug log"
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint

    width: MainWindow.width / 3
    height: MainWindow.height / 2

    x: Screen.width - width
    y: Screen.height - height

    RowLayout {
        anchors.fill: parent

        MouseArea {
            id: leftEdgeWindowResizeMouseArea
            onEntered: cursorShape = Qt.SizeHorCursor
            onExited: cursorShape = Qt.ArrowCursor
            onPressed: startSystemResize(Qt.LeftEdge)
            acceptedButtons: Qt.LeftButton
            hoverEnabled: true

            Layout.preferredWidth: 5
            Layout.fillHeight: true
        }

        Log2 {
            id: debugLog
            loadOnCompleted: true
            selectionEnabled: true
            Connections { target: Logger; function onNewMessage(message) { debugLog.append(message); } }

            Layout.fillWidth: true
            Layout.fillHeight: true
        }
        MouseArea {
            anchors.fill: debugLog
            acceptedButtons: Qt.RightButton

            onClicked: contextMenu.popup()

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
                    text: "Ignore"
                    enabled: debugLog.selectedText.length > 0
                    onTriggered: {
                        mb.show()
                    }
                }
            }
        }
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

    SmallMessageBox {
        id: mb
    }

    component SmallMessageBox : MessageBoxLoader {
        width: 400
        anchors.centerIn: Overlay.overlay
        height: 200
        hasCancelButton: true
        text: "Ignore?\n" + debugLog.selectedText
        onReject: hide()
        onAccept: {
            debugLog.ignoreSelected()
            hide()
        }

        backgroundColor: "maroon"
    }
}
