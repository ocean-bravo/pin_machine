#!/bin/bash

scriptDir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"   # the full directory name of the script

echo -e "\e[1;32m"   # begin green text
echo "current script directory: $scriptDir"
echo -e "\e[0m"      # end green text

cd $scriptDir

export QT_LOGGING_RULES="qt.qml.connections=false"
export QML_IMPORT_TRACE=1
#export QT_DEBUG_PLUGINS=1

./pin_machine
