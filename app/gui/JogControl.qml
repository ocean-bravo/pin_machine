import QtQml 2.12
import QtQuick 2.15
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12

Rectangle {
    color: "azure"

    height: width * 7 / 9
    //implicitHeight: grid.implicitHeight
    //implicitWidth: grid.implicitWidth

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

    function jog(axis, mm) {
        let feed = 1000
        write("$J=G91 " + axis + mm + " F" + feed + "\n")
    }

    Grid {
        id: grid
        anchors.fill: parent

        columns: 9
        columnSpacing: 5
        rowSpacing: 5

        Spacer {} Spacer {} Spacer {} MyButton { text: qsTr("+50");  onClicked: { jog("Y", 50)} } Spacer {} Spacer {} Spacer {} Spacer {} MyButton { text: qsTr("+1");  onClicked: { jog("Z", 1)} }
        Spacer {} Spacer {} Spacer {} MyButton { text: qsTr("+10");  onClicked: { jog("Y", 10)} } Spacer {} Spacer {} Spacer {} Spacer {} MyButton { text: qsTr("+0.1");  onClicked: { jog("Z", 0.1)} }
        Spacer {} Spacer {} Spacer {} MyButton { text: qsTr("+1");   onClicked: { jog("Y", 1)} } Spacer {} Spacer {} Spacer {} Spacer {} MyButton { text: qsTr("+0.01");  onClicked: { jog("Z", 0.01)} }


        MyButton { text: qsTr("-50"); onClicked: { jog("X", -50)} }
        MyButton { text: qsTr("-10"); onClicked: { jog("X", -10)} }
        MyButton { text: qsTr("-1");  onClicked: { jog("X", -1)} }

        MyButton { text: "X/Y"; }

        MyButton { text: qsTr("+1");  onClicked: { jog("X", 1)} }
        MyButton { text: qsTr("+10"); onClicked: { jog("X", 10)} }
        MyButton { text: qsTr("+50"); onClicked: { jog("X", 50)} }

        Spacer {}
        MyButton { text: "Z"; }


        Spacer {} Spacer {} Spacer {} MyButton { text: qsTr("-1");  onClicked: { jog("Y", -1)} } Spacer {} Spacer {} Spacer {} Spacer {} MyButton { text: qsTr("-0.01");  onClicked: { jog("Z", -0.01)} }
        Spacer {} Spacer {} Spacer {} MyButton { text: qsTr("-10");  onClicked: { jog("Y", -10)} } Spacer {} Spacer {} Spacer {} Spacer {} MyButton { text: qsTr("-0.1");  onClicked: { jog("Z", -0.1)} }
        Spacer {} Spacer {} Spacer {} MyButton { text: qsTr("-50");   onClicked: { jog("Y", -50)} } Spacer {} Spacer {} Spacer {} Spacer {} MyButton { text: qsTr("-1");  onClicked: { jog("Z", -1)} }
    }
}
