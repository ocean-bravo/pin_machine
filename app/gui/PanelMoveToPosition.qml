import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

CollapsiblePanel {
    id: root
    //width: parent.width
    width: parent.width
    height: checked ? 320 : 30

    Layout.preferredWidth: 400
    Layout.preferredHeight: checked ? 320 : 30

    text: qsTr("Move to position")

    onCheckedChanged: {
        grid.visible = checked
    }
    Component.onCompleted: {
        grid.visible = checked
    }


    GridLayout {
        id: grid
        //width: // parent.width
        anchors.fill: parent
        columns: 4
        columnSpacing: 5
        rowSpacing: 5

        // 1.
        Slider { id: xyFeedRate; from: 10; to: 5000; value: 5000; stepSize: 10; Layout.fillWidth: true; Layout.columnSpan: 4;
            Layout.preferredHeight: 20
            Text {
                anchors.centerIn: parent
                text: "XY feed rate: " + parent.value
                bottomPadding: 20
            }
        }

        // 2.
        DoubleSpinBox { id: moveX; }
        SmButton { text: qsTr("Move X");
            onClicked: {
                write("G1 " + (relAbsX.text === "Abs" ? "G90" : "G91") + " F" + xyFeedRate.value + " X" + moveX.text) }
        }
        SmButton { id: relAbsX; checkable: true; text: checked ? "Rel" : "Abs"; Layout.preferredWidth: 40 }
        Item {Layout.fillWidth: true; Layout.preferredHeight: 30}

        // 3.
        DoubleSpinBox { id: moveY; }
        SmButton { text: qsTr("Move Y");
            onClicked: {
                write("G1 " + (relAbsY.text === "Abs" ? "G90" : "G91") + " F" + xyFeedRate.value + " Y" + moveY.text) }
        }
        SmButton { id: relAbsY; checkable: true; text: checked ? "Rel" : "Abs"; Layout.preferredWidth: 40 }
        //Item {Layout.fillWidth: true; Layout.preferredHeight: 30}

        // 4.
        Item {Layout.fillWidth: true; Layout.preferredHeight: 10; Layout.columnSpan: 4}


        // 5.
        Slider { id: zFeedRate; from: 10; to: 5000; value: 4000; stepSize: 10; Layout.fillWidth: true; Layout.columnSpan: 4
            Text {
                anchors.centerIn: parent
                text: "Z feed rate: " + parent.value
                bottomPadding: 20
            }
        }

        // 6.
        DoubleSpinBox { id: moveZ; }
        SmButton { text: qsTr("Move Z");
            onClicked: {
                write("G1 " + (relAbsZ.text === "Abs" ? "G90" : "G91") + " F" + zFeedRate.value + " Z" + moveZ.text) }
        }
        SmButton { id: relAbsZ; checkable: true; text: checked ? "Rel" : "Abs"; Layout.preferredWidth: 40 }
        Item {Layout.fillWidth: true; Layout.preferredHeight: 10}

        // 7.
        DoubleSpinBox { id: cutZ; from: -50; to: 50; value: Settings.value("cut_z", 0.0)}
        SmButton { text: "Cut Z";
            onClicked: {
                write("G1 G90 F" + zFeedRate.value + " Z" + cutZ.text) }
        }
        SaveButton {
            acceptFunc: function() {
                Settings.setValue("cut_z", cutZ.value)
            }
        }
        Item {Layout.fillWidth: true; Layout.preferredHeight: 10}

        // 8.
        DoubleSpinBox { id: insertZ; from: -50; to: 50; value: Settings.value("insert_z", 0.0) }
        SmButton { text: "Insert Z";
            onClicked: {
                write("G1 G90 F" + zFeedRate.value + " Z" + insertZ.text) }}
        SaveButton {
            acceptFunc: function() {
                Settings.setValue("insert_z", insertZ.value)
            }
        }
        Item {Layout.fillWidth: true; Layout.preferredHeight: 10}

        // 9.
        SmTextEdit { id: sendText;  Layout.columnSpan: 2; Layout.fillWidth: true}
        SmButton { text: qsTr("Send");       onClicked: { write(sendText.text) } }
        Item {Layout.fillWidth: true; Layout.preferredHeight: 10; }

    }
}


