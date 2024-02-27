import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15

GroupBox {
    id: root
    width: 100
    height: 26

    topPadding:30
    bottomPadding: 0

    property alias checked: checkBox.checked

    property alias text: checkBox.text

    Behavior on height {
        NumberAnimation { duration: 10 }
    }

    //padding: 0
    // label: Rectangle {
    //     color: "#8000FF00"
    //     x: root.leftPadding
    //     width: root.availableWidth
    //     height: 25

    //     CheckBox {
    //         id: checkBox
    //         text: root.title
    //         anchors.fill: parent
    //     }
    // }
    //bottomPadding: 0

    label: CheckBox {
        id: checkBox
        checked: false

        spacing: 0
        bottomPadding: 0
        topPadding: 0

        indicator: Text {
            id: indic

            width: 26
            height: 26

            x: checkBox.leftPadding
            y: parent.height / 2 - height / 2

            text: checkBox.checked ? "➖" : "➕"
            font.pixelSize: 25
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }

        contentItem: Text {
            text: checkBox.text
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHLeft
            leftPadding: indic.width + checkBox.spacing
        }
    }

    background: Rectangle {
        //y: root.topPadding - root.bottomPadding
        anchors.top: checkBox.bottom
        width: parent.width
        height: parent.height - root.topPadding + root.bottomPadding
        //color: "transparent"
        color: "#8000FF00"
        border.color: "#21be2b"
        radius: 2
    }




}
