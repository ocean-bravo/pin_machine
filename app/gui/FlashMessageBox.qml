import QtQml 2.12
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

MessageBox {
    id: root
    text: qsTr("Not valid")
    backgroundColor: "maroon"
    noButtons: true
    onOpened: autoCloseTimer.start()

    property int interval: 500

    width: 500
    height: 250

    Timer {
        id: autoCloseTimer
        interval: root.interval
        repeat: false
        running: false
        onTriggered: root.close()
    }

    Overlay.modal: Rectangle {
        color: Qt.rgba(0,0,0,0.4)
        Behavior on opacity {
            enabled: root.visible // Чтобы работало только на погасании окна.
            NumberAnimation { duration: 500; easing.type: Easing.InQuad}
        }
    }

    enter: Transition {
        NumberAnimation {property: "opacity"; from: 0; to: 1; duration: 10; easing.type: Easing.OutQuad}
    }

    exit: Transition {
        NumberAnimation {property: "opacity"; from: 1; to: 0; duration: 500; easing.type: Easing.InQuad}
    }
}
