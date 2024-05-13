import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15

Item {
    id: root

    property int zwFeedRate

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
        color: colors.primary_70
    }

    DebugRect { color: "#22FF0FF0" }

    // RowLayout {
    //     anchors.fill: parent

    //     Item {
    //         Layout.preferredWidth: 140
    //         Layout.preferredHeight: 592
    //         Grid {}
    //     }
    // }

    GridLayout {
        id: grid
        anchors.fill: parent
        rows: 9
        columnSpacing: 40
        rowSpacing: 4

        flow: GridLayout.TopToBottom

        MyText { text: "Z+" }
        Btn { text: qsTr("1");    bgcolor: colors.primary_80; onClicked: { jog("Z", 1,    zwFeedRate)} }
        Btn { text: qsTr("0.1");  bgcolor: colors.primary_60; onClicked: { jog("Z", 0.1,  zwFeedRate)} }
        Btn { text: qsTr("0.01"); bgcolor: colors.primary_50; onClicked: { jog("Z", 0.01, zwFeedRate)} }
        CentralButton { text: "ШАГ"; }
        Btn { text: qsTr("0.01"); bgcolor: colors.primary_50; onClicked: { jog("Z", -0.01, zwFeedRate)} }
        Btn { text: qsTr("0.1");  bgcolor: colors.primary_60; onClicked: { jog("Z", -0.1,  zwFeedRate)} }
        Btn { text: qsTr("1");    bgcolor: colors.primary_80; onClicked: { jog("Z", -1,    zwFeedRate)} }
        MyText { text: "Z-" }

        MyText { text: "W+" }
        Btn { text: qsTr("1");    bgcolor: colors.primary_80; onClicked: { jog("W", 1,    zwFeedRate)} }
        Btn { text: qsTr("0.1");  bgcolor: colors.primary_60; onClicked: { jog("W", 0.1,  zwFeedRate)} }
        Btn { text: qsTr("0.01"); bgcolor: colors.primary_50; onClicked: { jog("W", 0.01, zwFeedRate)} }
        CentralButton { text: "ШАГ"; }
        Btn { text: qsTr("0.01"); bgcolor: colors.primary_50; onClicked: { jog("W", -0.01, zwFeedRate)} }
        Btn { text: qsTr("0.1");  bgcolor: colors.primary_60; onClicked: { jog("W", -0.1,  zwFeedRate)} }
        Btn { text: qsTr("1");    bgcolor: colors.primary_80; onClicked: { jog("W", -1,    zwFeedRate)} }
        MyText { text: "W-" }
    }
}
