import QtQml 2.15
import QtQuick 2.15

// Еле получилось по образу других компонентов. Не совсем понимаю, как это работает.
// Не проверял, нормально ли аборт срабатывает.

Component {
    Timer {
        property var _resolve
        property var _reject
        property var _abort
        property bool _aborted: false
        property bool _aborting: false

        property var signal

        running: true
        repeat: false

        function finish() {
            if (_aborted) {
                return;
            }

            signal.disconnect(slotConnection)
            stop();
            _resolve();

            Qt.callLater(destroy);
        }

        property var slotConnection

        function connectOnce(sig, slot) {
            var slotConnection = () => {
                slot.apply(this, arguments);    // run slot by passing `this` and arguments (as list).
                sig.disconnect(slotConnection); // disconnect slot :)
            }
            sig.connect(slotConnection);        // connecting slot to signal in Qt way
            return slotConnection
        }

        onSignalChanged: {
            slotConnection = connectOnce(signal, finish)
        }

        onTriggered: {
            console.log("wait for signal timeout!!!")
            finish()
        }

        on_AbortingChanged: {
            if (_aborting) {
                if (running) {
                    signal.disconnect(slotConnection)
                    stop();
                    _abort(_reject);
                    Qt.callLater(destroy);
                }
            }
        }
    }



//    QtObject {
//        id: root


//        property int timeout
//        property var signal

//        Timer {
//            running: true
//            repeat: false
//            interval: root.timeout
//            onTriggered: finish()
//        }

//        function connectOnce(sig, slot) {
//            var slotConnection = () => {
//                slot.apply(this, arguments);    // run slot by passing `this` and arguments (as list).
//                sig.disconnect(slotConnection); // disconnect slot :)
//            }
//            sig.connect(slotConnection);        // connecting slot to signal in Qt way
//        }



//        onSignalChanged: {
//            connectOnce(signal, finish)
//        }

//        function on_AbortingChanged() {
//            if (_aborting) {
//                _abort(_reject);
//                Qt.callLater(destroy);
//            }
//        }
//    }
}
