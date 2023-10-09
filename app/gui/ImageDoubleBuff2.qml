import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root

    function setSource(source){
        var imageNotVis = image.visible ? image2 : image
        imageNotVis.source = ""
        imageNotVis.source = source
    }

    property real zoom: 0.0
    property real zoomStep: 0.1

    component MyImage :
        Image {
            asynchronous: true
            cache: false
            smooth: false
            antialiasing: false
            mipmap: false

            anchors.centerIn: parent
            fillMode: Image.PreserveAspectFit
            transformOrigin: Item.Center
            scale: Math.min(root.width / width, root.height / height, 1) + zoom
        }

    Flickable {
        anchors.fill: parent

        contentWidth: Math.max(image.width * image.scale, root.width)
        contentHeight: Math.max(image.height * image.scale, root.height)
        clip: true

        MyImage {
            id: image
            visible: true
            onStatusChanged: {
                if (status === Component.Ready) {
                    visible = true
                }
            }
        }

        MyImage {
            id: image2
            visible: !image.visible
            onStatusChanged: {
                if (status === Component.Ready) {
                    image.visible = false
                }
            }
        }
    }

    // Mouse zoom
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.NoButton

        onWheel: {
           // let im = (image.visible ? image : image2)
            if (wheel.angleDelta.y > 0)
                zoom = Number((zoom + zoomStep).toFixed(1))
            else
                if (zoom > 0) zoom = Number((zoom - zoomStep).toFixed(1))

            wheel.accepted=true
        }
    }
}
