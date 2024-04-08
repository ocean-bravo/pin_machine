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
            text: qsTr("ВИЗУАЛИЗАЦИЯ ПРОДУКТА")

            Layout.preferredHeight: 18
            Layout.fillWidth: true
        }

        Item {
            id: scene
            objectName: "placeholderForGraphicsView"

            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }
}
