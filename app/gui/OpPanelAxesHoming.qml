import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15


Control {
    id: root

    padding: 16

    background: OpWhitePanel {}

    contentItem: ColumnLayout {

        spacing: 16

        Item {
            Layout.preferredHeight: 32
            Layout.fillWidth: true

            RowLayout {
                anchors.fill: parent
                spacing: 4

                OpText {
                    text: qsTr("СТАТУС ИНИЦИАЛИЗАЦИИ ОСЕЙ (HOMING):")

                    Layout.preferredHeight: 32
                }

                OpText {
                    text: {
                        if (DataBus.homing_status === "Not ready")   return qsTr("Не готов")
                        if (DataBus.homing_status === "In progress") return qsTr("В процессе")
                        if (DataBus.homing_status === "Ready")       return qsTr("Готов")
                    }

                    color: {
                        if (DataBus.homing_status === "Not ready")   return colors.error_80
                        if (DataBus.homing_status === "In progress") return colors.accent_90
                        if (DataBus.homing_status === "Ready")       return colors.green
                    }

                    font.pixelSize: 16
                    font.weight: Font.DemiBold

                    Layout.preferredHeight: 32
                    Layout.fillWidth: true
                }
            }
        }

        OpSolidButton {
            text: qsTr("Выполнить инициализацию осей")
            icon.source: "images/home.svg"

            font.weight: Font.Medium

            bgcolor: colors.primary_70

            Layout.preferredHeight: 44
            Layout.fillWidth: true

            onClicked: {
                write("$H")
            }

            enabled: DataBus.homing_status === "Not ready"
        }
    }
}
