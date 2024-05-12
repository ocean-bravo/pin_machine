import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15

Item {
    width: 700
    height: 700

    component Btn: OpSolidButton {
        Layout.preferredWidth: 60
        Layout.preferredHeight: 60

        textcolor: "white"

        font.pixelSize: 24
        font.weight: Font.Medium
    }

    component CentralButton: OpFrameButton {
        Layout.preferredWidth: 60
        Layout.preferredHeight: 60

        enabled: false
        background.opacity: 1.0

        leftPadding: 0
        rightPadding: 0

        color: colors.primary_90
    }

    component Sp: Item {
        Layout.preferredWidth: 60
        Layout.preferredHeight: 60
    }

    component MyText: OpText {
        Layout.preferredWidth: 60
        Layout.preferredHeight: 60

        horizontalAlignment: Text.AlignHCenter

        font.weight: Font.Medium
        font.pixelSize: 28
        color: colors.black_100
    }

    RowLayout {
        anchors.fill: parent

        Hspacer {}
        Item {
            Layout.preferredWidth: 700
            Layout.fillHeight: true
            Grid {}
        }
        Hspacer {}
    }

    Rectangle {
        z: -1
        anchors.fill: parent
        visible: guiDebug
        color: "#22FFFF00"
    }

    component Grid: GridLayout {
        id: grid
        anchors.fill: parent
        columns: 11
        columnSpacing: 4
        rowSpacing: 4


        Sp {} Sp {} Sp {} Sp {} Sp {} MyText { text: "Y+" }                                                      Sp {} Sp {} Sp {} Sp {} Sp {}

        Sp {} Sp {} Sp {} Sp {} Sp {} Btn { text: qsTr("10");   bgcolor: colors.primary_90; onClicked: { jog("Y", 10,   xyFeedRate.value)} } Sp {} Sp {} Sp {} Sp {} Sp {}
        Sp {} Sp {} Sp {} Sp {} Sp {} Btn { text: qsTr("1");    bgcolor: colors.primary_80; onClicked: { jog("Y", 1,    xyFeedRate.value)} } Sp {} Sp {} Sp {} Sp {} Sp {}
        Sp {} Sp {} Sp {} Sp {} Sp {} Btn { text: qsTr("0.1");  bgcolor: colors.primary_60; onClicked: { jog("Y", 0.1,  xyFeedRate.value)} } Sp {} Sp {} Sp {} Sp {} Sp {}
        Sp {} Sp {} Sp {} Sp {} Sp {} Btn { text: qsTr("0.01"); bgcolor: colors.primary_50; onClicked: { jog("Y", 0.01, xyFeedRate.value)} } Sp {} Sp {} Sp {} Sp {} Sp {}


        MyText { text: "X-" }
        Btn { text: qsTr("10");   bgcolor: colors.primary_90; onClicked: { jog("X", -10,   xyFeedRate.value)} }
        Btn { text: qsTr("1");    bgcolor: colors.primary_80; onClicked: { jog("X", -1,    xyFeedRate.value)} }
        Btn { text: qsTr("0.1");  bgcolor: colors.primary_60; onClicked: { jog("X", -0.1,  xyFeedRate.value)} }
        Btn { text: qsTr("0.01"); bgcolor: colors.primary_50; onClicked: { jog("X", -0.01, xyFeedRate.value)} }

        CentralButton { text: "ШАГ"; }

        Btn { text: qsTr("0.01"); bgcolor: colors.primary_50; onClicked: { jog("X", 0.01, xyFeedRate.value)} }
        Btn { text: qsTr("0.1");  bgcolor: colors.primary_60; onClicked: { jog("X", 0.1,  xyFeedRate.value)} }
        Btn { text: qsTr("1");    bgcolor: colors.primary_80; onClicked: { jog("X", 1,    xyFeedRate.value)} }
        Btn { text: qsTr("10");   bgcolor: colors.primary_90; onClicked: { jog("X", 10,   xyFeedRate.value)} }
        MyText { text: "X+" }

        Sp {} Sp {} Sp {} Sp {} Sp {} Btn { text: qsTr("0.01"); bgcolor: colors.primary_50; onClicked: { jog("Y", -0.01, xyFeedRate.value)} } Sp {} Sp {} Sp {} Sp {} Sp {}
        Sp {} Sp {} Sp {} Sp {} Sp {} Btn { text: qsTr("0.1");  bgcolor: colors.primary_60; onClicked: { jog("Y", -0.1,  xyFeedRate.value)} } Sp {} Sp {} Sp {} Sp {} Sp {}
        Sp {} Sp {} Sp {} Sp {} Sp {} Btn { text: qsTr("1");    bgcolor: colors.primary_80; onClicked: { jog("Y", -1,    xyFeedRate.value)} } Sp {} Sp {} Sp {} Sp {} Sp {}
        Sp {} Sp {} Sp {} Sp {} Sp {} Btn { text: qsTr("10");   bgcolor: colors.primary_90; onClicked: { jog("Y", -10,   xyFeedRate.value)} } Sp {} Sp {} Sp {} Sp {} Sp {}

        Sp {} Sp {} Sp {} Sp {} Sp {} MyText { text: "Y-" }                                                       Sp {} Sp {} Sp {} Sp {} Sp {}
    }


    Shortcut {
        sequence: "Ctrl+Up"
        context: Qt.WindowShortcut
        onActivated: jog("Y", 1,  xyFeedRate.value)
    }
    Shortcut {
        sequence: "Ctrl+Down"
        context: Qt.WindowShortcut
        onActivated: jog("Y", -1,  xyFeedRate.value)
    }
    Shortcut {
        sequence: "Ctrl+Left"
        context: Qt.WindowShortcut
        onActivated: jog("X", -1,  xyFeedRate.value)
    }
    Shortcut {
        sequence: "Ctrl+Right"
        context: Qt.WindowShortcut
        onActivated: jog("X", 1,  xyFeedRate.value)
    }
    Shortcut {
        sequence: "Ctrl+Shift+Up"
        context: Qt.WindowShortcut
        onActivated: jog("Y", 10,  xyFeedRate.value)
    }
    Shortcut {
        sequence: "Ctrl+Shift+Down"
        context: Qt.WindowShortcut
        onActivated: jog("Y", -10,  xyFeedRate.value)
    }
    Shortcut {
        sequence: "Ctrl+Shift+Left"
        context: Qt.WindowShortcut
        onActivated: jog("X", -10,  xyFeedRate.value)
    }
    Shortcut {
        sequence: "Ctrl+Shift+Right"
        context: Qt.WindowShortcut
        onActivated: jog("X", 10,  xyFeedRate.value)
    }
}
