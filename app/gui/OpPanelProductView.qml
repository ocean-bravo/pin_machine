import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import QmlGraphicsView 1.0
import QuickScene 1.0

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

        QmlGraphicsView {
            id: scene
            objectName: "overlayItem"

            //color: "yellow"
            scene: GraphicsScene

            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }
}
