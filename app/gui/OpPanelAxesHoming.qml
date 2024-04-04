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

                OpText {
                    text: qsTr("СТАТУС ИНИЦИАЛИЗАЦИИ ОСЕЙ (HOMING):")

                    Layout.preferredHeight: 32
                    Layout.fillWidth: true
                }

                OpFrameButton {
                    text: DataBus.is_homed ? qsTr("Готов") : qsTr("Не готов")

                    font.pixelSize: 16

                    color: DataBus.is_homed ? colors.green : colors.error_80

                    Layout.preferredHeight: 32
                    Layout.fillWidth: true
                }
            }
        }

        OpFrameButton {
            text: qsTr("Выполнить инициализацию осей")
            icon.source: "images/home.png"
            icon.width: 20
            icon.height: 20
            solid: true
            font.pixelSize: 16
            font.weight: Font.Medium

            color: colors.primary_70

            Layout.preferredHeight: 44
            Layout.fillWidth: true

            onClicked: {
                write("$H")
            }

            enabled: {
                let status = DataBus.status

                if (status === "Idle")
                    return true

                if (status === "Alarm")
                    return true

                return false
            }
        }
    }
}
