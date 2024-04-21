import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Label {
    font.pixelSize: 16
    font.family: mainFont
    font.weight: Font.Medium
    leftPadding: 12

    verticalAlignment: Text.AlignVCenter

    background: Rectangle {
        radius: 8
        border.width: 1
        border.color: colors.black_20
    }
}
