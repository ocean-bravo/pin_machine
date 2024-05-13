import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15

Button {
    id: root
    padding: 0
    spacing: 4

    font.family: mainFont
    font.pixelSize: 16

    property color textcolor: "white"
    property color bgcolor: "blue"

    Layout.preferredHeight: 40

    display: {
        if (icon.source && text) return AbstractButton.TextBesideIcon
        if (icon.source) return AbstractButton.IconOnly
        return AbstractButton.TextOnly
    }

    background: Rectangle {
        color: down ? "lightgrey" : root.bgcolor
        opacity: enabled ? 1 : 0.3
        radius: 8
    }

    Component.onCompleted: {
        // Не хотел переопределять свойство contentItem, а добраться до нужного свойства у существуюущего
        // Всего лишь нужно было поменять цвет текста
        contentItem.color = Qt.binding(() => root.textcolor)
    }
}
