#!/bin/bash

git clone https://github.com/jeremy-rifkin/cpptrace.git
cd cpptrace
git checkout v0.5.2
cd ..
mkdir cpptrace/build
cd cpptrace/build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j
sudo make install
