import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import QtGraphicalEffects 1.15


Item {

    property alias from: slider.from
    property alias value: slider.value
    property alias to: slider.to
    property alias stepSize: slider.stepSize

    Layout.preferredHeight: 36
    Layout.fillWidth: true

    RowLayout {
        anchors.fill: parent
        spacing: 8

        OpText {
            text: "0"
            color: colors.black_80
            font.pixelSize: 16
            font.weight: Font.Medium
        }

        OpSlider {
            id: slider

            from: 1
            value: 25
            to: 100
            stepSize: 1

            Layout.fillWidth: true
        }

        OpText {
            text: "100%"
            color: colors.black_80
            font.pixelSize: 16
            font.weight: Font.Medium
        }
    }
}

