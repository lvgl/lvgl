#!/bin/sh

# Install Linux package prerequisites needed for LVGL development
# and testing. Some less-common development packages are not included
# here, such as MicroPython and PC simulator packages.
#
# Note: This script is run by the CI workflows.
sudo apt update
sudo apt install gcc python3 libpng-dev ruby-full gcovr cmake libjpeg-turbo8-dev libfreetype6-dev pngquant libinput-dev libxkbcommon-dev pkg-config
pip3 install pypng lz4
