#!/usr/bin/sh

# Install Linux package prerequisites needed for LVGL development
# and testing. Some less-common development packages are not included
# here, such as MicroPython and PC simulator packages.
#
# Note: This script is run by the CI workflows.
sudo apt install gcc python3 libpng-dev ruby-full gcovr cmake
