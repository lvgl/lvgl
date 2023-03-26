#!/bin/sh

echo ". /usr/local/emsdk/emsdk_env.sh" >> /home/codespace/.bashrc

cd /workspace/lvgl_app
sudo chmod 777 .
mkdir build

cd lvgl/.devcontainer
cp __CMakeLists.txt__  ../../CMakeLists.txt
cp __lv_conf.h__       ../../lv_conf.h
cp __main.c__ ../../main.c
cp __build_all.sh__ ../../build_all.sh

chmod +x ../../build_all.sh