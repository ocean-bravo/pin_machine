import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import QtGraphicalEffects 1.15

Slider {
    id: root
    value: 0.5

    background: Rectangle {
        id: rightOfKnob
        x: root.leftPadding
        y: root.topPadding + root.availableHeight / 2 - height / 2
        implicitWidth: 200
        implicitHeight: 4
        width: root.availableWidth
        height: implicitHeight
        radius: 2
        color: colors.black_50


        Rectangle {
            id: leftOfKnob
            width: root.visualPosition * parent.width
            height: parent.height
            color: colors.primary_70
            radius: 2
        }
    }

    handle: Rectangle {
        id: knob
        x: root.leftPadding + root.visualPosition * (root.availableWidth - width)
        y: root.topPadding + root.availableHeight / 2 - height / 2
        implicitWidth: root.height
        implicitHeight: root.height
        radius: root.height / 2
        color: root.pressed ? "lightgrey" : "white"
        border.color: colors.primary_20
        border.width: 2
    }

    Rectangle {
        visible: guiDebug
        anchors.fill: parent
        color: "#4000FF00"
    }
}

