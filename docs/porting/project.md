```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/porting/project.md
```

# Set-up a project

## Get the library

LVGL Graphics Library is available on GitHub: [https://github.com/lvgl/lvgl](https://github.com/lvgl/lvgl).

You can clone it or download the latest version of the library from GitHub.

The graphics library is the **lvgl** directory which should be copied into your project.

## Configuration file

There is a configuration header file for LVGL called **lv_conf.h**. In this you can set the library's basic behaviour, disable unused modules and features, adjusts the size of memory buffers in compile-time, etc.

Copy **lvgl/lv_conf_template.h** next to the *lvgl* directory and rename it to *lv_conf.h*. Open the file and change the `#if 0` at the beginning to `#if 1` to enable its content.

*lv_conf.h* can be copied other places as well but then you should add `LV_CONF_INCLUDE_SIMPLE` define to your compiler options (e.g. `-DLV_CONF_INCLUDE_SIMPLE` for gcc compiler) and set the include path manually. 
In this case LVGL will attempt to include `lv_conf.h` simply with `#include "lv_conf.h"`.

In the config file comments explain the meaning of the options. Be sure to set at least `LV_COLOR_DEPTH` according to your display's colro depth.

## Initialization

To use the graphics library you have to initialize it and the other components too. The order of the initialization is:

1. Call `lv_init()`.
2. Initialize your drivers.
3. Register the display and input devices drivers in LVGL.  Lear more about [Display](/porting/display) and [Input device](/porting/indev) registration.
4. Call `lv_tick_inc(x)` in every `x` milliseconds in an interrupt to tell the elapsed time. [Learn more](/porting/tick).
5. Call `lv_timer_handler()` periodically in every few milliseconds to handle LVGL related tasks. [Learn more](/porting/task-handler).
