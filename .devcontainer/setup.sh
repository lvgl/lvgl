#!/bin/sh

echo ". /usr/local/emsdk/emsdk_env.sh" >> /home/codespace/.bashrc

cd /workspace/lvgl_app
sudo chmod 777 .
mkdir build
mkdir vscode

cd lvgl/.devcontainer
cp __CMakeLists.txt__  ../../CMakeLists.txt
cp __main.c__ ../../main.c
cp __build_all.sh__ ../../build_all.sh
cp __c_cpp_properties.json__ ../../.vscode/c_cpp_properties.json
cp __settings.json__ ../../.vscode/settings.json
touch ../../lv_conf.h
../scripts/generate_lv_conf.py --template ../lv_conf_template.h --config ../../lv_conf.h

chmod +x ../../build_all.sh