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
    //width: 120
    height: 30
    property int span: 1

    property alias text: sendText.text

    Layout.preferredHeight: 30
    Layout.preferredWidth: 100

    TextInput {
        id: sendText
        text: ""
        anchors.fill: parent
        anchors.margins: 4
        selectByMouse: true
    }
//    Layout.preferredWidth: 120
//    Layout.preferredHeight: 30

    width: {
        if (parent.hasOwnProperty("columns")) {
            let w = parent.width
            let s = parent.columnSpacing
            let c = parent.columns

            //console.log(w, s, c)

            let singleCellWidth = (w - (c-1)*s) / c
            if (span > 1)
                return singleCellWidth*span + (span-1)*s

            return singleCellWidth
        }
        else {
            return 100
        }
    }
}
