import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15

Rectangle {
    //color: "#4000FF00"
    color: "transparent"
    height: lay.height

    //Layout.preferredHeight: lay.height

    Component.onCompleted: {
        DataBus.blob_ad_tr_enable = false
        DataBus.blob_filter_area_enabled = true
        DataBus.blob_filter_convexity_enabled = false
        DataBus.blob_filter_convexity_min = 0.8
        DataBus.blob_filter_convexity_max = 1.0
    }

    Column {
        id: lay
        anchors.top:parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: col1.height + col2.height + col3.height + 75


        CollapsiblePanel {
            id: col1
            text: qsTr("adaptive threshold")

            anchors.left: parent.left
            anchors.right: parent.right

            height: checked ? 30  + column1.height : 30

            checked: DataBus.blob_ad_tr_enable

            onCheckedChanged: {
                column1.visible = checked
                DataBus.blob_ad_tr_enable = checked
            }
            Component.onCompleted: {
                column1.visible = checked
            }

            ColumnLayout {
                id: column1
                anchors.top:parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                spacing: 0

                RowSpinSlider { text: "block size"; from: 3; to: 2001; stepSize: 2; value: DataBus.blob_ad_tr_blockSize
                    valueBind: function() { return DataBus.blob_ad_tr_blockSize }
                    onValueChanged: DataBus.blob_ad_tr_blockSize = value
                }

                RowDoubleSpinSlider { text: "C"; from: -100.0; to: 100.0; stepSize: 0.1; value: DataBus.blob_ad_tr_c
                    valueBind: function() { return DataBus.blob_ad_tr_c }
                    onValueChanged: DataBus.blob_ad_tr_c = value
                }

                ComboBox {
                    id: adTrType
                    width: 300
                    valueRole: "value"
                    textRole: "text"

                    Layout.fillWidth: true
                    Layout.preferredHeight: 25
                    model: ListModel {
                        ListElement { text: "ADAPTIVE_THRESH_MEAN_C";     value: 0 }
                        ListElement { text: "*ADAPTIVE_THRESH_GAUSSIAN_C"; value: 1 }
                    }
                    onActivated: DataBus.blob_ad_tr_type = currentValue
                }

                ComboBox {
                    id: trType
                    valueRole: "value"
                    textRole: "text"
                    Layout.fillWidth: true
                    Layout.preferredHeight: 25
                    //width: 300
                    model: ListModel {
                        ListElement { text: "THRESH_BINARY";     value: 0 }
                        ListElement { text: "*THRESH_BINARY_INV"; value: 1 }
                        ListElement { text: "THRESH_TRUNC";      value: 2 }
                        ListElement { text: "THRESH_TOZERO";     value: 3 }
                        ListElement { text: "THRESH_TOZERO_INV"; value: 4 }
                        ListElement { text: "THRESH_MASK";       value: 7 }
                        ListElement { text: "THRESH_OTSU";       value: 8 }
                        ListElement { text: "THRESH_TRIANGLE";   value: 16 }
                    }

                    onActivated: DataBus.blob_tr_type = currentValue
                    //Component.onCompleted: DataBus.mode = "raw"
                }
            }

        }

        // Rectangle {
        //     color: "#20f00000"
        //     //color: "transparent"
        //     height: col1.height

        //     anchors.left: parent.left
        //     anchors.right: parent.right

        //     Behavior on height {
        //         NumberAnimation { duration: 30 }
        //     }

        //     Column {
        //         id: col1

        //         spacing: 0

        //         anchors.top:parent.top
        //         anchors.left: parent.left
        //         anchors.right: parent.right

        //         height: checkbox1.height + pane1.height

        //         CheckBox {
        //             id: checkbox1
        //             height:20
        //             anchors.left: parent.left
        //             anchors.right: parent.right

        //             checked: DataBus.blob_ad_tr_enable
        //             onCheckedChanged: DataBus.blob_ad_tr_enable = checked
        //             text: qsTr("adaptive threshold")
        //         }

        //         Pane {
        //             id: pane1
        //             visible: checkbox1.checked

        //             padding: 0
        //             anchors.left: parent.left
        //             anchors.right: parent.right
        //             anchors.leftMargin: 10
        //             anchors.rightMargin: 10

        //             ColumnLayout {
        //                 id: column1
        //                 anchors.fill: parent
        //                 spacing: 0

        //                 RowSpinSlider { text: "block size"; from: 3; to: 2001; stepSize: 2; value: DataBus.blob_ad_tr_blockSize
        //                     valueBind: function() { return DataBus.blob_ad_tr_blockSize }
        //                     onValueChanged: DataBus.blob_ad_tr_blockSize = value
        //                 }

        //                 RowDoubleSpinSlider { text: "C"; from: -100.0; to: 100.0; stepSize: 0.1; value: DataBus.blob_ad_tr_c
        //                     valueBind: function() { return DataBus.blob_ad_tr_c }
        //                     onValueChanged: DataBus.blob_ad_tr_c = value
        //                 }

        //                 ComboBox {
        //                     id: adTrType
        //                     width: 300
        //                     valueRole: "value"
        //                     textRole: "text"

        //                     Layout.fillWidth: true
        //                     Layout.preferredHeight: 25
        //                     model: ListModel {
        //                         ListElement { text: "ADAPTIVE_THRESH_MEAN_C";     value: 0 }
        //                         ListElement { text: "*ADAPTIVE_THRESH_GAUSSIAN_C"; value: 1 }
        //                     }
        //                     onActivated: DataBus.blob_ad_tr_type = currentValue
        //                 }

        //                 ComboBox {
        //                     id: trType
        //                     valueRole: "value"
        //                     textRole: "text"
        //                     Layout.fillWidth: true
        //                     Layout.preferredHeight: 25
        //                     //width: 300
        //                     model: ListModel {
        //                         ListElement { text: "THRESH_BINARY";     value: 0 }
        //                         ListElement { text: "*THRESH_BINARY_INV"; value: 1 }
        //                         ListElement { text: "THRESH_TRUNC";      value: 2 }
        //                         ListElement { text: "THRESH_TOZERO";     value: 3 }
        //                         ListElement { text: "THRESH_TOZERO_INV"; value: 4 }
        //                         ListElement { text: "THRESH_MASK";       value: 7 }
        //                         ListElement { text: "THRESH_OTSU";       value: 8 }
        //                         ListElement { text: "THRESH_TRIANGLE";   value: 16 }
        //                     }

        //                     onActivated: DataBus.blob_tr_type = currentValue
        //                     //Component.onCompleted: DataBus.mode = "raw"
        //                 }
        //             }
        //         }
        //     }
        // }


        CollapsiblePanel {
            id: col2

            text: qsTr("filter by area")

            anchors.left: parent.left
            anchors.right: parent.right

            height: checked ? 30  + column1.height : 30

            checked: DataBus.blob_filter_area_enabled


            onCheckedChanged: {
                column2.visible = checked
                DataBus.blob_filter_area_enabled = checked
            }
            Component.onCompleted: {
                column2.visible = checked
            }

            ColumnLayout {
                id: column2
                anchors.fill: parent
                spacing: 0

                RowDoubleSpinSlider { text: "minDia_mm"; from: 0.3; to: 6.0; stepSize: 0.1; value: DataBus.blob_minDia_mm
                    valueBind: function() { return DataBus.blob_minDia_mm }
                    onValueChanged: DataBus.blob_minDia_mm = value
                }
                RowDoubleSpinSlider { text: "maxDia_mm"; from: 0.3; to: 6.0; stepSize: 0.1; value: DataBus.blob_maxDia_mm
                    valueBind: function() { return DataBus.blob_maxDia_mm }
                    onValueChanged: DataBus.blob_maxDia_mm = value
                }
            }
        }

        // Rectangle {
        //     //color: "#200f0000"
        //     color: "transparent"
        //     height: col2.height

        //     anchors.left: parent.left
        //     anchors.right: parent.right

        //     Behavior on height {
        //         NumberAnimation { duration: 30 }
        //     }

        //     Column {
        //         id: col2

        //         spacing: 0

        //         anchors.top:parent.top
        //         anchors.left: parent.left
        //         anchors.right: parent.right

        //         height: checkbox2.height + pane2.height

        //         CheckBox {
        //             id: checkbox2
        //             height: 20
        //             anchors.left: parent.left
        //             anchors.right: parent.right

        //             checked: DataBus.blob_filter_area_enabled
        //             onCheckedChanged: DataBus.blob_filter_area_enabled = checked
        //             text: qsTr("filter by area")
        //         }

        //         Pane {
        //             id: pane2
        //             visible: checkbox2.checked

        //             padding: 0
        //             anchors.left: parent.left
        //             anchors.right: parent.right
        //             anchors.leftMargin: 10
        //             anchors.rightMargin: 10

        //             ColumnLayout {
        //                 id: column2
        //                 anchors.fill: parent
        //                 spacing: 0

        //                 RowDoubleSpinSlider { text: "minDia_mm"; from: 0.3; to: 6.0; stepSize: 0.1; value: DataBus.blob_minDia_mm
        //                     valueBind: function() { return DataBus.blob_minDia_mm }
        //                     onValueChanged: DataBus.blob_minDia_mm = value
        //                 }
        //                 RowDoubleSpinSlider { text: "maxDia_mm"; from: 0.3; to: 6.0; stepSize: 0.1; value: DataBus.blob_maxDia_mm
        //                     valueBind: function() { return DataBus.blob_maxDia_mm }
        //                     onValueChanged: DataBus.blob_maxDia_mm = value
        //                 }
        //             }
        //         }
        //     }
        // }

        CollapsiblePanel {
            id: col3
            checked: DataBus.blob_filter_convexity_enabled

            text: qsTr("filter by convexity")

            anchors.left: parent.left
            anchors.right: parent.right

            height: checked ? 30  + column1.height : 30

            onCheckedChanged: {
                column3.visible = checked
                DataBus.blob_filter_convexity_enabled = checked
            }
            Component.onCompleted: {
                column3.visible = checked
            }

            ColumnLayout {
                id: column3
                anchors.fill: parent
                spacing: 0

                RowDoubleSpinSlider { text: "convexity_min"; from: 0.01; to: 1.0; stepSize: 0.01; value: DataBus.blob_filter_convexity_min
                    valueBind: function() { return DataBus.blob_filter_convexity_min }
                    onValueChanged: DataBus.blob_filter_convexity_min = value
                }
                RowDoubleSpinSlider { text: "convexity_max"; from: 0.01; to: 1.0; stepSize: 0.01; value: DataBus.blob_filter_convexity_max
                    valueBind: function() { return DataBus.blob_filter_convexity_max }
                    onValueChanged: DataBus.blob_filter_convexity_max = value
                }
            }

        }

        // Rectangle {
        //     //color: "#600000F0"
        //     color: "transparent"
        //     height: col3.height

        //     anchors.left: parent.left
        //     anchors.right: parent.right

        //     Behavior on height {
        //         NumberAnimation { duration: 30 }
        //     }

        //     Column {
        //         id: col3

        //         spacing: 0

        //         anchors.top:parent.top
        //         anchors.left: parent.left
        //         anchors.right: parent.right

        //         height: checkbox3.height + pane3.height

        //         CheckBox {
        //             id: checkbox3
        //             height:20
        //             anchors.left: parent.left
        //             anchors.right: parent.right

        //             checked: DataBus.blob_filter_convexity_enabled
        //             onCheckedChanged: DataBus.blob_filter_convexity_enabled = checked
        //             text: qsTr("filter by convexity")
        //         }

        //         Pane {
        //             id: pane3
        //             visible: checkbox3.checked

        //             padding: 0
        //             anchors.left: parent.left
        //             anchors.right: parent.right
        //             anchors.leftMargin: 10
        //             anchors.rightMargin: 10

        //             ColumnLayout {
        //                 id: column3
        //                 anchors.fill: parent
        //                 spacing: 0

        //                 RowDoubleSpinSlider { text: "convexity_min"; from: 0.01; to: 1.0; stepSize: 0.01; value: DataBus.blob_filter_convexity_min
        //                     valueBind: function() { return DataBus.blob_filter_convexity_min }
        //                     onValueChanged: DataBus.blob_filter_convexity_min = value
        //                 }
        //                 RowDoubleSpinSlider { text: "convexity_max"; from: 0.01; to: 1.0; stepSize: 0.01; value: DataBus.blob_filter_convexity_max
        //                     valueBind: function() { return DataBus.blob_filter_convexity_max }
        //                     onValueChanged: DataBus.blob_filter_convexity_max = value
        //                 }
        //             }
        //         }
        //     }
        // }

        RowSpinSlider { text: "thresholdStep"; from: 0; to: 100; stepSize: 1; value: DataBus.blob_thresholdStep
            valueBind: function() { return DataBus.blob_thresholdStep }
            onValueChanged: DataBus.blob_thresholdStep = value
            height: 25

            anchors.left: parent.left
            anchors.right: parent.right
        }
        RowSpinSlider { text: "minThreshold"; from: 0; to: 500; stepSize: 1; value: DataBus.blob_minThreshold
            valueBind: function() { return DataBus.blob_minThreshold }
            onValueChanged: DataBus.blob_minThreshold = value
            height: 25

            anchors.left: parent.left
            anchors.right: parent.right
        }
        RowSpinSlider { text: "maxThreshold"; from: 0; to: 500; stepSize: 1; value: DataBus.blob_maxThreshold
            valueBind: function() { return DataBus.blob_maxThreshold }
            onValueChanged: DataBus.blob_maxThreshold = value
            height: 25

            anchors.left: parent.left
            anchors.right: parent.right
        }
    }
}
