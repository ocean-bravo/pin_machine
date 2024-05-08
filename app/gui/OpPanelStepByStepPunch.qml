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

    Timer {
        running: true
        onTriggered: {
            st = !st

            if (st)
                stopFrame.text = qsTr("ПускПускПускПуск")
            else
                stopFrame.text = qsTr("Пуск")
        }
        interval: 2000

        repeat: true
    }

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

            Layout.fillWidth: fill
        }

        OpFrameButton {
            text: qsTr("Шаг")
            color: colors.primary_90

            icon.source: "images/next_20.svg"

            font.pixelSize: 14

            Layout.fillWidth: fill
            Layout.preferredWidth: implicitWidth

            Layout.preferredHeight: 40
        }

        OpFrameButton {
            text: qsTr("СтопCтопСтопСтоп")
            color: colors.error_80
            font.pixelSize: 14

            icon.source: "images/stop_20.svg"

            Layout.fillWidth: fill
            Layout.preferredWidth: implicitWidth

            Layout.preferredHeight: 40
        }

        OpText {
            text: "Парковка Z и W"

            Layout.fillWidth: fill
            Layout.preferredWidth: implicitWidth*2

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
                    Layout.preferredWidth: 50
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
            text: qsTr("ПускПускПускПуск")
            color: colors.primary_90
            font.pixelSize: 14

            icon.source: "images/play_20.svg"

            Layout.fillWidth: fill
            Layout.preferredWidth: implicitWidth

            Layout.preferredHeight: 40
        }

        OpFrameButton {
            text: qsTr("Стоп")
            color: colors.error_80
            font.pixelSize: 14
            icon.source: "images/stop_20.svg"

            Layout.fillWidth: fill
            Layout.preferredWidth: implicitWidth

            Layout.preferredHeight: 40
        }
    }
}
