.. _display:

========
Displays
========

:important: The basic concept of a *display* in LVGL is explained in the :ref:`porting` section. So before reading further, please read that section first.

.. _display_multi_display_support:

Multiple display support
************************

In LVGL you can have multiple displays, each with their own driver,
widgets and color depth.

Creating more displays is easy: just use :cpp:func:`lv_display_create` and
add set the buffer and the ``flush_cb``. When you create the UI, use
:cpp:expr:`lv_display_set_default(disp)` to tell the library on which display to
create objects.

Why would you want multi-display support? Here are some examples:

- Have a "normal" TFT display with local UI and create "virtual" screens on VNC
  on demand. (You need to add your VNC driver).
- Have a large TFT display and a small monochrome display.
- Have some smaller and simple displays in a large instrument or technology.
- Have two large TFT displays: one for a customer and one for the shop assistant.

.. _display_one_display:

Using only one display
----------------------

Using more displays can be useful but in most cases it's not required.
Therefore, the whole concept of multi-display handling is completely
hidden if you register only one display. By default, the last created
(and only) display is used.

:cpp:func:`lv_screen_active`, :cpp:func:`lv_screen_load`, :cpp:func:`lv_layer_top`,
:cpp:func:`lv_layer_sys`, :c:macro:`LV_HOR_RES` and :c:macro:`LV_VER_RES` are always applied
on the most recently created (default) display. If you pass ``NULL`` as
``disp`` parameter to display related functions the default display will
usually be used. E.g. :cpp:expr:`lv_display_trigger_activity(NULL)` will trigger a
user activity on the default display. (See below in :ref:`Inactivity <display_inactivity>`).

Mirror display
--------------

To mirror the image of a display to another display, you don't need to
use multi-display support. Just transfer the buffer received in
``flush_cb`` to the other display too.

Split image
-----------

You can create a larger virtual display from an array of smaller ones.
You can create it as below: 1. Set the resolution of the displays to the
large display's resolution. 2. In ``flush_cb``, truncate and modify the
``area`` parameter for each display. 3. Send the buffer's content to
each real display with the truncated area.

.. _display_screens:

Screens
*******

Every display has its own set of :ref:`screens <objects_screens>` and the
objects on each screen.

Be sure not to confuse displays and screens:

-  **Displays** are the physical hardware drawing the pixels.
-  **Screens** are the high-level root objects associated with a
   particular display. One display can have multiple screens associated
   with it, but not vice versa.

Screens can be considered the highest level containers which have no
parent. A screen's size is always equal to its display and their origin
is (0;0). Therefore, a screen's coordinates can't be changed,
i.e. :cpp:func:`lv_obj_set_pos`, :cpp:func:`lv_obj_set_size` or similar functions
can't be used on screens.

A screen can be created from any object type but the two most typical
types are :ref:`Base object <lv_obj>` and :ref:`Image <lv_image>`
(to create a wallpaper).

To create a screen, use
:cpp:expr:`lv_obj_t * scr = lv_<type>_create(NULL)`. ``NULL`` indicates no parent.

To load a screen, use :cpp:expr:`lv_screen_load(scr)`. To get the active screen,
use :cpp:expr:`lv_screen_active()`. These functions work on the default display. If
you want to specify which display to work on, use
:cpp:expr:`lv_display_get_screen_active(disp)` and :cpp:expr:`lv_display_load_screen(disp, scr)`. A
screen can be loaded with animations too. Read more
:ref:`here <objects_load_screens>`.

Screens can be deleted with :cpp:expr:`lv_obj_delete(scr)`, but ensure that you do
not delete the currently loaded screen.

Transparent screens
-------------------

Usually, the opacity of the screen is :cpp:enumerator:`LV_OPA_COVER` to provide a
solid background for its children. If this is not the case (opacity <
100%) the display's ``bottom_layer`` be visible. If the bottom layer's
opacity is also not :cpp:enumerator:`LV_OPA_COVER` LVGL has no solid background to
draw.

This configuration (transparent screen and display) could be used to
create for example OSD menus where a video is played on a lower layer,
and a menu is overlaid on an upper layer.

To properly render the screen the display's color format needs to be set
to one with alpha channel.

In summary, to enable transparent screens and displays for OSD menu-like
UIs:

- Set the screen's ``bg_opa`` to transparent:
  :cpp:expr:`lv_obj_set_style_bg_opa(lv_screen_active(), LV_OPA_TRANSP, LV_PART_MAIN)`
- Set the bottom layer's ``bg_opa`` to transparent:
  :cpp:expr:`lv_obj_set_style_bg_opa(lv_layer_bottom(), LV_OPA_TRANSP, LV_PART_MAIN)`
- Set the screen's ``bg_opa`` to 0:
  :cpp:expr:`lv_obj_set_style_bg_opa(lv_screen_active(), LV_OPA_0, LV_PART_MAIN)`
- Set a color format with alpha channel. E.g.
  :cpp:expr:`lv_display_set_color_format(disp, LV_COLOR_FORMAT_ARGB8888)`

.. _display_features:

Features of displays
********************

.. _display_inactivity:

Inactivity
----------

A user's inactivity time is measured on each display. Every use of an
:ref:`Input device <indev>` (if :ref:`associated with the display <porting_indev_other_features>`) counts as an activity. To
get time elapsed since the last activity, use
:cpp:expr:`lv_display_get_inactive_time(disp)`. If ``NULL`` is passed, the lowest
inactivity time among all displays will be returned (**NULL isn't just
the default display**).

You can manually trigger an activity using
:cpp:expr:`lv_display_trigger_activity(disp)`. If ``disp`` is ``NULL``, the default
screen will be used (**and not all displays**).

Background
----------

Every display has a background color, background image and background
opacity properties. They become visible when the current screen is
transparent or not positioned to cover the whole display.

The background color is a simple color to fill the display. It can be
adjusted with :cpp:expr:`lv_obj_set_style_bg_color(obj, color)`;

The display background image is a path to a file or a pointer to an
:cpp:struct:`lv_image_dsc_t` variable (converted image data) to be used as
wallpaper. It can be set with :cpp:expr:`lv_obj_set_style_bg_img_src(obj, &my_img)`;
If a background image is configured the background won't be filled with
``bg_color``.

The opacity of the background color or image can be adjusted with
:cpp:expr:`lv_obj_set_style_bg_opa(obj, opa)`.

The ``disp`` parameter of these functions can be ``NULL`` to select the
default display.

.. _display_api:

API
***
