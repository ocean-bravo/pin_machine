import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15

Item {
    height: 5*25
    Column {
        anchors.fill: parent

        RowSpinSlider { text: "minDist"; from: 0; to: 500; stepSize: 1; value: DataBus.circle_minDist
            valueBind: function() { return DataBus.circle_minDist }
            onValueChanged: DataBus.circle_minDist = value
        }
        RowSpinSlider { text: "param1"; from: 0; to: 500; stepSize: 1; value: DataBus.circle_param1
            valueBind: function() { return DataBus.circle_param1 }
            onValueChanged: DataBus.circle_param1 = value
        }
        RowSpinSlider { text: "param2"; from: 0; to: 500; stepSize: 1; value: DataBus.circle_param2
            valueBind: function() { return DataBus.circle_param2 }
            onValueChanged: DataBus.circle_param2 = value
        }
        RowSpinSlider { text: "minRadius"; from: 0; to: 500; stepSize: 1; value: DataBus.circle_minRadius
            valueBind: function() { return DataBus.circle_minRadius }
            onValueChanged: DataBus.circle_minRadius = value
        }
        RowSpinSlider { text: "maxRadius"; from: 0; to: 500; stepSize: 1; value: DataBus.circle_maxRadius
            valueBind: function() { return DataBus.circle_maxRadius }
            onValueChanged: DataBus.circle_maxRadius = value
        }
    }
}
