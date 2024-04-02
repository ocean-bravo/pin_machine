import QtQml 2.12
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.15

import ImageItem 1.0

Control {
    id: root

    padding: 16

    background: OpWhitePanel {}

    contentItem: ColumnLayout {

        spacing: 16

        OpText {
            text: qsTr("ОПЦИИ КАМЕРЫ")

            Layout.preferredHeight: 30
            Layout.fillWidth: true
        }

        CustomSwitch {
            text: qsTr("Проверка реперных отверстий перед установкой")

            font.pixelSize: 16

            Layout.preferredHeight: 30
            Layout.fillWidth: true
        }

        CustomSwitch {
            text: qsTr("Проверка всех отверстий перед установкой")

            font.pixelSize: 16

            Layout.preferredHeight: 30
            Layout.fillWidth: true
        }
    }
}
