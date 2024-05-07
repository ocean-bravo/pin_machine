import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15


Window {
    id: machineLogWindow
    title: "Machine log"
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint

    width: Screen.width / 3
    height: Screen.height / 2 - 50

    x: Screen.width - width
    y: 0

    Log2 {
        id: machineLog
        anchors.fill: parent
        loadOnCompleted: true
    }
}
