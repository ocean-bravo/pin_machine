import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

CollapsiblePanel {
    id: root

    width: parent.width
    height: checked ? 30  + rect.height: 30

    Layout.preferredWidth: 400
    Layout.preferredHeight: checked ? 30  + rect.height: 30

    checked: false

    text: qsTr("Preview blob detection settings")

    onCheckedChanged: {
        rect.visible = checked
    }
    Component.onCompleted: {
        rect.visible = checked
    }

    Item {
        id: rect
        //color: "#400000FF"

        height: blobSettings.height

        anchors.top:parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins:0

        // DebugLoader {
        //     id: blobSettings
        //     anchors.left: parent.left
        //     anchors.right: parent.right
        //     anchors.top:parent.top
        //     path: "/home/mint/devel/pin_machine/app/gui/BlobSettings.qml"
        // }

        BlobSettings2 {
            id: blobSettings
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top:parent.top

            onDataChanged: {
                DataBus.blob_live_options = options
            }

            Component.onCompleted: {
                var defaultOptions = {}

                defaultOptions["blob_filter_area_enabled"] = true
                defaultOptions["blob_minDia_mm"] = 0.7
                defaultOptions["blob_maxDia_mm"] = 1.4

                defaultOptions["blob_filter_circularity_enabled"] = true
                defaultOptions["blob_filter_circularity_min"] =  0.5
                defaultOptions["blob_filter_circularity_max"] = 5.0

                defaultOptions["blob_filter_convexity_enabled"] = false
                defaultOptions["blob_filter_convexity_min"] =  0.5
                defaultOptions["blob_filter_convexity_max"] = 0.9

                defaultOptions["blob_thresholdStep"] = 10
                defaultOptions["blob_minThreshold"] = 1
                defaultOptions["blob_maxThreshold"] = 200

                defaultOptions["blob_ad_tr_enable"] = true
                defaultOptions["blob_ad_tr_blockSize"] = 29
                defaultOptions["blob_ad_tr_c"] = 9.0
                defaultOptions["blob_ad_tr_type"] = 1
                defaultOptions["blob_tr_type"] = 0

                options = defaultOptions
            }
        }
    }
}
