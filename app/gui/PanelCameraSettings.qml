import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

CollapsiblePanel {
    id: root

    width: parent.width
    height: checked ? 90 : 25

    Layout.preferredWidth: 400
    Layout.preferredHeight: checked ? 90 : 25

    text: qsTr("Camera settings")

    onCheckedChanged: {
        column.visible = checked
    }
    Component.onCompleted: {
        column.visible = checked
    }

    ColumnLayout {
        id: column
        anchors.top:parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        RowLayout {
            Layout.fillWidth: true
            Text {
                Layout.preferredWidth: 140
                text: "Auto Exposure"
            }

        ComboBox {
            id: dbKeys
            textRole: "text"
            valueRole: "value"

            model: ListModel {
                  ListElement { text: "Manual Mode";            value: 1 }
                  ListElement { text: "Aperture Priority Mode"; value: 3 }
            }

            // onDownChanged: {
            //     if (down)
            //         model = DataBus.keys().sort()
            // }

            onActivated: {
                DataBus.cam_prop_exp_type = currentValue


                // logViewer.append('<br>')
                // logViewer.append(currentText + '<br>')
                // let data = DataBus[currentText]

                // if(data === undefined)
                //     return

                // data = JSON.stringify(data).replace(/[,]/g, '<br>')
                // logViewer.append(data)
                // logViewer.append('<br>')
            }
            Layout.preferredWidth: 140
        }
        }

        Slider { id: exposure; from: 1; to: 2000; value: 150; stepSize: 1;
            Layout.fillWidth: true;
            Layout.preferredHeight: 30;
            Text {
                anchors.centerIn: parent
                text: "Exposure Time, Absolute: " + parent.value
                bottomPadding: 20
            }
            onValueChanged: DataBus.cam_prop_exp_time = value
        }


    }
}
