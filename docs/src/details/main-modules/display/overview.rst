.. _display_overview:

========
Overview
========

What is a Display?
******************

In LVGL, an *lv_display_t* (not to be confused with a :ref:`Screen <screens>`) is a
data type that represents a single display panel --- the hardware that displays
LVGL-rendered pixels on your device.



.. _multiple_displays:

How Many Displays Can LVGL Use?
*******************************

LVGL can use any number of displays.  It is only limited by available RAM and MCU time.

Why would you want multi-display support? Here are some examples:

- Have a "normal" TFT display with local UI and create "virtual" screens on VNC
  on demand. (You need to add your VNC driver.)
- Have a large TFT display and a small monochrome display.
- Have some smaller and simple displays in a large instrument or technology.
- Have two large TFT displays: one for a customer and one for the shop assistant.

If you set up LVGL to use more than one display, be aware that some functions use the
:ref:`default_display` during their execution, such as creating :ref:`screens`.



.. _display_attributes:

Attributes
**********

Once created, a Display object remembers the characteristics of the display hardware
it is representing, as well as other things relevant to its lifetime:

- Resolution (width and height in pixels)
- Color Depth (bits per pixel)
- Color Format (how colors in pixels are laid out)
- DPI (default is configured :c:macro:`LV_DPI_DEF` in ``lv_conf.h``, but can be
  modified with :cpp:expr:`lv_display_set_dpi(disp, new_dpi)`).
- 4 :ref:`display_screen_layers` automatically created with each display
- All :ref:`screens` created in association with this display (and not yet deleted---only
  one is displayed at any given time)
- The :ref:`draw_buffers` assigned to it
- The :ref:`flush_callback` function that moves pixels from :ref:`draw_buffers` to Display hardware
- What areas of the display have been updated (made "dirty") so rendering logic can
  compute what to render during a :ref:`display refresh <basic_data_flow>`
- Optional custom pointer as :ref:`display_user_data`



.. _display_user_data:

User Data
*********

With :cpp:expr:`lv_display_set_user_data(display1, p)` a custom pointer can be stored
with ``lv_display_t`` object.  This pointer can be used later, e.g. in
:ref:`display_events`.   See code example for how to do this in :ref:`display_events`.



API
***

.. API equals:
    lv_display_set_dpi
    lv_display_set_user_data
