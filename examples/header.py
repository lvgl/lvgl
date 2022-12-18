#!/opt/bin/lv_micropython -i

import lvgl as lv

default_group = lv.group_create()
default_group.set_default()

lv.sdl_window_create(480, 320)
sdl_indev = lv.sdl_mouse_create()
sdl_indev.set_group(default_group)

