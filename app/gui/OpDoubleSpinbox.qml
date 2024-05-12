import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

// BUG, косяк с редактированием. Если нажимать в места down и up кнопок, хотя их не видно - значение изменяется. Возможно что-то с activeFocus

Item {
    id: root

    property real from: -300
    property real to: 300
    property int decimals: 2
    property bool editable: true // Похоже не биндится к этой переменной!!!
    property real value: 0.0

    width: 80
    height: 40

    Layout.preferredWidth: 80
    Layout.preferredHeight: 40

    //onValueChanged: spinbox.value = root.value * self.factor

    signal valueModified

    property alias text: spinbox.displayText

    SpinBox {
        id: spinbox
        anchors.fill: parent
        font.pixelSize: 16
        font.family: mainFont
        font.weight: Font.Medium
        leftPadding: 12
        focus: false

        value: root.value * self.factor
        stepSize: 1
        editable: root.editable && activeFocus
        from: Math.min(root.from, root.to) * self.factor
        to: Math.max(root.from, root.to) * self.factor
        locale: Qt.locale("en_US") // Чтобы использовалась точка для десятичных разрядов
        wheelEnabled: activeFocus

        onEditableChanged: {
            console.log("editable ", editable)
        }

        onValueModified: {
            root.value = spinbox.value / self.factor
            root.valueModified()
        }

        validator: DoubleValidator {
            bottom: Math.min(root.from, root.to) * self.factor
            top: Math.max(root.from, root.to) * self.factor
            locale: "en_US"
        }

        textFromValue: function (value, locale) {
            return Number(value / self.factor).toLocaleString(locale, 'f', root.decimals)
        }

        valueFromText: function (text, locale) {
            return Number.fromLocaleString(locale, text) * self.factor
        }

        Component.onCompleted: {
            //contentItem.selectByMouse = true
            contentItem.color = Qt.binding( () => editable ? colors.black_100 : colors.black_40)
        }

        background: Rectangle {
            radius: 8
            border.width: 1
            border.color: colors.black_20
        }

        down.indicator.visible: root.editable && activeFocus
        down.indicator.enabled: root.editable && activeFocus
        up.indicator.visible: root.editable && activeFocus
        up.indicator.enabled: root.editable && activeFocus
    }

    // Костыль. Пока непойму, как починить странный BUG с редактированием
    MouseArea {
        anchors.fill: parent
        enabled: Qt.binding(() => root.editable)
        acceptedButtons: Qt.AllButtons
    }

    QtObject {
        id: self
        property int factor: Math.pow(10, root.decimals)
    }
}

