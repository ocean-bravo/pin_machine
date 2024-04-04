import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15

Switch {
    id: root
    text: qsTr("Switch")

    leftPadding: 0

    font.pixelSize: 16

    indicator: Rectangle {
        implicitWidth: 56
        implicitHeight: 32

        x: root.leftPadding
        y: parent.height / 2 - height / 2

        radius: height / 2
        color: down ? "lightgrey" : checked ? colors.primary_70 : colors.primary_20
        border.color: "transparent"
        border.width: 0

        Rectangle {

            property int brdWidth: parent.height / 2 - height / 2

            x: checked ? parent.width - width - brdWidth : brdWidth
            y: brdWidth
            width: 22
            height: width
            radius: height / 2
            color: "white"
            border.color: "transparent"
            border.width: 0
        }
    }

    contentItem: OpText {
        id: text
        text: root.text
        font.pixelSize: root.font.pixelSize
        font.weight: Font.Medium
        opacity: enabled ? 1.0 : 0.3
        color: colors.black_90
        verticalAlignment: Text.AlignVCenter
        leftPadding: root.indicator.width + root.spacing
        wrapMode: Text.WordWrap
    }

    Rectangle {
        visible: guiDebug
        anchors.fill: parent
        color: "#4000FF00"
    }
}
