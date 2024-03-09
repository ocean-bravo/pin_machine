import QtQml 2.12
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Loader {
    id: loader

    function show() {
        loader.sourceComponent = comp
    }
    function hide() {
        loader.sourceComponent = undefined
    }
    onLoaded: {
        item.open()
        item.closed.connect(loader.hide)
    }

    property var options

    property var optionsModified

    signal closed

    property color backgroundColor: "white"

    width: 480//appWin.width * 0.75
    height: appWin.height / 2

    Component {
        id: comp

        Popup {
            id: root
            width: loader.width
            height: loader.height
            anchors.centerIn: Overlay.overlay
            modal: true
            closePolicy: Popup.CloseOnPressOutside | Popup.CloseOnEscape

            z: loader.z

            property color backgroundColor: loader.backgroundColor

            background: Rectangle {
                color: root.backgroundColor
            }

            BlobSettings2 {
                id: blobSettings
                anchors.fill: parent
                options: loader.options
            }

            onClosed: {
                loader.optionsModified = blobSettings.options
                loader.closed()
            }
        }
    }
}
