import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import QuickScene 1.0


QuickScene {
    id: root

    /// Debug: Draw filled semi-opaque rectangles around regions
    property bool debug: false

    width: rootState ? rootState.width : 0
    height: rootState ? rootState.height : 0

    // transformOrigin: Item.TopLeft

    // instantiator: RecursiveInstantiator {
    //     id: instantiator

    //     anchors.fill: parent

    //     model: root.model
    //     delegate: SceneItemFactory {
    //         scene: root
    //     }
    // }
}
