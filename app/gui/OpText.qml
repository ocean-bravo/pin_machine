import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Text {
    font.family: mainFont
    font.pixelSize: 13
    font.weight: Font.Normal
    color: colors.black_50
    verticalAlignment: Text.AlignVCenter

    Rectangle {
        z: -1
        anchors.fill: parent
        visible: guiDebug
        color: "#4000FF00"
    }
}

