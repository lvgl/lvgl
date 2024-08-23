#!/bin/sh

rm -rf astyle
git clone https://github.com/lvgl/astyle.git
cd astyle/build/gcc
make -j
make install
cd ../../..
rm -rf astyle
