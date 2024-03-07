import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15

Rectangle {
    //color: "#4000FF00"
    color: "transparent"
    height: lay.height


    property var blobLiveOptions: DataBus.blob_live_options
    onBlobLiveOptionsChanged: {
        console.log("blob live settings updated")
        DataBus.blob_live_options = blobLiveOptions
    }

    component SmallCollapsiblePanel : CollapsiblePanel {
        height: checked ? 30 + implicitContentHeight : 30
        anchors.left: lay.left
        anchors.right: lay.right
        onCheckedChanged: contentItem.visible = checked
        Component.onCompleted: contentItem.visible = checked

        property alias checked: checkBox.checked
        property alias text: checkBox.text
        label: CheckBox { id: checkBox }
    }

    Component.onCompleted: {
        blobLiveOptions.blob_ad_tr_enable = false
        blobLiveOptions.blob_filter_area_enabled = true
        blobLiveOptions.blob_filter_convexity_enabled = false
        blobLiveOptions.blob_filter_convexity_min = 0.8
        blobLiveOptions.blob_filter_convexity_max = 1.0
    }

    Column {
        id: lay
        anchors.top:parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: col1.height + col2.height + col3.height + 75

        SmallCollapsiblePanel {
            id: col1
            text: qsTr("adaptive threshold")

            checked: blobLiveOptions.blob_ad_tr_enable
            onCheckedChanged: blobLiveOptions.blob_ad_tr_enable = checked

            ColumnLayout {
                id: column1
                anchors.top:parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                spacing: 0

                RowSpinSlider { text: "block size"; from: 3; to: 2001; stepSize: 2; value: blobLiveOptions.blob_ad_tr_blockSize
                    valueBind: function() { return blobLiveOptions.blob_ad_tr_blockSize }
                    onValueChanged: blobLiveOptions.blob_ad_tr_blockSize = value
                }

                RowDoubleSpinSlider { text: "C"; from: -100.0; to: 100.0; stepSize: 0.1; value: blobLiveOptions.blob_ad_tr_c
                    valueBind: function() { return blobLiveOptions.blob_ad_tr_c }
                    onValueChanged: blobLiveOptions.blob_ad_tr_c = value
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
                    onActivated: blobLiveOptions.blob_ad_tr_type = currentValue
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

                    onActivated: blobLiveOptions.blob_tr_type = currentValue
                    //Component.onCompleted: DataBus.mode = "raw"
                }
            }
        }

        SmallCollapsiblePanel {
            id: col2

            text: qsTr("filter by area")

            checked: blobLiveOptions.blob_filter_area_enabled
            onCheckedChanged: blobLiveOptions.blob_filter_area_enabled = checked

            ColumnLayout {
                id: column2
                anchors.top:parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                spacing: 0

                RowDoubleSpinSlider { text: "minDia_mm"; from: 0.3; to: 6.0; stepSize: 0.1; value: blobLiveOptions.blob_minDia_mm
                    valueBind: function() { return blobLiveOptions.blob_minDia_mm }
                    onValueChanged: blobLiveOptions.blob_minDia_mm = value
                }
                RowDoubleSpinSlider { text: "maxDia_mm"; from: 0.3; to: 6.0; stepSize: 0.1; value: blobLiveOptions.blob_maxDia_mm
                    valueBind: function() { return blobLiveOptions.blob_maxDia_mm }
                    onValueChanged: blobLiveOptions.blob_maxDia_mm = value
                }
            }
        }

        SmallCollapsiblePanel {
            id: col3

            text: qsTr("filter by convexity")

            checked: blobLiveOptions.blob_filter_convexity_enabled
            onCheckedChanged: blobLiveOptions.blob_filter_convexity_enabled = checked

            ColumnLayout {
                id: column3
                anchors.top:parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                spacing: 0

                RowDoubleSpinSlider { text: "convexity_min"; from: 0.01; to: 1.0; stepSize: 0.01; value: blobLiveOptions.blob_filter_convexity_min
                    valueBind: function() { return blobLiveOptions.blob_filter_convexity_min }
                    onValueChanged: blobLiveOptions.blob_filter_convexity_min = value
                }
                RowDoubleSpinSlider { text: "convexity_max"; from: 0.01; to: 1.0; stepSize: 0.01; value: blobLiveOptions.blob_filter_convexity_max
                    valueBind: function() { return blobLiveOptions.blob_filter_convexity_max }
                    onValueChanged: blobLiveOptions.blob_filter_convexity_max = value
                }
            }
        }


        RowSpinSlider { text: "thresholdStep"; from: 0; to: 100; stepSize: 1; value: blobLiveOptions.blob_thresholdStep
            valueBind: function() { return blobLiveOptions.blob_thresholdStep }
            onValueChanged: blobLiveOptions.blob_thresholdStep = value
            height: 25

            anchors.left: parent.left
            anchors.right: parent.right
        }
        RowSpinSlider { text: "minThreshold"; from: 0; to: 500; stepSize: 1; value: blobLiveOptions.blob_minThreshold
            valueBind: function() { return blobLiveOptions.blob_minThreshold }
            onValueChanged: blobLiveOptions.blob_minThreshold = value
            height: 25

            anchors.left: parent.left
            anchors.right: parent.right
        }
        RowSpinSlider { text: "maxThreshold"; from: 0; to: 500; stepSize: 1; value: blobLiveOptions.blob_maxThreshold
            valueBind: function() { return blobLiveOptions.blob_maxThreshold }
            onValueChanged: blobLiveOptions.blob_maxThreshold = value
            height: 25

            anchors.left: parent.left
            anchors.right: parent.right
        }
    }
}
