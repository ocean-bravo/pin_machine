import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

// Control {
//     id: root

Loader {
    id: loader
    //width: parent.width
    //anchors.fill: parent
    //height: contentHeight
    property string path: ""  // Эта строка меняется на нужную

    function reload() {
        loader.active = false
        //loader.source = ""
        QmlEngine.clearCache()
        loader.setSource(path)
        loader.active = true
        //loader.source = path
    }

    source: path

    onLoaded : {
        //Layout.preferredHeight =  item === null ? 10 : item.contentHeight
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
