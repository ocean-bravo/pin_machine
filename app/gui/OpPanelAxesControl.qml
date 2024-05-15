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


        Item {
            Layout.preferredHeight: 40
            Layout.fillWidth: true

            RowLayout {
                anchors.fill: parent

                ButtonGroup { id: btnGroup }

                component StepButton : OpFrameButton {
                    Layout.fillWidth: true
                    Layout.preferredWidth: 60
                    color: colors.primary_50
                    checkable: true
                    ButtonGroup.group: btnGroup
                }

                OpText {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Layout.preferredWidth: 30
                    text: qsTr("ШАГ")
                }

                StepButton {
                    text: "0.1"
                }
                StepButton {
                    text: "1"
                }
                StepButton {
                    text: "5"
                    checked: true
                }
                StepButton {
                    text: "10"
                }
                StepButton {
                    text: "50"
                }

            }

        }

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
