import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15

import QtQuick 2.12
import QtQuick.Controls 2.12

ApplicationWindow {
    id: root
    visible: true
    width: 1280
    height: 720

    Repeater {
        model: 10000

        Rectangle {
            id: rectangle
            width: Math.random()*20+20
            height: Math.random()*20+20
            color: Qt.hsva(Math.random(), 1.0, 1.0)
            Component.onCompleted: {
                x = Math.random()*root.width
                y = Math.random()*root.height
            }
            DragHandler {}

            RotationAnimator {
                running: true
                target: rectangle
                from: 0
                to: 360
                duration: 2000
                loops: Animator.Infinite
            }
        }
    }
}
