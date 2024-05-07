import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    Layout.fillWidth: true
    height: 10

    Rectangle {
        anchors.fill: parent
        visible: guiDebug
        color: "#400000FF"
    }
}
