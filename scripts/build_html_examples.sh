#!/bin/bash

set -e

# These variables allow us to specify an alternate repository URL and commit reference
# This is particularly useful when running in CI environments for pull requests
# where we need to build from the contributor's forked repository
REPO_URL="${1:-}"
COMMIT_REF="${2:-}"

export PATH="/usr/lib/ccache:/usr/local/opt/ccache/libexec:$PATH"
rm -rf emscripten_builder
git clone https://github.com/lvgl/lv_sim_emscripten.git emscripten_builder
scripts/genexamplelist.sh > emscripten_builder/examplelist.c
cd emscripten_builder
git submodule update --init -- lvgl
cd lvgl
if [ -n "$REPO_URL" ] && [ -n "$COMMIT_REF" ]; then
  echo "Using provided repo URL: $REPO_URL and commit ref: $COMMIT_REF for lvgl submodule"
  git remote set-url origin "$REPO_URL"
  git fetch origin
  git checkout "$COMMIT_REF"
else
  CURRENT_REF="$(git rev-parse HEAD)"
  echo "Using current commit ref: $CURRENT_REF for lvgl"
  git checkout "$CURRENT_REF"
fi
cd ..
mkdir cmbuild
cd cmbuild
emcmake cmake .. -DLVGL_CHOSEN_DEMO=lv_example_noop -DCMAKE_C_COMPILER_LAUNCHER=ccache -DCMAKE_CXX_COMPILER_LAUNCHER=ccache
emmake make -j$(nproc)
rm -rf CMakeFiles
cd ../..
cp -a emscripten_builder/cmbuild docs/src/_static/built_lv_examples
