import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15

GroupBox {
    id: root
    width: 100

    property alias checked: checkBox.checked

    property alias text: checkBox.text

    Behavior on height {
        NumberAnimation { duration: 100 }
    }

    label: CheckBox {
        id: checkBox
        checked: false
        indicator: Item {
            implicitWidth: 26
            implicitHeight: 26
            x: checkBox.leftPadding
            y: parent.height / 2 - height / 2

            Text {
                width: 14
                height: 14
                x: 6
                y: 6
                text: checkBox.checked ? "➖" : "➕"
                font.pixelSize: 25
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }
}
