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

        OpHeader {
            text: qsTr("Управление координатами")
        }


        Vspacer {}
        OpHeader { text: qsTr("Скорость осей X/Y") }

        VGap4 {}

        Item {
            Layout.preferredHeight: 36
            Layout.fillWidth: true

            RowLayout {
                anchors.fill: parent

                OpText {
                    text: "0"
                    color: colors.black_80
                    font.pixelSize: 16
                    font.weight: Font.Medium
                }

                OpSlider {
                    id: xyFeedRateSlider
                    from: 10
                    value: 1000
                    to: 5000
                    stepSize: 10

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



    }

}
