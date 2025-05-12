#!/bin/sh

rm -rf pngquant
git clone https://github.com/kornelski/pngquant
cd pngquant
git checkout 2.17.0
./configure
make -j
sudo make install
cd ..
rm -rf pngquant
