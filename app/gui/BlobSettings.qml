import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15

Item {
    height: 5*25
    Column {
        anchors.fill: parent

        RowDoubleSpinSlider { text: "minDia"; from: 0.3; to: 6.0; stepSize: 0.1; value: DataBus.blob_minDia
            valueBind: function() { return DataBus.blob_minDia }
            onValueChanged: DataBus.blob_minDia = value
        }
        RowDoubleSpinSlider { text: "maxDia"; from: 0.3; to: 6.0; stepSize: 0.1; value: DataBus.blob_maxDia
            valueBind: function() { return DataBus.blob_maxDia }
            onValueChanged: DataBus.blob_maxDia = value
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
