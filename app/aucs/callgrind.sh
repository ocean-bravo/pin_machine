#!/bin/bash

scriptDir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"   # the full directory name of the script

echo -e "\e[1;32m"   # begin green text
echo "current script directory: $scriptDir"
echo -e "\e[0m"      # end green text

cd $scriptDir

export QT_LOGGING_RULES="qt.qml.connections=false"

valgrind \
    --tool=callgrind \
    --collect-atstart=no \
    --instr-atstart=no \
    ./pin_machine




