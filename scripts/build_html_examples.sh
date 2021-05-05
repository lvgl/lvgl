#!/bin/bash
set -e
CURRENT_REF="$(git rev-parse HEAD)"
rm -rf emscripten_builder
git clone https://github.com/lvgl/lv_sim_emscripten.git emscripten_builder
scripts/genexamplelist.sh > emscripten_builder/examplelist.c
cd emscripten_builder
git submodule update --init -- lvgl
cd lvgl
git checkout $CURRENT_REF
cd ..
git submodule update --init -- lv_drivers
mkdir cmbuild
cd cmbuild
emcmake cmake .. -DLVGL_CHOSEN_DEMO=lv_example_noop
make -j$(nproc)
rm -rf CMakeFiles
cd ../..
cp -a emscripten_builder/cmbuild docs/_static/built_lv_examples
