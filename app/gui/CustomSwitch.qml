import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15

Switch {
    id: root
    text: qsTr("Switch")

    leftPadding: 0

    font.pixelSize: 16

    indicator: Rectangle {
        implicitWidth: 48
        implicitHeight: 26

        x: root.leftPadding
        y: parent.height / 2 - height / 2

        radius: 13
        color: down ? "lightgrey" : checked ? colors.primary_70 : colors.primary_20
        border.color: "transparent"
        border.width: 0

        Rectangle {
            x: checked ? parent.width - width - 3 : 3
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
        text: root.text
        font.pixelSize: root.font.pixelSize
        opacity: enabled ? 1.0 : 0.3
        color: colors.black_90
        verticalAlignment: Text.AlignVCenter
        leftPadding: root.indicator.width + root.spacing
    }
}
