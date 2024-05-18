import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15


Control {
    id: root

    padding: 16

    background: OpWhitePanel {}

    property int xyFeedRate: xyFeedRateMax * xyFeedRateSlider.value / 100

    component VGap4 : Item { width: 10; DebugRect {} Layout.preferredHeight: 4}
    component VGap16 : Item { width: 10; DebugRect {} Layout.preferredHeight: 16}

    contentItem: ColumnLayout {
        spacing: 0

        OpHeader { text: qsTr("Управление координатами") }

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true

            component Sp: Item {
                Layout.preferredWidth: 60
                Layout.preferredHeight: 60
            }

            component Btn: OpSolidButton {
                Layout.preferredWidth: 60
                Layout.preferredHeight: 60
                icon.width: 40
                icon.height: 40
                icon.source: "images/arrow_40.svg"
                bgcolor: colors.primary_50
            }

            GridLayout {
                anchors.centerIn: parent
                columns: 3
                rows: 3
                rowSpacing: 4
                columnSpacing: 4

                Sp {}
                Btn {rotation: 90; onClicked: jog("Y", btnGroup.checkedButton.text, xyFeedRate)}
                Sp {}

                Btn { rotation: 0; onClicked: jog("X", -btnGroup.checkedButton.text, xyFeedRate) }
                Sp{}
                Btn{rotation: 180; onClicked: jog("X", btnGroup.checkedButton.text, xyFeedRate)}

                Sp {}
                Btn {rotation: 270; onClicked: jog("Y", -btnGroup.checkedButton.text, xyFeedRate)}
                Sp {}

            }
        }


        OpHeader { text: qsTr("Шаг") }
        VGap4 {}
        Item {
            Layout.preferredHeight: 40
            Layout.fillWidth: true

            RowLayout {
                anchors.fill: parent

                ButtonGroup { id: btnGroup }

                component StepButton : OpFrameButton {
                    Layout.fillWidth: true
                    Layout.preferredWidth: 1
                    color: colors.primary_50
                    checkable: true
                    ButtonGroup.group: btnGroup
                }

                StepButton { text: "0.1" }
                StepButton { text: "1"   }
                StepButton { text: "5";   checked: true }
                StepButton { text: "10"  }
                StepButton { text: "50"  }
            }

        }

        VGap16 {}

        OpHeader { text: qsTr("Скорость осей X/Y") }
        VGap4 {}

        OpSliderMinMax {
            id: xyFeedRateSlider

            Layout.preferredHeight: 36
            Layout.fillWidth: true
        }

    }




}
