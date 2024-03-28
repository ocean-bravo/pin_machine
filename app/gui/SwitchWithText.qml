import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    color: "transparent"
    width: 200
    height: 30

    property alias text: text.text
    property alias checked : switchElement.checked
    property bool interactive: true //как аналог enabled, но чтобы работала MouseArea для перехвата кликов
    property string tooltipText

    Row {
        anchors.fill: parent
        anchors.margins: 0
        spacing: 5

        Item {
            id: switchItem
            width: root.width * 0.4 - 5
            height: 30
            anchors.verticalCenter: parent.verticalCenter

            CustomSwitch {
                id: switchElement
                anchors.fill: parent
            }

            MouseArea {
                anchors.fill: parent
                enabled: !root.interactive

                onClicked: {

                }
            }
        }

        SmText {
            id: text
            width: root.width * 0.6
            height: parent.height
            anchors.verticalCenter: parent.verticalCenter
            color: switchElement.checked ? mainLabelsColor : "grey"
            font.pixelSize: 18
            wrapMode: Text.WordWrap
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignRight
        }

    }
}

