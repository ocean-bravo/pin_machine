import QtQml 2.12
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12

Pane {
    id: pane
    width: parent.width
    height: loader.item === null ? 10 : loader.item.height

    padding: 0
    spacing: 0
    leftInset: 0
    rightInset: 0
    topInset: 0
    bottomInset: 0

    Loader {
        id: loader

        readonly property string path: "/home/mint/devel/pin_machine/app/gui/PanelMoveToPosition.qml"  // Эта строка меняется на нужную

        function reload() {
            loader.source = ""
            QmlEngine.clearCache()

            loader.source = path
        }

        source: path

        onLoaded : {
            parent.height = loader.item.height

        }

        Connections { target: FileSystemWatcher; function onFileChanged (path) {
            if (path === loader.path) {
                loader.reload()
            }

        }
        }

        // Костыль. Проблемы какие-то постоянно с filesystemwatcher
        Timer { interval: 100; running: true; repeat: true; onTriggered: {  FileSystemWatcher.addPath(loader.path) }
        }
    }
}
