import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

CollapsiblePanel {
    id: root
    //width: parent.width
    width: 400
    checked: false
    height: checked ? 230 : 30

    text: qsTr("Blob detection settings")

    onCheckedChanged: {
        column.visible = checked
    }
    Component.onCompleted: {
        column.visible = checked
    }

    ColumnLayout {
        id: column
        width: parent.width

        BlobSettings {
            Layout.preferredHeight: 200
            Layout.fillWidth: true

        }
    }
}
