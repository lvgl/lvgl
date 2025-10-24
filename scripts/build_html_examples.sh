#!/bin/bash

set -e

# These variables allow us to specify an alternate repository URL and commit reference
# This is particularly useful when running in CI environments for pull requests
# where we need to build from the contributor's forked repository
ARG_1="${1:-}"
ARG_2="${2:-}"

export PATH="/usr/lib/ccache:/usr/local/opt/ccache/libexec:$PATH"
rm -rf emscripten_builder
git clone https://github.com/lvgl/lv_sim_emscripten.git emscripten_builder
cd emscripten_builder
if [ "$ARG_1" != "--symlink" ]; then
  REPO_URL="$ARG_1"
  COMMIT_REF="$ARG_2"
  git submodule update --init -- lvgl
  if [ -n "$REPO_URL" ] && [ -n "$COMMIT_REF" ]; then
    cd lvgl
    echo "Using provided repo URL: $REPO_URL and commit ref: $COMMIT_REF for lvgl submodule"
    git remote set-url origin "$REPO_URL"
    git fetch origin
    git checkout "$COMMIT_REF"
    cd ..
  fi
else
  SYMLINK_TARGET="$ARG_2"
  echo "Using provided symbolic link to LVGL directory (should be absolute): $SYMLINK_TARGET"
  rmdir lvgl # remove the uninitialized submodule empty dir
  ln -s -T "$SYMLINK_TARGET" lvgl
fi

# Grab the path to emscripten's port examplelist.c before changing to LVGL's directory
EXAMPLE_LIST_C=$(pwd)/examplelist.c
cd lvgl
scripts/genexamplelist.sh > $EXAMPLE_LIST_C
cd ..

# Generate lv_conf
LV_CONF_PATH=`pwd`/lvgl/configs/ci/docs/lv_conf_docs.h

python ./lvgl/scripts/generate_lv_conf.py \
  --template lvgl/lv_conf_template.h \
  --config $LV_CONF_PATH \
  --defaults lvgl/configs/ci/docs/lv_conf_docs.defaults

mkdir cmbuild
cd cmbuild
emcmake cmake .. -DLV_BUILD_CONF_PATH=$LV_CONF_PATH -DLVGL_CHOSEN_DEMO=lv_example_noop -DCMAKE_C_COMPILER_LAUNCHER=ccache -DCMAKE_CXX_COMPILER_LAUNCHER=ccache
emmake make -j$(nproc)
rm -rf CMakeFiles
cd ..
if [ "$ARG_1" != "--symlink" ]; then
  cp -a cmbuild ../docs/src/_static/built_lv_examples
else
  cp -a cmbuild lvgl/docs/src/_static/built_lv_examples
fi
cd ..
