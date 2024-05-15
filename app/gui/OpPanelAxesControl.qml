import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15


Control {
    id: root

    padding: 16

    background: OpWhitePanel {}

    component VGap4 : Item { width: 10; DebugRect {} Layout.preferredHeight: 4}
    component VGap16 : Item { width: 10; DebugRect {} Layout.preferredHeight: 16}

    contentItem: ColumnLayout {
        spacing: 16

        OpHeader { text: qsTr("Управление координатами") }
        Vspacer {}
        OpHeader { text: qsTr("Скорость осей X/Y") }
        VGap4 {}

        OpSliderMinMax {
            Layout.preferredHeight: 36
            Layout.fillWidth: true

            id: xyFeedRateSlider
            from: 10
            value: 1000
            to: 5000
            stepSize: 10

        }

    }




}
