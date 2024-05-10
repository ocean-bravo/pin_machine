import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Button {
    id: root
    text: qsTr("Button")
    spacing: 4

    leftPadding: 16
    rightPadding: 16

    icon.height: 20
    icon.width: 20

    font.family: mainFont
    font.pixelSize: 16

    property color color: "blue"
    property color framecolor: color

    display: icon.source === "" ? AbstractButton.TextOnly : AbstractButton.TextBesideIcon

    background: Rectangle {
        color: down ? "lightgrey" : "transparent"
        opacity: enabled ? 1 : 0.3
        border.color: root.framecolor
        border.width: 1
        radius: 8
    }

    // contentItem: Text {
    //     text: root.text
    //     font: root.font
    //     opacity: enabled ? 1.0 : 0.3
    //     color: root.color
    //     horizontalAlignment: Text.AlignHCenter
    //     verticalAlignment: Text.AlignVCenter
    //     elide: Text.ElideRight
    // }

    // indicator: Image {
    //     id: image

    //     width: icon.width
    //     height: icon.height
    //     //anchors.centerIn: parent
    //     source: root.icon.source
    //     mipmap: true
    // }

    Component.onCompleted: {
        // Не хотел переопределять свойство contentItem, а добраться до нужного свойства у существуюущего
        // Всего лишь нужно было поменять цвет текста
        contentItem.color = Qt.binding(() => root.color)
    }
}
