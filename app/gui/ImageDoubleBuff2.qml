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

    property real zoomStep: 0.1

    component MyImage :
        Image {
        asynchronous: true
        cache: false
        smooth: false
        antialiasing: false
        mipmap: false

        //anchors.centerIn: parent
        fillMode: Image.PreserveAspectFit
        //transformOrigin: Item.Center

    }

    property real zoom: 1;
    onZoomChanged: {
            var zoomPoint = Qt.point(flickArea.width/2 + flickArea.contentX,
                                 flickArea.height/2 + flickArea.contentY);

            flickArea.resizeContent((inner.width * zoom), (inner.height * zoom), zoomPoint);
            flickArea.returnToBounds();
        }

    Flickable {
        id: flickArea
        anchors.fill:  parent

        contentWidth: inner.width
        contentHeight: inner.height
        clip: true
        focus: true

        //boundsBehavior: Flickable.StopAtBounds

        Rectangle {
            id: inner
            scale: zoom
            color: "yellow"
            anchors.centerIn: parent
            width: Math.max(image.width, image2.width)
            height: Math.max(image.height, image2.height)
            transform: Scale {
                                id: scaleID ;
                                origin.x: flickArea.contentX + flickArea.width * flickArea.visibleArea.widthRatio / 2
                                origin.y: flickArea.contentY + flickArea.height * flickArea.visibleArea.heightRatio / 2
            }
            MyImage {
                id: image
                anchors.centerIn: parent
                visible: true
                onStatusChanged: {
                    if (status === Component.Ready) {
                        visible = true
                    }
                }
            }

            MyImage {
                id: image2
                //anchors.fill: parent
                anchors.centerIn: parent
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
            acceptedButtons: Qt.LeftButton | Qt.RightButton | Qt.Wheel //Qt.NoButton



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


}
