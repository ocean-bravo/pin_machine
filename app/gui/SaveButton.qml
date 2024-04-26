import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

// MessageBox находятся в другом QML engine. Туда нужно передать внешний вид и текст сообщения, а назад получить результат.
// Туда отсылается по каналу "messagebox". Результат получается по каналу "messagebox_result". Варианты ответа: accept, reject
SmButton {
    text: ("💾")

    Layout.preferredWidth: 30
    Layout.preferredHeight: 30

    onClicked: {
        const messageBox = {
            text: qsTr("Really?"),
            backgroundColor: "lightblue",
            hasCancelButton: true,
            okButtonText: qsTr("Save"),
            cancelButtonText: qsTr("No")
        }

        // Просто объект что-то не пролазит, поэтому строка.
        DataBus.messagebox = JSON.stringify(messageBox)

        messageBoxResult.enabled = true

        console.log("messabe box created")
    }

    property var acceptFunc

    Connections {
        id: messageBoxResult
        target: DataBus
        function onValueChanged (key, value) {
            if (key !== "messagebox_result")
                return

            if (value === "")
                return

            messageBoxResult.enabled = false


            console.log("mb result: ", value)

            if (value === "accept") {
                acceptFunc()
            }

            DataBus.messagebox_result = "" // Перезарядка
        }
    }
}
