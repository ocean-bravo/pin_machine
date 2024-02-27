import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15

CollapsiblePanel {
    id: root

    height: checked ? 30 + implicitContentHeight : 30

    onCheckedChanged: {
        contentItem.visible = checked
    }

    Component.onCompleted: {
        contentItem.visible = checked
    }
}
