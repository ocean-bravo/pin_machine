import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

CollapsiblePanel2 {
    id: jogPanel

    width: parent.width

    text: qsTr("Jog control")

    JogControl {
        id: jogControl
    }
}
