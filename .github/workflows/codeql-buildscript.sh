#!/usr/bin/env bash

#./scripts/install-prerequisites.sh
sudo apt-get -y update
sudo apt-get -y install gcc python3 libpng-dev ruby-full gcovr cmake
python3 tests/main.py --build-option=OPTIONS_NORMAL_8BIT build
