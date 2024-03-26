import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    width: 100
    height: head.height + cont.height
    color: "transparent"

    property alias checked: checkBox.checked

    property alias text: label.text

    // Объект _container становится родителем элементов, которые поместят внутри компонента CollapsiblePanel
    default property alias _content: cont.data

    Rectangle {
        id: head
        width: parent.width
        anchors.top: parent.top
        height: 30

        //color: "lightgrey"

        gradient: Gradient {
                  GradientStop { position: 0.0; color:  (("#AA98A3B4")) }
                  GradientStop { position: 1.0; color:  (("#AA5A6B83")) }
        }

        CheckBox {
            id: checkBox
            anchors.fill: parent
            checked: false

            spacing: 0
            bottomPadding: 0
            topPadding: 0

            indicator: Text {
                id: indic

                width: 26
                height: 26

                x: checkBox.leftPadding
                y: parent.height / 2 - height / 2

                text: checkBox.checked ? "▽" : "▷"
                font.pixelSize: 20
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }

            contentItem: Text {
                // text: label.text
                id:label
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHLeft
                leftPadding: indic.width// + label.spacing
            }
        }
    }

    Flickable {
        id: cont
        width: parent.width
        height: checkBox.checked ? childrenRect.height : 0
        anchors.top: head.bottom
        clip: true

        Behavior on height {
            NumberAnimation { duration: 100 }
        }
    }
}
