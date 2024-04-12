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
            text: qsTr("ОПЦИИ КАМЕРЫ")

            Layout.preferredHeight: 18
            Layout.fillWidth: true
        }

        CustomSwitch {
            text: qsTr("Проверка реперных отверстий перед установкой")



            Layout.preferredHeight: 48
            Layout.fillWidth: true
        }

        CustomSwitch {
            text: qsTr("Проверка всех отверстий перед установкой")



            Layout.preferredHeight: 48
            Layout.fillWidth: true
        }
    }
}
