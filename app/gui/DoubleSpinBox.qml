import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root

    property real from: -300
    property real to: 300
    property int decimals: 3
    property bool editable: true
    property real value: 0.0

    width: 100
    height: 25

    Layout.preferredWidth: 100
    Layout.preferredHeight: 30

    //onValueChanged: spinbox.value = root.value * self.factor

    signal valueModified

    property alias text: spinbox.displayText

    SpinBox {
        id: spinbox
        anchors.fill: parent
        value: root.value * self.factor
        stepSize: 1
        editable: root.editable
        from: Math.min(root.from, root.to) * self.factor
        to: Math.max(root.from, root.to) * self.factor
        locale: Qt.locale("en_US") // Чтобы использовалась точка для десятичных разрядов
        wheelEnabled: true

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
            // implicitWidth: 140
            color: Qt.lighter("grey")
            border.color: "transparent"

            radius: 5
        }

        // contentItem: TextInput {
        //          z: 2
        //          text: control.textFromValue(control.value, control.locale)

        //          font: control.font
        //          color: "#21be2b"
        //          selectionColor: "#21be2b"
        //          selectedTextColor: "#ffffff"
        //          horizontalAlignment: Qt.AlignHCenter
        //          verticalAlignment: Qt.AlignVCenter

        //          readOnly: !control.editable
        //          validator: control.validator
        //          inputMethodHints: Qt.ImhFormattedNumbersOnly
        //      }

    }
    QtObject {
        id: self
        property int factor: Math.pow(10, root.decimals)
    }
}

