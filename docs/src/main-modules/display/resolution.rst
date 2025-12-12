.. _display_resolution:

===================
Changing Resolution
===================

To set the resolution of the display after creation use
:cpp:expr:`lv_display_set_resolution(display, hor_res, ver_res)`

It's not mandatory to use the whole display for LVGL, however in some
cases the physical resolution is important. For example, the touchpad
still sees the whole resolution, and the values need to be converted to
the active LVGL display area. So the physical resolution and the offset
of the active area can be set with
:cpp:expr:`lv_display_set_physical_resolution(disp, hor_res, ver_res)` and
:cpp:expr:`lv_display_set_offset(disp, x, y)`



API
***

.. API equals:
    lv_display_set_resolution,
    lv_display_set_physical_resolution,
    lv_display_set_offset

