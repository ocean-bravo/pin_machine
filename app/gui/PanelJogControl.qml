import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15


CollapsiblePanel {
    id: jogPanel
    width: parent.width
    height: checked ? 440 : 30
    text: qsTr("Jog control")
    //checked: true

    onCheckedChanged: {
        jogControl.visible = checked
    }
    Component.onCompleted: {
        jogControl.visible = checked
    }
    padding: 1

    // RowLayout {
    //     id: jogControl
    //     //margins: 0

    //     //width: parent.width
    //     anchors.fill: parent

    //     Loader {
    //         id: loader

    //         Layout.fillWidth: true
    //         Layout.fillHeight: true


    //         readonly property string path: "/home/mint/devel/pin_machine/app/gui/JogControl.qml"  // Эта строка меняется на нужную

    //         function reload() {
    //             loader.source = ""
    //             QmlEngine.clearCache()

    //             loader.source = path
    //         }

    //         source: path

    //         onLoaded : {
    //             // console.log(parent.height)
    //             // console.log(loader.item.height)
    //             // parent.height = loader.item.height
    //             // loader.item.width = parent.width
    //         }

    //         Connections { target: FileSystemWatcher; function onFileChanged (path) {
    //             if (path === loader.path) {
    //                 loader.reload()
    //             }

    //         }
    //         }

    //         // Костыль. Проблемы какие-то постоянно с filesystemwatcher
    //         Timer { interval: 100; running: true; repeat: true; onTriggered: {  FileSystemWatcher.addPath(loader.path) }
    //         }
    //     }
    // }


    JogControl {
        id: jogControl
        //width: parent.width
        anchors.fill: parent
        //height: parent.height
    }
}
