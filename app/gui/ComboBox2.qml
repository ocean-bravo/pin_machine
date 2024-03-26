import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15


TextField {
    id: root
    property real value: NaN  // Основное значение.
    property int decimals: 1  // Количество знаков после запятой.
    property real topValue    // Верхняя граница допустимого диапазона значений.
    property real bottomValue // Нижняя граница допустимого диапазона значений.
    property bool initialized: false

    Component.onCompleted: {
        fixupInputMask();
        initialized = true;
        updateText();
    }

    validator: DoubleValidator {
        decimals: root.decimals
        top: topValue
        bottom: bottomValue
        notation: DoubleValidator.StandardNotation
    }

    onValueChanged: {
        if (!initialized)
            return;
        updateText();
    }

    onDisplayTextChanged: {
        if (!initialized||valueInputLocker.locked)
            return;

        valueInputLocker.locked = true;

        try {
            value = parseFloat(Number.fromLocaleString(Qt.locale(), displayText));
        }
        catch(error) {
            // Штатная ситуация, например ввели строку вида "-".
            // Является валидной при начале ввода отрицательного числа, но не является числом.
            return;
        }
        finally {
            valueInputLocker.locked = false;
        }
    }

    // Для облегчения ввода с английской раскладки на NumPad клавиатуре.
    Keys.onPressed: {
        if (event.text === "." || event.text === ",") {
            insert(cursorPosition, Qt.locale().decimalPoint);
            event.accepted = true;
        }
    }

    QtObject {
        id: valueInputLocker
        property bool locked: false
    }

    function updateText() {
        if (valueInputLocker.locked)
            return;

        valueInputLocker.locked = true;
        if (inputMask !== "" && inputMask !== undefined) {
            var sourceString = Number(value).toLocaleString(Qt.locale(), "f", decimals);
            var sourceStringArray = sourceString.split(Qt.locale().decimalPoint);

            var mask = inputMask.split(";");
            var integerPart = mask[0].split(Qt.locale().decimalPoint);

            text = zeroPad(sourceStringArray[0], integerPart[0].length) + Qt.locale().decimalPoint + sourceStringArray[1];
        }
        else {
            text = Number(value).toLocaleString(Qt.locale(), "f", decimals);
        }
        valueInputLocker.locked = false;
    }

    /// Заменяет в маске разделитель целой и дробной части на разделитель локали
    function fixupInputMask() {
        if (Qt.locale().decimalPoint === ".")
            inputMask = inputMask.replace(",", Qt.locale().decimalPoint);
        if (Qt.locale().decimalPoint === ",")
            inputMask = inputMask.replace(".", Qt.locale().decimalPoint);
    }

    /// Возвращает маску с разделителем из локали
    function craeteInputMask(base, fraction, value) {
        return base + Qt.locale().decimalPoint + fraction + ";" + value;
    }

    /// Добавление незначащих нулей (num - число, places - количество разрядов)
    function zeroPad(num, places) {
        var zero = places - num.toString().length + 1;
        return Array(+(zero > 0 && zero)).join("0") + num;
    }
}

