import QtQml 2.12
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12


Rectangle {
    id: root
    border.width: 1
    border.color: "grey"
    radius: 5
    width: 120
    height: 30
    property alias text: sendText.text

    TextInput {
        id: sendText
        text: ""
        anchors.fill: parent
        anchors.margins: 4
    }
}
