import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15


Item {
    id: root

//    property string source

//    onSourceChanged: {
//        setSource(source)
//    }

    component MyImage :
        Image {
            property real zoom: 0.0
            property real zoomStep: 0.1

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
                //console.log("im 1 " , status, source)
            }
        }

        MyImage {
            id: image2
            visible: false
            onStatusChanged: {
                //console.log("im 2 " , status, source)
            }
        }
    }

    // Mouse zoom
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.NoButton

        onWheel: {
            if (wheel.angleDelta.y > 0)
                image.zoom = Number((image.zoom + image.zoomStep).toFixed(1))
            else
                if (image.zoom > 0) image.zoom = Number((image.zoom - image.zoomStep).toFixed(1))

            image2.zoom = image.zoom

            wheel.accepted=true
        }
    }

    function setSource(source){
        //console.log("set source ", source)
        var imageNotVis = image.visible ? image2 : image
        var imageVis = image.visible ? image : image2

        imageNotVis.source = ""

        imageNotVis.source = source

        function finishImage(){
            if(imageNotVis.status === Component.Ready) {
                imageNotVis.statusChanged.disconnect(finishImage)
                image.visible = !image.visible
                image2.visible =!image2.visible
            }
        }

        if (imageNotVis.status === Component.Loading){
            imageNotVis.statusChanged.connect(finishImage)
        }
        else {
            finishImage()
        }
    }
}
