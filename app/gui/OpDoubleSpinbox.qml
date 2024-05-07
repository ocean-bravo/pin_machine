import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root

    property real from: -300
    property real to: 300
    property int decimals: 2
    property bool editable: true
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


        value: root.value * self.factor
        stepSize: 1
        editable: root.editable
        from: Math.min(root.from, root.to) * self.factor
        to: Math.max(root.from, root.to) * self.factor
        locale: Qt.locale("en_US") // Чтобы использовалась точка для десятичных разрядов
        wheelEnabled: activeFocus

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
            contentItem.selectByMouse = true
        }

        background: Rectangle {
            radius: 8
            border.width: 1
            border.color: colors.black_20
        }

        down.indicator.visible: activeFocus
        up.indicator.visible: activeFocus
    }
    QtObject {
        id: self
        property int factor: Math.pow(10, root.decimals)
    }
}

