import QtQml 2.12
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12

Button {
    id: root
    height: 30
    property int span: 1

    property string tooltipText

    ToolTip.delay: 1000
    ToolTip.timeout: 5000
    ToolTip.visible: hovered && tooltipText
    ToolTip.text: root.tooltipText

//    Layout.minimumWidth: 10

//    Layout.maximumWidth: 200

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
    //Layout.preferredHeight: 30
    //Layout.fillWidth: true

//    Component.onCompleted: {
//        if (parent.hasOwnProperty("columns"))
//        {

//        }
//    }
}
