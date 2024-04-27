import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Loader {
    id: loader

    function show() {
        loader.sourceComponent = comp
        Engine.setOverlayWidgetTransparent(false)
    }
    function hide() {
        loader.sourceComponent = undefined
        Engine.setOverlayWidgetTransparent(true)
        DataBus.messagebox = "" // перезарядка, чтобы одно и тоже сообщение могло показываться
    }

    onLoaded: {
        item.open()
        item.closed.connect(loader.hide)
    }

    property var headerText
    property var mainText

    property var buttonText1
    property var buttonText2
    property var buttonText3

    property var bgColor1
    property var bgColor2
    property var bgColor3

    width: 640
    height: 360

    Connections {
        target: DataBus
        function onValueChanged (key, value) {
            if (key !== "messagebox")
                return

            if (value === "")
                return

            var msg = JSON.parse(value)

            headerText = msg.headerText
            mainText = msg.mainText

            buttonText1 = msg.buttonText1
            buttonText2 = msg.buttonText2
            buttonText3 = msg.buttonText3

            bgColor1 = msg.bgColor1
            bgColor2 = msg.bgColor2
            bgColor3 = msg.bgColor3

            show()
        }
    }

    Component {
        id: comp

        Popup {
            id: root
            width: loader.width
            height: loader.height

            padding: 0
            modal: true
            closePolicy: Popup.NoAutoClose

            background: Rectangle {
                color: "white"
                radius: 8
            }

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 24

                spacing: 24

                Item {
                    id: header

                    OpText {
                        anchors.fill: parent
                        color: "black"
                        font.pixelSize: 24
                        font.weight: Font.Medium
                        text: loader.headerText
                    }

                    Layout.preferredHeight: 32
                    Layout.fillWidth: true
                }

                Item {
                    id: mainText

                    // /Layout.fillHeight: true
                    Layout.fillWidth: true

                    OpText {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        text: loader.mainText
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 16
                        font.weight: Font.Medium
                        color: "black"
                        wrapMode: Text.WordWrap
                        lineHeight: 1.5
                    }
                }

                VSpacer {}

                Item {
                    id: buttons

                    Layout.fillWidth: true
                    Layout.preferredHeight: {
                        let lc = Math.max(button1.contentItem.lineCount, button2.contentItem.lineCount, button3.contentItem.lineCount)

                        if (lc === 1)
                            return 56

                        if (lc === 2)
                            return 80
                    }

                    RowLayout {
                        anchors.fill: parent

                        spacing: 16

                        Item {
                            visible: loader.buttonText1 !== undefined

                            Layout.fillWidth: true
                            Layout.fillHeight: true

                            BlueButton {
                                id: button1
                                text: loader.buttonText1
                                onClicked: {
                                    DataBus.messagebox_result = "variant1"
                                    hide()
                                }
                                bgColor: loader.bgColor1
                            }
                        }

                        Item {
                            visible: loader.buttonText2 !== undefined

                            Layout.fillWidth: true
                            Layout.fillHeight: true

                            BlueButton {
                                id: button2
                                text: loader.buttonText2 === undefined ? "_" : loader.buttonText2 // Qt bug. Текст должен быть не пустой, иначе не lineCount не срабатывает
                                onClicked: {
                                    DataBus.messagebox_result = "variant2"
                                    hide()
                                }
                            }
                        }

                        Item {
                            visible: loader.buttonText3 !== undefined

                            Layout.fillWidth: true
                            Layout.fillHeight: true

                            BlueButton {
                                id: button3
                                text: loader.buttonText3 === undefined ? "_" : loader.buttonText3 // Qt bug. Текст должен быть не пустой, иначе не lineCount не срабатывает
                                onClicked: {
                                    DataBus.messagebox_result = "variant3"
                                    hide()
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    component BlueButton : Button {
        anchors.fill: parent
        property var bgColor: colors.primary_70

        background: Rectangle
        {
            color: parent.bgColor === undefined ? colors.primary_70 : parent.bgColor
            radius: 8
            border.width: 2
            border.color: colors.primary_70
        }

        contentItem: Text {
            text: parent.text

            font.pixelSize: 16
            font.weight: Font.Medium
            font.family: "DINPro"

            color: parent.bgColor === "white" ? colors.primary_90 : "white"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter

            wrapMode: Text.WordWrap
        }
    }

    component VSpacer: Item {
        Layout.fillHeight: true
        width: 20

        Rectangle {
            anchors.fill: parent
            visible: guiDebug
            color: "#400000FF"
        }
    }
}
