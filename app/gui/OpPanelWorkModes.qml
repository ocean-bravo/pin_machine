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

        OpText {
            text: qsTr("РЕЖИМЫ РАБОТЫ")

            Layout.preferredHeight: 18
            Layout.fillWidth: true
        }

        CustomSwitch {
            text: qsTr("Без установки")

            Layout.preferredHeight: 48
            Layout.fillWidth: true
        }

        CustomSwitch {
            text: qsTr("Пошагово")

            Layout.preferredHeight: 48
            Layout.fillWidth: true
        }
    }
}
