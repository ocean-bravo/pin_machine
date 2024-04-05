import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQml.Models 2.15

ListView {
    id: root

    ListModel {
        id: listModel
    }

    model: listModel

    property bool snap

    clip: true

    boundsBehavior: Flickable.StopAtBounds

    Connections {
        target: listModel
        function onCountChanged() {
            if (snap)
                scrollBar.position = 1.0
        }
    }

    delegate: SmText {
        padding: 0
        height: 18
        font.pixelSize: 14
        textFormat: TextEdit.RichText
        text: val
    }

    onAtYEndChanged: {
        if (atYEnd)
            snap = true

        if (!atYEnd && scrollBar.pressed)
            snap = false
    }

    onMovementStarted: {
        snap = false
    }

    ScrollBar.vertical: ScrollBar {
        id: scrollBar
        policy: ScrollBar.AlwaysOn
        minimumSize: 0.1
    }

    function append(text) {
        listModel.append({"val" : text})
    }

    function clear() {
        listModel.clear()
    }
}
