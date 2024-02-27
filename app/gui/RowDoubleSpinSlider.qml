import QtQml 2.12
import QtQuick 2.15
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12

Rectangle {
    id: root
    Layout.preferredHeight: 25
    Layout.fillWidth: true


    //color: "#40FF0000"
    color: "transparent"

    property alias text: text.text

    property alias from: spinbox.from
    property alias to: spinbox.to
    property alias stepSize: spinbox.stepSize

    property var value
    property var valueBind

    RowLayout {
        anchors.fill: parent
        spacing: 5

        DoubleSpinBox {
            id: spinbox
            Layout.preferredWidth: 50
            Layout.preferredHeight: 25
            value: root.value
            decimals: 1
            property real stepSize

            onValueModified: {
                root.value = value
                spinbox.value = Qt.binding(root.valueBind)
            }
        }

        ColumnLayout {
            spacing: 0

            Text {
                id: text
                Layout.preferredWidth: 70
                Layout.preferredHeight: 10
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignLeft
            }

            Slider {
                id: slider
                Layout.fillWidth: true
                Layout.preferredHeight: 10

                from: root.from
                to: root.to
                stepSize: root.stepSize
                value: root.value

                onMoved: {
                    root.value = value
                    root.value = Qt.binding(root.valueBind)
                }
            }
        }
    }
}
