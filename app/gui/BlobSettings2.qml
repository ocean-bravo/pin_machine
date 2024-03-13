import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15

Rectangle {
    //color: "#4000FF00"
    color: "transparent"
    height: lay.height

    property var options

    onOptionsChanged:  {

    }

    signal dataChanged

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

    Column {
        id: lay
        anchors.top:parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: col1.height + col2.height + col3.height + col4.height + 150

        SmallCollapsiblePanel {
            id: col1
            text: qsTr("adaptive threshold")

            checked: options.blob_ad_tr_enable
            onCheckedChanged: {
                options.blob_ad_tr_enable = checked
                dataChanged()
            }
            height: checked ? column1.height + 30 : 30

            ColumnLayout {
                id: column1
                anchors.top:parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                spacing: 0

                RowSpinSlider { text: "block size"; from: 3; to: 2001; stepSize: 2; value: options.blob_ad_tr_blockSize
                    valueBind: function() { return options.blob_ad_tr_blockSize }
                    onValueChanged: {
                        options.blob_ad_tr_blockSize = value
                        dataChanged()
                    }
                }

                RowDoubleSpinSlider { text: "C"; from: -100.0; to: 100.0; stepSize: 0.1; value: options.blob_ad_tr_c
                    valueBind: function() { return options.blob_ad_tr_c }
                    onValueChanged: {
                        options.blob_ad_tr_c = value
                        dataChanged()
                    }
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
                    onActivated: {
                        options.blob_ad_tr_type = currentValue
                        dataChanged()
                    }
                    Component.onCompleted: {
                        currentIndex = indexOfValue(options.blob_ad_tr_type)
                    }
                }

                ComboBox {
                    id: trType
                    valueRole: "value"
                    textRole: "text"
                    Layout.fillWidth: true
                    Layout.preferredHeight: 25
                    //width: 300
                    model: ListModel {
                        ListElement { text: "*THRESH_BINARY";    value: 0 }
                        ListElement { text: "THRESH_BINARY_INV"; value: 1 }
                        ListElement { text: "THRESH_TRUNC";      value: 2 }
                        ListElement { text: "THRESH_TOZERO";     value: 3 }
                        ListElement { text: "THRESH_TOZERO_INV"; value: 4 }
                        ListElement { text: "THRESH_MASK";       value: 7 }
                        ListElement { text: "THRESH_OTSU";       value: 8 }
                        ListElement { text: "THRESH_TRIANGLE";   value: 16 }
                    }

                    onActivated: {
                        options.blob_tr_type = currentValue
                        dataChanged()
                    }
                    Component.onCompleted: {
                        currentIndex = indexOfValue(options.blob_tr_type)
                    }

                }
            }
        }

        SmallCollapsiblePanel {
            id: col2

            text: qsTr("filter by area")

            checked: options.blob_filter_area_enabled
            onCheckedChanged: {
                options.blob_filter_area_enabled = checked
                dataChanged()
            }

            height: checked ? column2.height + 30 : 30

            ColumnLayout {
                id: column2
                anchors.top:parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                spacing: 0

                RowDoubleSpinSlider { text: "minDia_mm"; from: 0.3; to: 6.0; stepSize: 0.1; value: options.blob_minDia_mm
                    valueBind: function() { return options.blob_minDia_mm }
                    onValueChanged: {
                        options.blob_minDia_mm = value
                        dataChanged()
                    }
                }
                RowDoubleSpinSlider { text: "maxDia_mm"; from: 0.3; to: 6.0; stepSize: 0.1; value: options.blob_maxDia_mm
                    valueBind: function() { return options.blob_maxDia_mm }
                    onValueChanged: {
                        options.blob_maxDia_mm = value
                        dataChanged()
                    }
                }
            }
        }

        SmallCollapsiblePanel {
            id: col3

            text: qsTr("filter by convexity")

            checked: options.blob_filter_convexity_enabled
            onCheckedChanged: {
                options.blob_filter_convexity_enabled = checked
                dataChanged()
            }

            height: checked ? column3.height + 30 : 30

            ColumnLayout {
                id: column3
                anchors.top:parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                spacing: 0

                RowDoubleSpinSlider { text: "convexity_min"; from: 0.01; to: 1.0; stepSize: 0.01; value: options.blob_filter_convexity_min
                    valueBind: function() { return options.blob_filter_convexity_min }
                    onValueChanged: {
                        options.blob_filter_convexity_min = value
                        dataChanged()
                    }
                }
                RowDoubleSpinSlider { text: "convexity_max"; from: 0.01; to: 1.0; stepSize: 0.01; value: options.blob_filter_convexity_max
                    valueBind: function() { return options.blob_filter_convexity_max }
                    onValueChanged: {
                        options.blob_filter_convexity_max = value
                        dataChanged()
                    }
                }
            }
        }

        SmallCollapsiblePanel {
            id: col4

            text: qsTr("filter by circularity")

            checked: options.blob_filter_circularity_enabled
            onCheckedChanged: {
                options.blob_filter_circularity_enabled = checked
                dataChanged()
            }

            height: checked ? column4.height + 30 : 30

            ColumnLayout {
                id: column4
                anchors.top:parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                spacing: 0

                RowDoubleSpinSlider { text: "circularity_min"; from: 0.01; to: 10.0; stepSize: 0.01; value: options.blob_filter_circularity_min
                    valueBind: function() { return options.blob_filter_circularity_min }
                    onValueChanged: {
                        options.blob_filter_circularity_min = value
                        dataChanged()
                    }
                }
                RowDoubleSpinSlider { text: "circularity_max"; from: 0.01; to: 10.0; stepSize: 0.01; value: options.blob_filter_circularity_max
                    valueBind: function() { return options.blob_filter_circularity_max }
                    onValueChanged: {
                        options.blob_filter_circularity_max = value
                        dataChanged()
                    }
                }
            }
        }


        RowSpinSlider { text: "thresholdStep"; from: 0; to: 100; stepSize: 1; value: options.blob_thresholdStep
            valueBind: function() { return options.blob_thresholdStep }
            onValueChanged: {
                options.blob_thresholdStep = value
                dataChanged()
            }
            height: 25

            anchors.left: parent.left
            anchors.right: parent.right
        }
        RowSpinSlider { text: "minThreshold"; from: 0; to: 500; stepSize: 1; value: options.blob_minThreshold
            valueBind: function() { return options.blob_minThreshold }
            onValueChanged: {
                options.blob_minThreshold = value
                dataChanged()
            }
            height: 25

            anchors.left: parent.left
            anchors.right: parent.right
        }
        RowSpinSlider { text: "maxThreshold"; from: 0; to: 500; stepSize: 1; value: options.blob_maxThreshold
            valueBind: function() { return options.blob_maxThreshold }
            onValueChanged: {
                options.blob_maxThreshold = value
                dataChanged()
            }
            height: 25

            anchors.left: parent.left
            anchors.right: parent.right
        }
    }
}
