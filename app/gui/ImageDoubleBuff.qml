import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15


Rectangle {
    id: root
    color: "yellow"
    property int imageVisible: 1

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
            visible: imageVisible === 1
        }

        MyImage {
            id: image2
            visible: imageVisible === 2
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
        var imageNew = imageVisible === 1 ? image2 : image;
        var imageOld = imageVisible === 2 ? image2 : image;

        //imageNew.source = ""

        imageNew.source = source;

        function finishImage(){
            if(imageNew.status === Component.Ready) {
                imageNew.statusChanged.disconnect(finishImage);
                imageVisible = imageVisible === 1 ? 2 : 1;
            }
        }

        if (imageNew.status === Component.Loading){
            imageNew.statusChanged.connect(finishImage);
        }
        else {
            finishImage();
        }
    }
}
