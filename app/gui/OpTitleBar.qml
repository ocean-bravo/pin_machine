import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15

Item {

    height: 40
    width: parent.width

    Rectangle {

        anchors.fill: parent

        color: "gold"


        Text {
            anchors.verticalCenter: parent.verticalCenter
            leftPadding: 8
            text: "pin_machine"//appWin.title
            color: "white"
        }

        MouseArea {
            anchors.fill: parent

            onPressed: {
                previousX = mouseX
                previousY = mouseY
            }

            onMouseXChanged: {
                var dx = mouseX - previousX
                appWin.setX(mainWindow.x + dx)
            }

            onMouseYChanged: {
                var dy = mouseY - previousY
                appWin.setY(mainWindow.y + dy)
            }

        }

    }

    MouseArea {
        width: 5

        anchors {
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }

        cursorShape: Qt.SizeHorCursor

        onPressed: previousX = mouseX

        onMouseXChanged: {
            var dx = mouseX - previousX
            appWin.setWidth(parent.width + dx)
        }

    }

    MouseArea {
        height: 5

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }

        cursorShape: Qt.SizeVerCursor

        onPressed: previousY = mouseY

        onMouseYChanged: {
            var dy = mouseY - previousY
            appWin.setY(appWin.y + dy)
            appWin.setHeight(appWin.height - dy)

        }
    }
}
