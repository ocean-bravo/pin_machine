import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

CollapsiblePanel {
    id: root

    width: parent.width
    height: checked ? 30  + rect.height : 30

    // Layout.preferredWidth: 400
    // Layout.preferredHeight: checked ? 30  + rect.height : 30

    checked: false

    text: qsTr("Blob detection settings")

    onCheckedChanged: {
        rect.visible = checked
    }
    Component.onCompleted: {
        rect.visible = checked
    }

    //padding: 0
    // spacing: 0
    // bottomInset: 0
    // topInset: 0


    Rectangle {
        id: rect
        color: "#400000FF"

        height: blobSettings.height
        anchors.top:parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins:0

        //anchors.margins: -10
        //anchors.fill: parent

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

        // ColumnLayout {
        //     id: column
        //     anchors.fill: parent


        // }
    }

    // ColumnLayout {
    //     id: column
        //width: parent.width
        // Rectangle
        // {
        //     Layout.preferredHeight: 200
        //     Layout.fillWidth: true
        //     color: "yellow"

        // Pane {
        //     id: pane
        //     //width: parent.width
        //     Layout.preferredHeight: loader.item === null ? 10 : loader.item.contentHeight
        //     //Layout.preferredWidth: parent.width
        //     Layout.fillWidth: true
        //     padding: 0
        //     spacing: 0
        //     leftInset: 0
        //     rightInset: 0
        //     topInset: 0
        //     bottomInset: 0

        // DebugLoader {
        //     id: loader
        //     //width: parent.width
        //     //anchors.fill: parent
        //     Layout.preferredHeight: item.height  //loader.item === null ? 10 : loader.item.contentHeight
        //     Layout.fillWidth: true
        //     path: "/home/mint/devel/pin_machine/app/gui/BlobSettings.qml"
        // }

        // Loader {
        //     id: loader
        //     width: parent.width
        //     //anchors.fill: parent
        //     //height: contentHeight
        //     readonly property string path: "/home/mint/devel/pin_machine/app/gui/BlobSettings.qml"  // Эта строка меняется на нужную

        //     function reload() {
        //         loader.source = ""
        //         QmlEngine.clearCache()

        //         loader.source = path
        //     }

        //     source: path

        //     onLoaded : {
        //         Layout.preferredHeight =  loader.item === null ? 10 : loader.item.contentHeight

        //     }

        //     Connections { target: FileSystemWatcher; function onFileChanged (path) {
        //         if (path === loader.path) {
        //             loader.reload()
        //         }

        //     }
        //     }

        //     // Костыль. Проблемы какие-то постоянно с filesystemwatcher
        //     Timer { interval: 100; running: true; repeat: true; onTriggered: {  FileSystemWatcher.addPath(loader.path) }
        //     }
        //}
        //}


    //     BlobSettings {
    //         id: blobSettings

    //         Layout.preferredWidth: 400
    //         //Layout.preferredHeight: 500
    //         //anchors.fill: parent
    //     }
    //     //}
    // }
}
