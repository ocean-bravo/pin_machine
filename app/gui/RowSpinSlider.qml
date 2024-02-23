import QtQml 2.12
import QtQuick 2.15
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12

Item {
    id: root
    //height: 25
    Layout.preferredHeight: 25
    //width: parent.width
    Layout.fillWidth: true

    property alias text: text.text

    property alias from: spinbox.from
    property alias to: spinbox.to
    property alias stepSize: spinbox.stepSize

    property var value

    property var valueBind


    RowLayout {
        anchors.fill: parent
        spacing: 0

        Text {
            id: text
            Layout.preferredWidth: 70
            Layout.preferredHeight: 25

            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }

        SpinBox {
            id: spinbox
            //height: root.height
            Layout.preferredWidth: 50
            Layout.preferredHeight: 25
            value: root.value
            editable: true

            onValueModified: {
                root.value = value
                root.value = Qt.binding(root.valueBind)
            }

//            MouseArea {
//                anchors.fill: parent
//                onWheel: {
//                    if (Math.sign(wheel.angleDelta.y) > 0) {
//                        spinbox.increase()
//                        root.value = spinbox.value
//                        root.value = Qt.binding(root.valueBind)
//                    }

//                    if (Math.sign(wheel.angleDelta.y) < 0) {
//                        spinbox.decrease()
//                        root.value = value
//                        root.value = Qt.binding(root.valueBind)
//                    }
//                }
//            }
        }

        Slider {
            id: slider

            Layout.preferredWidth: root.width - text.width - spinbox.width
            //height: root.height
            Layout.preferredHeight: 25

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
