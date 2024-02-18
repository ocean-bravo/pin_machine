import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

CollapsiblePanel {
    id: root
    width: parent.width
    height: checked ? 95 : 30

    text: qsTr("Start Point")

    onCheckedChanged: {
        column.visible = checked
    }
    Component.onCompleted: {
        column.visible = checked
    }

    ColumnLayout {
        id: column
        width: parent.width

        Rectangle {
            color: "lightgrey"
            Layout.fillWidth: true
            Layout.preferredHeight: 30

            RowLayout {
                anchors.fill: parent

                Text {
                    text: qsTr("Start point")
                    verticalAlignment: Text.AlignVCenter
                    Layout.preferredWidth: 80
                }

                Text {
                    text: qsTr("x")
                    verticalAlignment: Text.AlignVCenter


                    Layout.preferredHeight: 30
                    Layout.preferredWidth: 20
                }
                DoubleSpinBox {
                    id: startPointX
                    decimals: 3
                    value: DataBus.punchpath_start_point.x
                    onValueModified: DataBus.punchpath_start_point = Qt.point(startPointX.value, startPointY.value)

                    Layout.preferredHeight: 30
                    Layout.preferredWidth: 100
                }
                Text {
                    text: qsTr("y")
                    verticalAlignment: Text.AlignVCenter

                    Layout.preferredHeight: 30
                    Layout.preferredWidth: 20

                }
                DoubleSpinBox {
                    id: startPointY
                    decimals: 3
                    value: DataBus.punchpath_start_point.y
                    onValueModified: DataBus.punchpath_start_point = Qt.point(startPointX.value, startPointY.value)

                    Layout.preferredHeight: 30
                    Layout.preferredWidth: 100
                }

                SaveButton {
                    acceptFunc: function() {
                        Settings.setValue("punchpath_start_point_x", startPointX.value)
                        Settings.setValue("punchpath_start_point_y", startPointY.value)
                    }
                }
            }
        }
    }
}

