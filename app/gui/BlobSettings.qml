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
//        RowSpinSlider { text: "param2"; from: 0; to: 500; stepSize: 1; value: DataBus.param2
//            valueBind: function() { return DataBus.param2 }
//            onValueChanged: DataBus.param2 = value
//        }
//        RowSpinSlider { text: "minRadius"; from: 0; to: 500; stepSize: 1; value: DataBus.minRadius
//            valueBind: function() { return DataBus.minRadius }
//            onValueChanged: DataBus.minRadius = value
//        }
//        RowSpinSlider { text: "maxRadius"; from: 0; to: 500; stepSize: 1; value: DataBus.maxRadius
//            valueBind: function() { return DataBus.maxRadius }
//            onValueChanged: DataBus.maxRadius = value
//        }
    }
}
