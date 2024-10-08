#!/bin/sh

# Install Linux package prerequisites needed for LVGL development
# and testing. Some less-common development packages are not included
# here, such as MicroPython and PC simulator packages.
#
# Note: This script is run by the CI workflows.
sudo dpkg --add-architecture i386
sudo apt update
sudo apt install gcc gcc-multilib g++-multilib ninja-build \
    libpng-dev libjpeg-turbo8-dev libfreetype6-dev \
    libglew-dev libglfw3-dev libsdl2-dev \
    libpng-dev:i386 libjpeg-dev:i386 libfreetype6-dev:i386 \
    ruby-full gcovr cmake  python3 pngquant libinput-dev libxkbcommon-dev \
    libdrm-dev pkg-config wayland-protocols libwayland-dev libwayland-bin \
    libwayland-dev:i386 libxkbcommon-dev:i386
pip3 install pypng lz4 kconfiglib
