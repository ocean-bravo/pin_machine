import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

CollapsiblePanel {
    id: root

    width: parent.width
    height: checked ? 30  + rect.height: 30

    Layout.preferredWidth: 400
    Layout.preferredHeight: checked ? 30  + rect.height: 30

    checked: false

    text: qsTr("Blob detection settings")

    onCheckedChanged: {
        rect.visible = checked
    }
    Component.onCompleted: {
        rect.visible = checked
    }

    Item {
        id: rect
        //color: "#400000FF"

        height: blobSettings.height

        anchors.top:parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins:0

        // DebugLoader {
        //     id: blobSettings
        //     anchors.left: parent.left
        //     anchors.right: parent.right
        //     anchors.top:parent.top
        //     path: "/home/mint/devel/pin_machine/app/gui/BlobSettings.qml"
        // }

        BlobSettings {
            id: blobSettings
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top:parent.top
        }
    }
}
