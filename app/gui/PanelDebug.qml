import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

CollapsiblePanel2 {
    id: root

    width: parent.width

    text: "Debug"

    GridLayout {
        id: grid
        width: parent.width

        columns: 10
        columnSpacing: 5
        rowSpacing: 5

        DoubleSpinBox {
            id: pixelSizeSpinBox
            decimals: 3
            value: DataBus.pixInMm()
            onValueModified: DataBus.setPixInMm(value)

            Connections { target: DataBus; function onPixelSizeChanged() { pixelSizeSpinBox.value = DataBus.pixInMm() }}

            Layout.row: 0
            Layout.column: 0
            Layout.columnSpan: 3
        }

        ComboBox {
            id: dbKeys
            model: DataBus.keys()
            onDownChanged: {
                if (down)
                    model = DataBus.keys().sort()
            }

            onActivated: {
                logViewer.append('<br>')
                logViewer.append(currentText + '<br>')
                let data = DataBus[currentText]

                if(data === undefined)
                    return

                data = JSON.stringify(data).replace(/[,]/g, '<br>')
                logViewer.append(data)
                logViewer.append('<br>')
            }

            Layout.row: 1
            Layout.column: 0
            Layout.columnSpan: 6

            Layout.preferredWidth: 140
        }
        SmTextEdit {
            id: sendDataBus
            Layout.row: 1
            Layout.column: 6
            Layout.columnSpan: 2
            Layout.preferredWidth: 90
        }
        ComboBox {
            id: dataType
            model: ["text", "int", "double"]
            Layout.row: 1
            Layout.column: 8
            Layout.preferredWidth: 60
            //Layout.columnSpan: 1
            //Layout.fillWidth: true
        }

        SmButton {
            text: qsTr("Write value")
            onClicked: {
                if (dataType.currentText === "int")    DataBus[dbKeys.currentText] = parseInt(sendDataBus.text)
                if (dataType.currentText === "double") DataBus[dbKeys.currentText] = parseFloat(sendDataBus.text)
                if (dataType.currentText === "text")   DataBus[dbKeys.currentText] = sendDataBus.text
            }
            Layout.row: 1
            Layout.column: 9
            //Layout.columnSpan: 1
            //Layout.fillWidth: true
        }

        SmButton {
            id: testScanUpdateCycle
            text: qsTr("Begin test")
            checkable: true
            checked: false
            onCheckedChanged: checked ? TaskTestScanUpdateCycle.run(codeEditor.text) : TaskTestScanUpdateCycle.stopProgram()
            Layout.row: 2
        }

        SmButton {
            id: testAlgo
            text: qsTr("Test match points")
            checkable: true
            checked: false
            onCheckedChanged: checked ? TaskTestAlgo.run() : TaskTestAlgo.stopProgram()
            Layout.row: 3
        }
        SmButton {
            id: findPixelSize
            text: qsTr("Find pixel size")
            checkable: true
            checked: false
            onCheckedChanged: checked ? TaskFindPixelSize.run() : TaskFindPixelSize.stopProgram()
            Connections { target: TaskFindPixelSize; function onFinished() { findPixelSize.checked = false } }
            Layout.row: 4
        }
        // SmButton {
        //     id: findBestPath
        //     text: qsTr("Best path")
        //     checkable: true
        //     checked: false
        //     property int count: 0
        //     onCheckedChanged: {
        //         if (checked) {
        //             TaskBestPath.run()
        //             //count = 0
        //             // splash.text = Qt.binding(function() { return "Searching optimal path..." + "\n" + count} )
        //             // splash.backgroundColor = "green"
        //             //splash.open()
        //             //countTimer.start()
        //            }
        //         else {
        //             TaskBestPath.stopProgram()
        //             //splash.close()
        //             //countTimer.stop()
        //         }
        //     }
        //     Timer {
        //         id: countTimer
        //         interval: 1000
        //         repeat: true
        //         running: false
        //         onTriggered: ++findBestPath.count
        //     }

        //     Connections { target: TaskBestPath; function onFinished() { findBestPath.checked = false } }
        //     Layout.row: 5
        // }
        //Button {onClicked: Engine.capture1();Layout.row: 5; Layout.column: 0; Layout.columnSpan: 4}
        // Button {
        //     onClicked: {
        //         Engine.capture2();
        //     }
        //     Layout.row: 5; Layout.column: 4; Layout.columnSpan: 3
        // }
        // Button {onClicked: Engine.corr();    Layout.row: 5; Layout.column: 7; Layout.columnSpan: 3}
    }
}
