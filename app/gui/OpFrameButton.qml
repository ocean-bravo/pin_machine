import QtQml 2.15
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Button {
    id: root
    text: qsTr("Button")
    spacing: 4
    padding: 16

    display: AbstractButton.TextBesideIcon

    background: Rectangle {
        color: "transparent" // checked ? "lightgrey" : "transparent"
        opacity: enabled ? 1 : 0.3
        border.color: colors.blue
        border.width: 1
        radius: 8
    }

    Component.onCompleted: {
        // Не хотел переопределять свойство contentItem, а добраться до нужного свойства у существуюущего
        // Всего лишь нужно было поменять цвет текста
        contentItem.color = colors.blue
    }
}
