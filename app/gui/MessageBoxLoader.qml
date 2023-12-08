import QtQml 2.12
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Loader {
    id: loader

    function show() {
        loader.sourceComponent = comp
    }
    function hide() {
        loader.sourceComponent = undefined
    }
    onLoaded: {
        item.open()
        item.closed.connect(loader.hide)

        item.accept.connect(loader.accept)
        item.reject.connect(loader.reject)
    }

    property bool visibility: false

    onVisibilityChanged: visibility ? show() : hide()

    signal accept
    signal reject

    // Режим работы без кнопок. Например, для показа сообщения какое-то время.
    property bool noButtons: false

    property color backgroundColor: "white"

    property bool hasCancelButton: false
    property bool hasOkButton: true

    property string text: ""
    property string okButtonText: qsTr("OK")
    property string cancelButtonText: qsTr("Cancel")

    width: appWin.width * 0.75
    height: appWin.height / 2

    Component {
        id: comp

        Popup {
            id: root
            width: loader.width
            height: loader.height
            anchors.centerIn: Overlay.overlay
            modal: true
            closePolicy: Popup.NoAutoClose
            z: loader.z

            signal accept
            signal reject

            property bool noButtons: loader.noButtons

            property color backgroundColor: loader.backgroundColor

            property bool hasCancelButton: loader.hasCancelButton
            property bool hasOkButton: loader.hasOkButton

            property string text: loader.text
            property string okButtonText: loader.okButtonText
            property string cancelButtonText: loader.cancelButtonText

            // От этого значения в сторону уменьшения идет подбор размера шрифта, чтобы текст помещался в отведенную ему область.
            readonly property int defaultFontPixelSize: 38

            background: Rectangle {
                color: root.backgroundColor
            }

            ColumnLayout {
                spacing: 0
                anchors.fill: parent
                Item {
                    Layout.fillHeight: true
                    Layout.preferredWidth: parent.width

                    SmText {
                        id: text
                        anchors.fill: parent
                        text: root.text
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 38
                        lineHeight: 0.9
                        color: "white"
                        wrapMode: Text.WordWrap

                        // Итеративный подбор размера шрифта, чтобы текст влезал в отведенную ему область.
                        // Вызов функции через таймер нужен чтобы функция могла вызывать сама себя, но чтобы это получилось не рекурсивно.
                        function calculateFontSize() {
                            if (text.contentHeight > parent.height) {
                                font.pixelSize -= 1
                                asyncCallTimer.start()
                            }
                        }

                        Timer {
                            id: asyncCallTimer
                            interval: 1
                            onTriggered: text.calculateFontSize()
                        }

                        onTextChanged: {
                            font.pixelSize = defaultFontPixelSize
                            asyncCallTimer.start()
                        }
                    }
                }

                Item {
                    id: buttons
                    visible: !root.noButtons
                    width: parent.width
                    Layout.preferredHeight: 60

                    RowLayout {
                        anchors.fill: parent

                        Item {
                            visible: root.hasOkButton
                            Layout.preferredWidth: parent.width / self.buttonsCount

                            Button {
                                anchors.centerIn: parent
                                text: root.okButtonText
                                font.pixelSize: 28
                                onClicked: root.accept()
                            }
                        }

                        Item {
                            visible: root.hasCancelButton
                            Layout.preferredWidth: parent.width / self.buttonsCount

                            Button {
                                anchors.centerIn: parent
                                text: root.cancelButtonText
                                font.pixelSize: 28
                                onClicked: root.reject()
                            }
                        }
                    }
                }
            }

            QtObject {
                id: self

                // Костыль. По другому не смог надежно определить количество кнопок в диалоге.
                readonly property int buttonsCount: root.hasCancelButton ? 2 : 1
            }
        }
    }
}
