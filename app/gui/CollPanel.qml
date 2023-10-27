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
    }
}
