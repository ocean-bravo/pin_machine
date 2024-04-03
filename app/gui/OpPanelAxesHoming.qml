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

        RowLayout {

            Layout.preferredHeight: 32
            Layout.fillWidth: true

            OpText {
                text: qsTr("СТАТУС ИНИЦИАЛИЗАЦИИ ОСЕЙ (HOMING):")

                Layout.preferredHeight: 32
                Layout.fillWidth: true
            }

            OpFrameButton {
                text: qsTr("Готов")

                font.pixelSize: 16

                color: colors.primary_70

                Layout.preferredHeight: 32
                Layout.fillWidth: true
            }
        }

        OpFrameButton {
            text: qsTr("Выполнить инициализацию осей")
            solid: true
            font.pixelSize: 16

            color: colors.primary_70

            Layout.preferredHeight: 44
            Layout.fillWidth: true
        }

    }
}
