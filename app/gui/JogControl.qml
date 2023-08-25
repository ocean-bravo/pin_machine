import QtQml 2.12
import QtQuick 2.15
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12

Rectangle {
    color: "azure"
    component MyButton: Button {
        width: {
            var wi = parent.width
            var s = parent.columnSpacing
            var c = parent.columns

            var singleCellWidth = (wi - (c-1)*s) / c
            return singleCellWidth
        }
        height: width
    }

    component Spacer: Item {
        width: {
            var w = parent.width
            var s = parent.columnSpacing
            var c = parent.columns

            var singleCellWidth = (w - (c-1)*s) / c
            return singleCellWidth
        }
        height: width
    }

    Grid {
        anchors.fill: parent

        columns: 9
        columnSpacing: 5
        rowSpacing: 5

        Spacer {} Spacer {} Spacer {} MyButton { text: qsTr("+50");  onClicked: { write("$H\n" )} } Spacer {} Spacer {} Spacer {} Spacer {} MyButton { text: qsTr("+1");  onClicked: { write("$H\n" )} }
        Spacer {} Spacer {} Spacer {} MyButton { text: qsTr("+10");  onClicked: { write("$H\n" )} } Spacer {} Spacer {} Spacer {} Spacer {} MyButton { text: qsTr("+0.1");  onClicked: { write("$H\n" )} }
        Spacer {} Spacer {} Spacer {} MyButton { text: qsTr("+1");   onClicked: { write("$H\n" )} } Spacer {} Spacer {} Spacer {} Spacer {} MyButton { text: qsTr("+0.01");  onClicked: { write("$H\n" )} }


        MyButton { text: qsTr("-50");   onClicked: { write("$H\n" )} }
        MyButton { text: qsTr("-10");   onClicked: { write("$H\n" )} }
        MyButton { text: qsTr("-1");    onClicked: { write("$H\n" )} }

        MyButton { text: "X/Y"; }

        MyButton { text: qsTr("+1");   onClicked: { write("$H\n" )} }
        MyButton { text: qsTr("+10");   onClicked: { write("$H\n" )} }
        MyButton { text: qsTr("+50");    onClicked: { write("$H\n" )} }

        Spacer {}
        MyButton { text: "Z"; }


        Spacer {} Spacer {} Spacer {} MyButton { text: qsTr("-1");  onClicked: { write("$H\n" )} } Spacer {} Spacer {} Spacer {} Spacer {} MyButton { text: qsTr("-0.01");  onClicked: { write("$H\n" )} }
        Spacer {} Spacer {} Spacer {} MyButton { text: qsTr("-10");  onClicked: { write("$H\n" )} } Spacer {} Spacer {} Spacer {} Spacer {} MyButton { text: qsTr("-0.1");  onClicked: { write("$H\n" )} }
        Spacer {} Spacer {} Spacer {} MyButton { text: qsTr("-50");   onClicked: { write("$H\n" )} } Spacer {} Spacer {} Spacer {} Spacer {} MyButton { text: qsTr("-1");  onClicked: { write("$H\n" )} }
    }
}
