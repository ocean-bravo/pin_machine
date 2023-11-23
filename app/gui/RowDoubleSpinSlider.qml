import QtQml 2.12
import QtQuick 2.15
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12

Item {
    id: root
    height: 25
    width: parent.width

    property alias text: text.text

    property alias from: spinbox.from
    property alias to: spinbox.to
    property alias stepSize: spinbox.stepSize

    property var value

    property var valueBind


    Row {
        anchors.fill: parent
        spacing: 0
        Text {
            id: text
            width: 70
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }

        DoubleSpinBox {
            id: spinbox
            height: root.height
            width: 50
            value: root.value
            decimals: 1
            property real stepSize

            onValueModified: {
                root.value = value
                spinbox.value = Qt.binding(root.valueBind)
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

            width: root.width - text.width - spinbox.width
            height: root.height

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
