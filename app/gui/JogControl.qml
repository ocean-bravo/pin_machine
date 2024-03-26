import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15

Rectangle {
    color: "transparent"

    width: 200 //parent.width
    height: 400

    component MyButton: Button {
        // width: {
        //     var wi = parent.width
        //     var s = parent.columnSpacing
        //     var c = parent.columns

        //     var singleCellWidth = (wi - (c-1)*s) / c
        //     return singleCellWidth
        // }
        padding: 0
        Layout.preferredWidth: 36
        //height: width
        Layout.preferredHeight: 35
    }

    component MyButtonZ: Button {
        // width: {
        //     var wi = parent.width
        //     var s = parent.columnSpacing
        //     var c = parent.columns

        //     var singleCellWidth = (wi - (c-1)*s) / c
        //     return singleCellWidth
        // }
        Layout.preferredWidth: 45
        Layout.preferredHeight: 35
    }

    component Sp: Item {
        Layout.preferredWidth: 35
        Layout.preferredHeight: 35
    }

    component Nsp: Sp { // узкий спейсер
        Layout.preferredWidth: 20
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            color: 'transparent'
            Layout.fillWidth: true;
            Layout.preferredHeight: 30;

            Slider { id: xyFeedRate; from: 10; to: 5000; value: 5000; stepSize: 10;
                anchors.fill: parent
                Text {
                    anchors.centerIn: parent
                    text: "XY feed rate: " + parent.value
                    bottomPadding: 20
                }
            }
        }

        Rectangle {
            color: 'transparent'
            Layout.fillWidth: true;
            Layout.preferredHeight: 30;

            Slider { id: zFeedRate; from: 10; to: 5000; value: 5000; stepSize: 10;
                anchors.fill: parent
                Text {
                    anchors.centerIn: parent
                    text: "Z feed rate: " + parent.value
                    bottomPadding: 20
                }
            }
        }

        GridLayout {
            id: grid
            //anchors.fill: parent
            Layout.fillWidth: true
            //Layout.preferredHeight: 50

            columns: 11
            columnSpacing: 1
            rowSpacing: 1

            Sp {} Sp {} Sp {} Sp {} MyButton { text: qsTr("+10");   onClicked: { jog("Y", 10,   xyFeedRate.value)} } Sp {} Sp {} Sp {} Sp {} Nsp {} Sp {}
            Sp {} Sp {} Sp {} Sp {} MyButton { text: qsTr("+1");    onClicked: { jog("Y", 1,    xyFeedRate.value)} } Sp {} Sp {} Sp {} Sp {} Nsp {} MyButtonZ { text: qsTr("+1");    onClicked: { jog("Z", 1,    zFeedRate.value)} }
            Sp {} Sp {} Sp {} Sp {} MyButton { text: qsTr("+0.1");  onClicked: { jog("Y", 0.1,  xyFeedRate.value)} } Sp {} Sp {} Sp {} Sp {} Nsp {} MyButtonZ { text: qsTr("+0.1");  onClicked: { jog("Z", 0.1,  zFeedRate.value)} }
            Sp {} Sp {} Sp {} Sp {} MyButton { text: qsTr("+0.01"); onClicked: { jog("Y", 0.01, xyFeedRate.value)} } Sp {} Sp {} Sp {} Sp {} Nsp {} MyButtonZ { text: qsTr("+0.01"); onClicked: { jog("Z", 0.01, zFeedRate.value)} }


            MyButton { text: qsTr("-10");   onClicked: { jog("X", -10,   xyFeedRate.value)} }
            MyButton { text: qsTr("-1");    onClicked: { jog("X", -1,    xyFeedRate.value)} }
            MyButton { text: qsTr("-0.1");  onClicked: { jog("X", -0.1,  xyFeedRate.value)} }
            MyButton { text: qsTr("-0.01"); onClicked: { jog("X", -0.01, xyFeedRate.value)} }

            MyButton { text: "X/Y"; }

            MyButton { text: qsTr("+0.01"); onClicked: { jog("X", 0.01, xyFeedRate.value)} }
            MyButton { text: qsTr("+0.1");  onClicked: { jog("X", 0.1,  xyFeedRate.value)} }
            MyButton { text: qsTr("+1");    onClicked: { jog("X", 1,    xyFeedRate.value)} }
            MyButton { text: qsTr("+10");   onClicked: { jog("X", 10,   xyFeedRate.value)} }

            Nsp {}
            MyButtonZ { text: "Z"; }

            Sp {} Sp {} Sp {} Sp {} MyButton { text: qsTr("-0.01"); onClicked: { jog("Y", -0.01, xyFeedRate.value)} } Sp {} Sp {} Sp {} Sp {} Nsp {} MyButtonZ { text: qsTr("-0.01"); onClicked: { jog("Z", -0.01, zFeedRate.value)} }
            Sp {} Sp {} Sp {} Sp {} MyButton { text: qsTr("-0.1");  onClicked: { jog("Y", -0.1,  xyFeedRate.value)} } Sp {} Sp {} Sp {} Sp {} Nsp {} MyButtonZ { text: qsTr("-0.1");  onClicked: { jog("Z", -0.1,  zFeedRate.value)} }
            Sp {} Sp {} Sp {} Sp {} MyButton { text: qsTr("-1");    onClicked: { jog("Y", -1,    xyFeedRate.value)} } Sp {} Sp {} Sp {} Sp {} Nsp {} MyButtonZ { text: qsTr("-1");    onClicked: { jog("Z", -1,    zFeedRate.value)} }
            Sp {} Sp {} Sp {} Sp {} MyButton { text: qsTr("-10");   onClicked: { jog("Y", -10,   xyFeedRate.value)} } Sp {} Sp {} Sp {} Sp {} Nsp {} Sp {}
        }


    }

    Shortcut {
        sequence: "Ctrl+Up"
        context: Qt.ApplicationShortcut
        onActivated: jog("Y", 1,  xyFeedRate.value)
    }
    Shortcut {
        sequence: "Ctrl+Down"
        context: Qt.ApplicationShortcut
        onActivated: jog("Y", -1,  xyFeedRate.value)
    }
    Shortcut {
        sequence: "Ctrl+Left"
        context: Qt.ApplicationShortcut
        onActivated: jog("X", -1,  xyFeedRate.value)
    }
    Shortcut {
        sequence: "Ctrl+Right"
        context: Qt.ApplicationShortcut
        onActivated: jog("X", 1,  xyFeedRate.value)
    }
    Shortcut {
        sequence: "Ctrl+Shift+Up"
        context: Qt.ApplicationShortcut
        onActivated: jog("Y", 10,  xyFeedRate.value)
    }
    Shortcut {
        sequence: "Ctrl+Shift+Down"
        context: Qt.ApplicationShortcut
        onActivated: jog("Y", -10,  xyFeedRate.value)
    }
    Shortcut {
        sequence: "Ctrl+Shift+Left"
        context: Qt.ApplicationShortcut
        onActivated: jog("X", -10,  xyFeedRate.value)
    }
    Shortcut {
        sequence: "Ctrl+Shift+Right"
        context: Qt.ApplicationShortcut
        onActivated: jog("X", 10,  xyFeedRate.value)
    }
}
