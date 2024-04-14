import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQml.Models 2.15

Loader {
    id: loader

    function show() {
        loader.sourceComponent = comp
    }
    function hide() {
        loader.sourceComponent = undefined
    }

    property bool loadOnCompleted: false

    function append(text) {
        for (const ignoreText of item.ignoreList) {
            if (text.includes(ignoreText))
                return
        }

        item.model.append({"val" : text})
    }

    function clear() {
        item.model.clear()
        item.snap = true // После очистки примагничиваемся
    }

    function ignoreSelected() {
        item.ignoreList.push(selectedText)
    }

    function clearHighlight() {
        item.highlightText = null
    }

    function highlightSelected() {
        item.highlightText = selectedText
    }

    property bool selectionEnabled: false

    property string selectedText

    Component {
        id: comp

        ListView {
            id: root

            property var ignoreList : []
            property var highlightText: null


            ListModel {
                id: listModel
            }

            model: listModel

            property bool snap: true

            clip: true

            boundsBehavior: Flickable.StopAtBounds

            Connections {
                target: listModel
                function onCountChanged() {
                    if (snap)
                        scrollBar.position = 1.0
                }
            }

            delegate: TextArea {
                id: delegateRoot
                padding: 0
                width: ListView.view.width
                font.family: "Consolas"
                font.pixelSize: 13
                textFormat: TextEdit.RichText
                wrapMode: Text.WordWrap

                text: {
                    let textToHighlight = root.highlightText

                    if (val.includes(textToHighlight))
                        return val.replace(textToHighlight, "<span style='background-color:yellow;'>" + textToHighlight + "</span>")
                    return val
                }
                renderType: Text.NativeRendering // ! Текст выглядит значительно лучше

                onSelectedTextChanged: {
                    loader.selectedText = selectedText
                }

                Connections {
                    target: selectionArea
                    function onSelectionChanged() {
                        updateSelection();
                    }
                }

                Component.onCompleted: {
                    updateSelection()
                }

                function updateSelection() {
                    if (index < selectionArea.selStartIndex || index > selectionArea.selEndIndex)
                        delegateRoot.select(0, 0);
                    else if (index > selectionArea.selStartIndex && index < selectionArea.selEndIndex)
                        delegateRoot.selectAll();
                    else if (index === selectionArea.selStartIndex && index === selectionArea.selEndIndex)
                        delegateRoot.select(selectionArea.selStartPos, selectionArea.selEndPos);
                    else if (index === selectionArea.selStartIndex)
                        delegateRoot.select(selectionArea.selStartPos, delegateRoot.length);
                    else if (index === selectionArea.selEndIndex)
                        delegateRoot.select(0, selectionArea.selEndPos);
                }

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

            function indexAtRelative(x, y) {
                return indexAt(x + contentX, y + contentY)
            }

            // Есть небольшой конфликт с ListView когда мышь pressAndHold. Но мешает несильно.
            // ListView хочет перемещаться, а selectionArea выделять текст. ListView побеждает.
            // Вообще, выделение плоховато работает, странно.
            MouseArea {
                id: selectionArea
                property int selStartIndex
                property int selEndIndex
                property int selStartPos
                property int selEndPos

                signal selectionChanged

                anchors.fill: root
                //enabled: !scrollBar.hovered && loader.selectionEnabled
                enabled: !scrollBar.hovered && loader.selectionEnabled
                cursorShape: enabled ? Qt.IBeamCursor : Qt.ArrowCursor
                acceptedButtons: Qt.LeftButton

                function indexAndPos(x, y) {
                    const index = root.indexAtRelative(x, y);
                    if (index === -1)
                        return [-1, -1]
                    const item = root.itemAtIndex(index);
                    const relItemY = item.y - root.contentY;
                    const pos = item.positionAt(x, y - relItemY);

                    return [index, pos];
                }

                onPressed: {
                    [selStartIndex, selStartPos] = indexAndPos(mouse.x, mouse.y);

                    // Пустое место пытаюсь выделить или мышь выводится за пределы окна (отрицательные значения координаты)
                    if (selEndIndex === -1 && selEndPos === -1)
                        return

                    selectionChanged();
                }

                onPositionChanged: {
                    [selEndIndex, selEndPos] = indexAndPos(mouse.x, mouse.y);

                    // Пустое место пытаюсь выделить или мышь выводится за пределы окна (отрицательные значения координаты)
                    if (selEndIndex === -1 && selEndPos === -1)
                        return

                    selectionChanged();
                }
            }
        }
    }
    Component.onCompleted: {
        if (loadOnCompleted)
            show()
    }
}
