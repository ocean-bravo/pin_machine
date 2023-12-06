import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15

Item {
    height: 7*25
    Column {
        anchors.fill: parent

        RowSpinSlider { text: "ad tr block size"; from: 1; to: 201; stepSize: 2; value: DataBus.blob_ad_tr_blockSize
            valueBind: function() { return DataBus.blob_ad_tr_blockSize }
            onValueChanged: DataBus.blob_ad_tr_blockSize = value
        }

        RowDoubleSpinSlider { text: "ad tr c"; from: -100.0; to: 100.0; stepSize: 0.1; value: DataBus.blob_ad_tr_c
            valueBind: function() { return DataBus.blob_ad_tr_c }
            onValueChanged: DataBus.blob_ad_tr_c = value
        }


        RowDoubleSpinSlider { text: "minDia_mm"; from: 0.3; to: 6.0; stepSize: 0.1; value: DataBus.blob_minDia_mm
            valueBind: function() { return DataBus.blob_minDia_mm }
            onValueChanged: DataBus.blob_minDia_mm = value
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
