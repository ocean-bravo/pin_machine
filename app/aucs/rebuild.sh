#!/bin/bash

git pull

rm -rf build &&  mkdir build && cd build

/home/mint/Qt/5.15.2/gcc_64/bin/qmake .. -spec linux-g++ CONFIG+=debug CONFIG+=qml_debug

make -j3 && make install
