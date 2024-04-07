import QtQml 2.12
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12


// Item {
//     id: root
//     width: 200
//     height: 200
//     Canvas {
//         id: cloud
//         anchors.fill: parent

//         onPaint: {
//             var ctx = getContext("2d");
//             ctx.beginPath();
//             var x = 100;
//             var y = 170;
//             ctx.arc(x, y, 60, Math.PI * 0.5, Math.PI * 1.5);
//             ctx.arc(x + 70, y - 60, 70, Math.PI * 1, Math.PI * 1.85);
//             ctx.arc(x + 152, y - 45, 50, Math.PI * 1.37, Math.PI * 1.91);
//             ctx.arc(x + 200, y, 60, Math.PI * 1.5, Math.PI * 0.5);
//             ctx.moveTo(x + 200, y + 60);
//             ctx.lineTo(x, y + 60);
//             ctx.strokeStyle = "#797874";
//             ctx.stroke();
//             ctx.fillStyle = "#8ED6FF";
//             ctx.fill();
//         }
//     }
// }


Rectangle {
    id: appWin
    width: 300
    height: 300


    color: "#80808080"

    // Popup {
    //     id: popup
    //     x: 200
    //     y: 200
    //     width: 500
    //     height: 300
    //     modal: true
    //     focus: true
    //     closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    //     //z: 2
    // }

    FlashMessageBox {
        id: message
        text: "sfadfa "//ataBus.messagebox
        backgroundColor: "maroon"
        interval: 2000
        x: 10
        y: 10
        onTextChanged: {
            if (text.length > 0)
                open()
        }
        onClosed: appWin.enabled = false
        // Component.onCompleted: DataBus.messagebox = ""  // Для убирания warninga "Unable to assign [undefined] to QString"
    }

    Timer {
        repeat: true
        interval: 5000
        running: true
        onTriggered: {
            message.open()
            appWin.enabled = true
        }
    }

    Shortcut {
        sequence: "F5"
        context: Qt.ApplicationShortcut
        onActivated: {

            console.log("popun open")
            message.open()
        }

    }


}
