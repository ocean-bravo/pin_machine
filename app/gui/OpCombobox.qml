import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtGraphicalEffects 1.15

ComboBox {
    id: root

    Layout.preferredHeight:  40

    model: ["One", "Two", "Three", "Four", "5", "6", "7", "8", "9", "10", "11"]

    //text fontsize
    // property int fontsize: 18
    // property color colortext: mainLabelsColor
    // property color colorIndicator: mainLabelsColor

    // Текст добавляемый после значения модели
    property string suffix

    readonly property var currentVal: model[currentIndex]

    delegate: ItemDelegate {
        width: root.width
        contentItem: OpText {
            text: modelData + root.suffix
            color: colors.primary_60
            font.bold: true
            font.pixelSize: 13
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
        }
        highlighted: root.highlightedIndex === index
        background: Rectangle {
            border.color: colors.primary_60
            border.width: 2
            radius: 8
        }
    }

    //draw the right side conrner dinamically
    indicator: Canvas {
        id: canvas
        x: root.width - width - root.rightPadding
        y: root.topPadding + (root.availableHeight - height) / 2
        width: 15
        height: 10
        contextType: "2d"

        onPaint: {
            context.reset()
            context.moveTo(0, 0)
            context.lineTo(width, 0)
            context.lineTo(width / 2, height)
            context.closePath()
            context.fillStyle = colors.primary_50
            context.fill()
        }
    }

    contentItem: OpText {
        leftPadding: 10
        text: root.currentText + root.suffix
        font.pixelSize: 13
        font.weight: Font.Medium
        //color: colortext
        color: colors.primary_50
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        border.color: colors.primary_50
        border.width: 2
        radius: 8
    }

    popup: Popup {
        y: root.height
        width: root.width
        implicitHeight: contentItem.implicitHeight

        // Все равно не работает как надо. Код ниже использовать как стартовую точку, когда все таки надо будет починить.
        // Компонент работает нормально, когда число элементов в модели столько, что выпадающий список помещается между нижним краем
        // ComboBox и нижним краем экрана.
        // Также все хорошо, когда число элементов больше 10 - т.е. выпадающий список не помещается на экране целиком - popup располагается под
        // нижней стороной СomboBox и прокручивается.
        // Когда число элементов между этими порогами - popup показывается на экране целиком, игноририруя явные заданные ему координаты.
        // Поэтому я вычисляю место под нижним краем ComboBox до низа экрана и высоту контента и задаю минимальное из этих значений
        //height: Math.min(contentItem.implicitHeight, appWin.height - globalPosition().y - root.height )
        // Получаю глобальные координаты себя в окне
        //function globalPosition() {
        //    var point = mapToItem(null, root.x, root.y)
        //    return point
        //}

        padding: 0

        contentItem: ListView {
            clip: true
            boundsBehavior: Flickable.StopAtBounds
            implicitHeight: contentHeight
            model: root.popup.visible ? root.delegateModel : null
            currentIndex: root.highlightedIndex
        }

        background: Rectangle {
            color: "transparent"
        }
    }
}
