#!/bin/sh

# Check if the script is being run as root
if [ "$(id -u)" -ne 0 ]; then
  echo "This script must be run as root or with sudo" >&2
  exit 1
fi

rm -rf astyle
git clone https://github.com/lvgl/astyle.git
cd astyle/build/gcc
git checkout v3.4.12
make -j
make install
cd ../../..
rm -rf astyle
