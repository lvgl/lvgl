#!/bin/sh

SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 || exit ; pwd -P )"
sed -n '/LVGL_VERSION_MAJOR/ {N;s@#define \+LVGL_VERSION_M.... \+@@g;s@\n@.@p}' "$SCRIPTPATH/../lv_version.h"
