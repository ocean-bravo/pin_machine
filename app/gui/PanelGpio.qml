import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import "utils.js" as Utils

CollapsiblePanel2 {
    id: root

    width: parent.width

    text: qsTr("GPIO state")

    Component.onCompleted: {
        // // Начальный прогрев значения, чтобы не сыпало ошибками
        let pins = new Array(32)

        for (let i = 0; i < pins.length; ++i)
            pins[i] = {};

        DataBus.gpio = pins
    }

    component Pin : Rectangle {
        width: 10
        height: 10
        border.width: 1
        border.color: "white"
        color: {
            if (modelData.dir === 'input') return "red"
            if (modelData.dir === 'output') return "green"
            return "lightgrey"
        }
        opacity: {
            if (modelData.value === '0') return 0.2
            return 1.0
        }
    }

    component PinNumber: SmText {
        width: 10
        height: 10
        font.pixelSize: 10
        text: modelData.number === undefined ? "" : modelData.number

    }

    ColumnLayout {
        id: column
        width: parent.width
        spacing: 2

        Rectangle {
            color: guiDebug ? "#4000FF00" : Qt.transparent
            Layout.fillWidth: true
            Layout.preferredHeight: 30

            RowLayout {
                anchors.fill: parent
                spacing: 1

                Repeater {
                    model: DataBus.gpio.slice(16, 31)

                    Column {
                        padding: 0
                        spacing: 10

                        PinNumber { }
                        Pin { }
                    }
                }
            }
        }

        Rectangle {
            color: guiDebug ? "#4000FF00" : Qt.transparent
            Layout.fillWidth: true
            Layout.preferredHeight: 30

            RowLayout {
                anchors.fill: parent
                spacing: 1

                Repeater {
                    model: DataBus.gpio.slice(0, 15)

                    Column {
                        PinNumber { }
                        Pin { }
                    }
                }
            }
        }

        // Item {
        //     Layout.fillHeight: true
        //     Layout.fillWidth: true
        // }

    }
}
