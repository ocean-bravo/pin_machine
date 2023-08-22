import QtQml 2.12
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtMultimedia 5.12


Item {
    id: root


    Column {
        x: 0
        y: 50
        spacing: 10

        Button {
            width: 100
            height: 30
            text: qsTr("update")
            onPressed: {
                Engine.setPhotoCommand(getPhotoCommand.text)
                Engine.update()
            }
        }
        SmTextEdit {
            width: 1000
            height: 30
            id: getPhotoCommand
            text: "ffmpeg -f v4l2 -i /dev/video0  -vf \"drawtext=text=%{localtime}:fontcolor=white@1.0:fontsize=26:borderw=1:x=980:y=25\" -pix_fmt yuyv422 -s 3264x2448 -frames:v 1  /dev/shm/img.png"
        }
    }
}
