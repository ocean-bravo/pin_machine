import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Button {
    id: root
    spacing: 4

    leftPadding: 16
    rightPadding: 16

    icon.height: 20
    icon.width: 20

    font.family: mainFont
    font.pixelSize: 16
    font.weight: Font.Medium

    property color color: "blue"
    property color framecolor: color

    Layout.preferredHeight: 40

    display: {
        if (icon.source && text) return AbstractButton.TextBesideIcon
        if (icon.source) return AbstractButton.IconOnly
        return AbstractButton.TextOnly
    }

    background: Rectangle {
        color: down ? "lightgrey" : "transparent"
        opacity: enabled ? 1 : 0.3
        border.color: root.framecolor
        border.width: 1
        radius: 8
    }

    // contentItem:
    //     RowLayout {
    //         //anchors.fill: item
    //         spacing: 0

    //         Hspacer {}

    //         Image {
    //             id: image
    //             //enabled: icon.source !== ""

    //             width: root.icon.width
    //             height: root.icon.height
    //             source: root.icon.source
    //             mipmap: true

    //             // /Layout.preferredHeight:  root.icon.height
    //         }

    //         Text {
    //             text: root.text
    //             font: root.font
    //             opacity: enabled ? 1.0 : 0.3
    //             color: root.color
    //             horizontalAlignment: Text.AlignHCenter
    //             verticalAlignment: Text.AlignVCenter
    //             // /elide: Text.ElideRight
    //             wrapMode: Text.WordWrap
    //             // lineHeightMode: Text.FixedHeight
    //             // lineHeight: 16

    //             //Layout.fillHeight: true
    //             // /Layout.preferredWidth: implicitWidth
    //         }

    //         Hspacer {}
    //     }


    Component.onCompleted: {
        // Не хотел переопределять свойство contentItem, а добраться до нужного свойства у существуюущего
        // Всего лишь нужно было поменять цвет текста
        contentItem.color = Qt.binding(() => root.color)
        //contentItem.wrapMode = Text.WordWrap
        //console.log(JSON.stringify(contentItem.contentItem.keys))




    }


}
