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
            text: qsTr("Скорость осей Z/W")
            font.capitalization: Font.AllUppercase

            Layout.preferredHeight: 18
            Layout.fillWidth: true
        }
        OpSlider {
            Layout.fillWidth: true
        }


    }
}
