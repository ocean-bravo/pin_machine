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

        OpHeader { text: qsTr("Визуализация продукта") }

        Item {
            id: scene
            objectName: "placeholderForGraphicsView"

            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }
}
