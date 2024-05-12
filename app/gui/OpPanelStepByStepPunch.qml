import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15


Control {
    id: root

    padding: 16

    background: OpWhitePanel {}

    property bool fill: true

    property bool st: true

    // Timer {
    //     running: true
    //     onTriggered: {
    //         st = !st

    //         if (st)
    //             stopFrame.text = qsTr("ПускПускПускПуск")
    //         else
    //             stopFrame.text = qsTr("Пуск")
    //     }
    //     interval: 2000

    //     repeat: true
    // }

    component MyHeader : OpText {
        font.capitalization: Font.AllUppercase
        font.weight: Font.Medium

        Layout.preferredHeight: 18
        Layout.fillWidth: true
    }

    contentItem: GridLayout {
        columns: 3
        rowSpacing: 10
        columnSpacing: 16

        MyHeader {
            text: qsTr("Пошаговый цикл установки контакта")
            GridLayout.columnSpan: 3

            Layout.fillWidth: true
        }

        OpFrameButton {
            text: qsTr("Шаг")
            color: colors.primary_90

            icon.source: "images/next_20.svg"

            Layout.fillWidth: true
            Layout.preferredWidth: implicitWidth
        }

        OpFrameButton {
            text: qsTr("Стоп")
            color: colors.error_80

            icon.source: "images/stop_20.svg"

            Layout.fillWidth: true
            Layout.preferredWidth: implicitWidth
        }

        OpText {
            text: "Парковка Z и W"

            Layout.fillWidth: true
            Layout.preferredWidth: implicitWidth * 3 // 3 - фактически коэффициент расширения

            Layout.preferredHeight: 40
        }

        Item {
            GridLayout.columnSpan: 3

            Layout.fillWidth: fill
            Layout.preferredHeight: 40

            RowLayout {
                anchors.fill: parent

                OpText {
                    text: qsTr("Автоустановка N:")
                    font.capitalization: Font.AllUppercase
                    font.weight: Font.Medium

                    Layout.fillHeight: true
                }

                OpDoubleSpinbox {
                    decimals: 0
                    from: 1
                    to: 100
                    Layout.preferredWidth: 60
                    Layout.fillHeight: true
                }

                OpText {
                    text: qsTr("контактов")
                    font.capitalization: Font.AllUppercase
                    font.weight: Font.Medium

                    Layout.fillHeight: true
                }

                Hspacer {}

            }
        }

        OpFrameButton {
            id: stopFrame
            text: qsTr("Пуск")
            color: colors.primary_90

            icon.source: "images/play_20.svg"

            Layout.fillWidth: true
            Layout.preferredWidth: implicitWidth
        }

        OpFrameButton {
            text: qsTr("Стоп")
            color: colors.error_80
            icon.source: "images/stop_20.svg"

            Layout.fillWidth: true
            Layout.preferredWidth: implicitWidth
        }
    }
}
