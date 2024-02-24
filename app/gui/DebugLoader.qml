import QtQml 2.12
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

// Control {
//     id: root

Loader {
    id: loader
    //width: parent.width
    //anchors.fill: parent
    //height: contentHeight
    property string path: ""  // Эта строка меняется на нужную

    function reload() {
        loader.source = ""
        QmlEngine.clearCache()
        loader.source = path
    }

    source: path

    onLoaded : {
        //Layout.preferredHeight =  loader.item === null ? 10 : loader.item.contentHeight
    }

    Connections {
        target: FileSystemWatcher
        function onFileChanged (path) {
            if (path === loader.path)
                loader.reload()
        }
    }

    // Костыль. Проблемы какие-то постоянно с filesystemwatcher
    Timer {
        interval: 100
        running: true
        repeat: true
        onTriggered: { FileSystemWatcher.addPath(loader.path) }
    }
}
// }
