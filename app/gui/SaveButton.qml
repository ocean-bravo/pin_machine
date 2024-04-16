import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

SmButton {
    text: ("💾")

    Layout.preferredWidth: 30
    Layout.preferredHeight: 30

    onClicked: saveMessageBox.show()

    property var acceptFunc

    MessageBoxLoader {
        id: saveMessageBox
        text: qsTr("Really?")
        backgroundColor: "lightblue"
        hasCancelButton: true
        okButtonText: qsTr("Save")
        cancelButtonText: qsTr("No")


        width: 500
        height: 250
        parent: appWin
        anchors.centerIn: parent

        onAccept: {
            acceptFunc()
            hide()
        }
        onReject: hide()
    }
}
