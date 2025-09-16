#!/bin/sh

# Install Linux package prerequisites needed for LVGL development
# and testing. Some less-common development packages are not included
# here, such as MicroPython and PC simulator packages.
#
# Note: This script is run by the CI workflows.
SCRIPT_PATH=$(readlink -f $0)
SCRIPT_DIR=$(dirname $SCRIPT_PATH)

sudo dpkg --add-architecture i386
sudo apt update

cat $SCRIPT_DIR/prerequisites-apt.txt | xargs sudo apt install -y
pip3 install --user -r $SCRIPT_DIR/prerequisites-pip.txt
