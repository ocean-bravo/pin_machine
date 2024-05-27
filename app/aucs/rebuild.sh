#!/bin/bash

cd /home/mint/deploy/pin_machine
mv --force settings.ini settings2.ini


cd /home/mint/devel/pin_machine

git pull

rm -rf build &&  mkdir build && cd build

/home/mint/Qt/5.15.2/gcc_64/bin/qmake .. -spec linux-g++ CONFIG+=debug CONFIG+=qml_debug

make -j3 && make install

cd /home/mint/deploy/pin_machine


cd /home/mint/deploy/pin_machine
mv --force settings2.ini settings.ini
