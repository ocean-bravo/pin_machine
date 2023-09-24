import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15

Item {
    height: 5*25
    Column {
        anchors.fill: parent

        RowSpinSlider { text: "minArea"; from: 0; to: 100000; stepSize: 1; value: DataBus.blob_minArea
            valueBind: function() { return DataBus.blob_minArea }
            onValueChanged: DataBus.blob_minArea = value
        }
        RowSpinSlider { text: "maxArea"; from: 0; to: 100000; stepSize: 1; value: DataBus.blob_maxArea
            valueBind: function() { return DataBus.blob_maxArea }
            onValueChanged: DataBus.blob_maxArea = value
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
