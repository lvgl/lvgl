#!/bin/bash

#Add LVGL #if LV_USE_VG_LITE_DRIVER guard
#Usage
#   find -name "*.c" | xargs ./add_lvgl_if.sh
#   find -name "t*.h" | xargs ./add_lvgl_if.sh

sed '0,/\*\/$/ {/\*\/$/ {n; s|^|\n#include "../../lv_conf_internal.h"\n#if LV_USE_VG_LITE_DRIVER\n|}}' $@ -i

sed -i -e '$a\
\
#endif /* LV_USE_VG_LITE_DRIVER */\
' $@ -i
