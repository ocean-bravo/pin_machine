import QtQml 2.12
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12


Button {
    id: root
    width: 120
    height: 30

    property string tooltipText

    ToolTip.delay: 1000
    ToolTip.timeout: 5000
    ToolTip.visible: hovered && tooltipText
    ToolTip.text: root.tooltipText

    Layout.preferredWidth: 120
    Layout.preferredHeight: 30
}
