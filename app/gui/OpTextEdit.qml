import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15


Rectangle {
    id: root
    border.width: 1
    border.color: colors.primary_60
    radius: 8

    property alias text: sendText.text

    Layout.preferredHeight: 30
    Layout.preferredWidth: 100

    DebugRect {
        anchors.fill: sendText
    }

    TextInput {
        id: sendText
        anchors.fill: parent
        anchors.margins: 4
        selectByMouse: true
        verticalAlignment: TextInput.AlignVCenter

        font.weight: Font.Medium
        font.pixelSize: 24
        font.family: "DINPro"

        color: colors.primary_60
    }
}
