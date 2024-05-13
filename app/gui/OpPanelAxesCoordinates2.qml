import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15


Control {
    id: root

    padding: 16

    background: OpWhitePanel {}

    component VGap16 : Item { width: 10; DebugRect {} Layout.preferredHeight: 16}
    component VGap20 : Item { width: 10; DebugRect {} Layout.preferredHeight: 20}

    component HGap8 : Item { height: 10; DebugRect {} Layout.preferredWidth: 8}
    component HGap24 : Item { height: 10; DebugRect {} Layout.preferredWidth: 24}

    component MyText: OpText {
        Layout.preferredWidth: 24
        Layout.fillHeight: true
        horizontalAlignment: Text.AlignLeft

        font.weight: Font.Medium
        font.pixelSize: 24
        color: colors.black_90
    }

    contentItem: Item {
        DebugRect { color: "#22FF0FF0" }

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            OpHeader { text: qsTr("КООРДИНАТЫ ОСЕЙ") }

            VGap16 {}

            Item {
                Layout.preferredHeight: 40
                Layout.fillWidth: true

                RowLayout {
                    anchors.fill: parent
                    spacing: 0

                    MyText { text: "X" }
                    HGap8 {}
                    OpDoubleSpinbox {
                        value: DataBus.xPos === undefined ? 0.0 : DataBus.xPos
                        editable: false
                    }

                    HGap24 {}

                    MyText { text: "Y" }
                    HGap8 {}
                    OpDoubleSpinbox {
                        value: DataBus.yPos === undefined ? 0.0 : DataBus.yPos
                        editable: false
                    }
                    Hspacer {}
                }
            }

            VGap16 {}

            Item {
                Layout.preferredHeight: 40
                Layout.fillWidth: true

                RowLayout {
                    anchors.fill: parent
                    spacing: 0

                    MyText { text: "Z" }
                    HGap8{}
                    OpDoubleSpinbox {
                        value: DataBus.zPos === undefined ? 0.0 : DataBus.zPos
                        editable: false
                    }

                    HGap24{}

                    MyText { text: "W" }
                    HGap8{}
                    OpDoubleSpinbox {
                        value: 0
                        editable: false
                    }
                    Hspacer {}
                }
            }
            Vspacer {}
        }
    }
}
