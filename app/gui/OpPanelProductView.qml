import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Control {
    id: root

    property string objName

    padding: 16

    background: OpWhitePanel {}

    contentItem: ColumnLayout {

        spacing: 16

        OpHeader { text: qsTr("Визуализация продукта") }

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true

            Item {
               anchors.fill: parent
               objectName: root.objName
            }
        }
    }
}
