#!/bin/bash
set -e
export PATH="/usr/lib/ccache:/usr/local/opt/ccache/libexec:$PATH"
CURRENT_REF="$(git rev-parse HEAD)"
rm -rf emscripten_builder
git clone https://github.com/lvgl/lv_sim_emscripten.git emscripten_builder
scripts/genexamplelist.sh > emscripten_builder/examplelist.c
cd emscripten_builder
git submodule update --init -- lvgl
cd lvgl
git checkout $CURRENT_REF
cd ..
mkdir cmbuild
cd cmbuild
emcmake cmake .. -DLVGL_CHOSEN_DEMO=lv_example_noop -DCMAKE_C_COMPILER_LAUNCHER=ccache -DCMAKE_CXX_COMPILER_LAUNCHER=ccache
emmake make -j$(nproc)
rm -rf CMakeFiles
cd ../..
cp -a emscripten_builder/cmbuild docs/_static/built_lv_examples
