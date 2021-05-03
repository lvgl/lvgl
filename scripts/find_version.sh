#!/bin/bash
# Credit: https://stackoverflow.com/a/4774063
SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
TMPENVFILE=$(mktemp /tmp/lvgl.script.XXXXXX)
cat $SCRIPTPATH/../lvgl.h | grep "#define LVGL_VERSION_" | sed 's/#define //g' | sed -r 's/\s+/=/' > $TMPENVFILE
. $TMPENVFILE
rm $TMPENVFILE
echo $LVGL_VERSION_MAJOR.$LVGL_VERSION_MINOR
