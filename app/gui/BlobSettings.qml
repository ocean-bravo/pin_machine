import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15

Item {
    height: 9*25

    Component.onCompleted: {
        DataBus.blob_ad_tr_enable = false
    }

    ColumnLayout {
        anchors.fill: parent

        GroupBox {
            Layout.preferredHeight: checkbox.checked ? 200 : 30
            Layout.fillWidth: true
            padding: 0

            label: CheckBox {
                id: checkbox
                checked: DataBus.blob_ad_tr_enable
                onCheckedChanged: DataBus.blob_ad_tr_enable = checked
                text: qsTr("adaptive threshold")
            }

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

        // CheckBox {
        //     checked: DataBus.blob_ad_tr_enable
        //     onCheckedChanged: DataBus.blob_ad_tr_enable = checked
        //     text: qsTr("adaptive threshold")
        // }



        RowDoubleSpinSlider { text: "minDia_mm"; from: 0.3; to: 6.0; stepSize: 0.1; value: DataBus.blob_minDia_mm
            valueBind: function() { return DataBus.blob_minDia_mm }
            onValueChanged: DataBus.blob_minDia_mm = value
            //Layout.fillWidth: true
        }
        RowDoubleSpinSlider { text: "maxDia_mm"; from: 0.3; to: 6.0; stepSize: 0.1; value: DataBus.blob_maxDia_mm
            valueBind: function() { return DataBus.blob_maxDia_mm }
            onValueChanged: DataBus.blob_maxDia_mm = value
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
