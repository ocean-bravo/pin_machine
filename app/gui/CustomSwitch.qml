import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15

Switch {
    id: control
    text: qsTr("Switch")

    //property alias font: text.font

    indicator: Rectangle {
        implicitWidth: 48
        implicitHeight: 26

        x: control.leftPadding
        y: parent.height / 2 - height / 2

        radius: 13
        color: control.checked ? colors.switchOn : colors.switchOff
        border.color: "transparent"
        border.width: 0

        Rectangle {
            x: control.checked ? parent.width - width - 3 : 3
            y: 3
            width: 20
            height: 20
            radius: 10
            color: "white"
            border.color: "transparent"
            border.width: 0
        }
    }

    contentItem: SmText {
        id: text
        text: control.text
        font.pixelSize: root.font.pixelSize
        opacity: enabled ? 1.0 : 0.3
        color: "#1A1A1A"
        verticalAlignment: Text.AlignVCenter
        leftPadding: control.indicator.width + control.spacing
    }
}
