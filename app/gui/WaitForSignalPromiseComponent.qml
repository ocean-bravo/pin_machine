import QtQml 2.15
import QtQuick 2.15

// Еле получилось по образу других компонентов. Не совсем понимаю, как это работает.
// Не проверял, нормально ли аборт срабатывает.

Component {
    QtObject {
        id: root
        property var _resolve
        property var _reject
        property var _abort
        property bool _aborted: false
        property bool _aborting: false

        property var signal

        function connectOnce(sig, slot) {
            var slotConnection = () => {
                slot.apply(this, arguments);    // run slot by passing `this` and arguments (as list).
                sig.disconnect(slotConnection); // disconnect slot :)
            }
            sig.connect(slotConnection);        // connecting slot to signal in Qt way
        }

        onSignalChanged: {
            connectOnce(signal, function () {
                if (_aborted) {
                    return;
                }

                _resolve();

                Qt.callLater(destroy);
            })
        }

        function on_AbortingChanged() {
            if (_aborting) {
                _abort(_reject);
                Qt.callLater(destroy);
            }
        }
    }
}
