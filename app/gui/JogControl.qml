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

        Spacer {} Spacer {} Spacer {} MyButton { text: qsTr("+50");  onClicked: { write("$J=G91 Y50 F100\n")} } Spacer {} Spacer {} Spacer {} Spacer {} MyButton { text: qsTr("+1");  onClicked: { write("$J=G91 Z1 F100\n")} }
        Spacer {} Spacer {} Spacer {} MyButton { text: qsTr("+10");  onClicked: { write("$J=G91 Y10 F100\n")} } Spacer {} Spacer {} Spacer {} Spacer {} MyButton { text: qsTr("+0.1");  onClicked: { write("$J=G91 Z0.1 F100\n")} }
        Spacer {} Spacer {} Spacer {} MyButton { text: qsTr("+1");   onClicked: { write("$J=G91 Y1 F100\n")} } Spacer {} Spacer {} Spacer {} Spacer {} MyButton { text: qsTr("+0.01");  onClicked: { write("$J=G91 Z0.01 F100\n")} }


        MyButton { text: qsTr("-50");   onClicked: { write("$J=G91 X-50 F100\n")} }
        MyButton { text: qsTr("-10");   onClicked: { write("$J=G91 X-10 F100\n")} }
        MyButton { text: qsTr("-1");    onClicked: { write("$J=G91 X-1 F100\n")} }

        MyButton { text: "X/Y"; }

        MyButton { text: qsTr("+1");   onClicked: { write("$J=G91 X1 F100\n")} }
        MyButton { text: qsTr("+10");   onClicked: { write("$J=G91 X10 F100\n")} }
        MyButton { text: qsTr("+50");    onClicked: { write("$J=G91 X50 F100\n")} }

        Spacer {}
        MyButton { text: "Z"; }


        Spacer {} Spacer {} Spacer {} MyButton { text: qsTr("-1");  onClicked: { write("$J=G91 Y-1 F100\n")} } Spacer {} Spacer {} Spacer {} Spacer {} MyButton { text: qsTr("-0.01");  onClicked: { write("$J=G91 Z-0.01 F100\n")} }
        Spacer {} Spacer {} Spacer {} MyButton { text: qsTr("-10");  onClicked: { write("$J=G91 Y-10 F100\n")} } Spacer {} Spacer {} Spacer {} Spacer {} MyButton { text: qsTr("-0.1");  onClicked: { write("$J=G91 Z-0.1 F100\n")} }
        Spacer {} Spacer {} Spacer {} MyButton { text: qsTr("-50");   onClicked: { write("$J=G91 Y-50 F100\n")} } Spacer {} Spacer {} Spacer {} Spacer {} MyButton { text: qsTr("-1");  onClicked: { write("$J=G91 Z-1 F100\n")} }
    }
}
