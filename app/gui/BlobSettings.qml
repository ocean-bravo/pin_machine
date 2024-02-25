import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15

Rectangle {
    //color: "red"
    //height: 30//lay.height
     // height: 100
     // width: 100


    Layout.preferredHeight: checkbox.checked ? 30 +pane.height + pane2.height  : 30
    Layout.fillWidth: true

    Component.onCompleted: {
        DataBus.blob_ad_tr_enable = false
        DataBus.blob_filter_area_enabled = true
        DataBus.blob_filter_convexity_enabled = false
        DataBus.blob_filter_convexity_min = 0.8
        DataBus.blob_filter_convexity_max = 1.0
    }

    ColumnLayout {
        id: lay
        anchors.fill: parent

        Rectangle {
            color: "lightgrey"
            //color: "transparent"
            Layout.preferredHeight: checkbox.checked ? 20+pane.height : 20
            Layout.fillWidth: true
            Behavior on Layout.preferredHeight {
                NumberAnimation { duration: 30 }
            }

            ColumnLayout {
                spacing: 0
                anchors.fill: parent

                CheckBox {
                    id: checkbox
                    padding: 0
                    Layout.fillWidth: true
                    Layout.preferredHeight: 20
                    topInset: 0
                    bottomInset: 0
                    spacing: 0

                    checked: DataBus.blob_ad_tr_enable
                    onCheckedChanged: DataBus.blob_ad_tr_enable = checked
                    text: qsTr("adaptive threshold")
                }

                Pane {
                    id: pane
                    visible: checkbox.checked
                    Layout.fillWidth: true
                    // padding: 0

                    ColumnLayout {
                        id: column
                        anchors.fill: parent
                        visible: checkbox.checked
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
                            model: ListModel {
                                ListElement { text: "ADAPTIVE_THRESH_MEAN_C";     value: 0 }
                                ListElement { text: "*ADAPTIVE_THRESH_GAUSSIAN_C"; value: 1 }
                            }
                            onActivated: DataBus.blob_ad_tr_type = currentValue
                            MouseArea {
                                anchors.fill: parent
                                onWheel: {
                                    if (wheel.angleDelta.y > 0) {
                                        parent.decrementCurrentIndex()
                                    }
                                    else if (wheel.angleDelta.y < 0) {
                                        parent.incrementCurrentIndex()
                                    }
                                }
                                onPressed: {
                                    // propogate to ComboBox
                                    mouse.accepted = false;
                                }
                                onReleased: {
                                    // propogate to ComboBox
                                    mouse.accepted = false;
                                }
                            }
                        }

                        ComboBox {
                            id: trType
                            valueRole: "value"
                            textRole: "text"
                            Layout.fillWidth: true
                            width: 300
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
                            MouseArea {
                                anchors.fill: parent
                                onWheel: {
                                    if (wheel.angleDelta.y > 0) {
                                        parent.decrementCurrentIndex()
                                    }
                                    else if (wheel.angleDelta.y < 0) {
                                        parent.incrementCurrentIndex()
                                    }
                                }
                                onPressed: {
                                    // propogate to ComboBox
                                    mouse.accepted = false;
                                }
                                onReleased: {
                                    // propogate to ComboBox
                                    mouse.accepted = false;
                                }
                            }
                        }
                    }
                }
            }
        }

        Rectangle {
            color: "lightgrey"
            //color: "transparent"
            Layout.preferredHeight: checkbox2.checked ? 20+pane2.height : 20
            Layout.fillWidth: true
            Behavior on Layout.preferredHeight {
                NumberAnimation { duration: 30 }
            }

            ColumnLayout {
                spacing: 0
                anchors.fill: parent

                CheckBox {
                    id: checkbox2
                    padding: 0
                    Layout.fillWidth: true
                    Layout.preferredHeight: 20
                    topInset: 0
                    bottomInset: 0
                    spacing: 0

                    checked: DataBus.blob_filter_area_enabled
                    onCheckedChanged: DataBus.blob_filter_area_enabled = checked
                    text: qsTr("filter by area")
                }

                Pane {
                    id: pane2
                    visible: checkbox2.checked
                    Layout.fillWidth: true

                    ColumnLayout {
                        id: column2
                        anchors.fill: parent
                        visible: checkbox2.checked
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
            }
        }

        Rectangle {
            color: "lightgrey"
            //color: "transparent"
            Layout.preferredHeight: checkbox3.checked ? 20+pane3.height : 20
            Layout.fillWidth: true
            Behavior on Layout.preferredHeight {
                NumberAnimation { duration: 30 }
            }

            ColumnLayout {
                spacing: 0
                anchors.fill: parent

                CheckBox {
                    id: checkbox3
                    padding: 0
                    Layout.fillWidth: true
                    Layout.preferredHeight: 20
                    topInset: 0
                    bottomInset: 0
                    spacing: 0

                    checked: DataBus.blob_filter_convexity_enabled
                    onCheckedChanged: DataBus.blob_filter_convexity_enabled = checked
                    text: qsTr("filter by convexity")
                }

                Pane {
                    id: pane3
                    visible: checkbox3.checked
                    Layout.fillWidth: true

                    ColumnLayout {
                        id: column3
                        anchors.fill: parent
                        visible: checkbox3.checked
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
            }
        }



        RowSpinSlider { text: "thresholdStep"; from: 0; to: 100; stepSize: 1; value: DataBus.blob_thresholdStep
            valueBind: function() { return DataBus.blob_thresholdStep }
            onValueChanged: DataBus.blob_thresholdStep = value
        }
        RowSpinSlider { text: "minThreshold"; from: 0; to: 500; stepSize: 1; value: DataBus.blob_minThreshold
            valueBind: function() { return DataBus.blob_minThreshold }
            onValueChanged: DataBus.blob_minThreshold = value
        }
        RowSpinSlider { text: "maxThreshold"; from: 0; to: 500; stepSize: 1; value: DataBus.blob_maxThreshold
            valueBind: function() { return DataBus.blob_maxThreshold }
            onValueChanged: DataBus.blob_maxThreshold = value
        }
    }
}
