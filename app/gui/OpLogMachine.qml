import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

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
            property bool snap: true

            Connections {
                target: LogModel
                function onNewRows() {
                    if (snap)
                        scrollBar.position = 1.0
                }
            }

            Connections {
                target: Serial
                function onMessage(text) {
                    footerText.text = text
                }
            }

            ListView {
                anchors.fill: parent
                model: LogModel
                boundsBehavior: Flickable.StopAtBounds
                delegate: TextArea {
                    padding: 0
                    width: ListView.view.width
                    textFormat: TextEdit.AutoText
                    wrapMode: Text.Wrap
                    text: display
                    background: Rectangle {
                        gradient: Gradient {
                            GradientStop { position: 0; color: "#ffffff" }
                            GradientStop { position: 1; color: "#c1bbf9" }
                        }
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



            }

            QtObject {
                id: self

                // Костыль. По другому не смог надежно определить количество кнопок в диалоге.
                readonly property int buttonsCount: root.hasCancelButton ? 2 : 1
            }
        }
    }
}
