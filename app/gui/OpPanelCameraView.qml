import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtGraphicalEffects 1.15

import ImageItem 1.0

Control {
    id: root

    padding: 16

    background: OpWhitePanel {}

    component VGap8 : Item { width: 10; DebugRect {} Layout.preferredHeight: 8}
    component VGap16 : Item { width: 10; DebugRect {} Layout.preferredHeight: 16}

    contentItem: ColumnLayout {
        spacing: 0

        OpHeader { text: qsTr("ВИД С КАМЕРЫ") }
        VGap16 {}

        OpAspectCameraView {}

        VGap16 {}

        Vspacer {}

        OpFrameButton {
            icon.source: checked ? "images/videocam_off_20.svg" : "images/videocam_on_20.svg"
            checkable: true
            text: checked ? qsTr("Выключить камеру") : qsTr("Включить камеру")
            color: colors.primary_90

            Layout.preferredHeight: 56


            onCheckedChanged: {
                if (checked) {
                    //resolutionList.setCurrentFormat()
                    Video4.start()
                }
                else {
                    Video4.stop()
                    execAfterDelay(function() { image.clear(); }, 100)
                }
            }
        }

        Timer {
            id: findCameraTimer
            interval: 1000
            repeat: false
            triggeredOnStart: false
            running: true
            onTriggered: {
                //Video4.reloadDevices()
            }
        }
    }
}
