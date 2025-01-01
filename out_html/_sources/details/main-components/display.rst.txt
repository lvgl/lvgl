.. _display:

====================
Display (lv_display)
====================


What is a Display?
******************
In LVGL, an *lv_display_t* (not to be confused with a :ref:`Screen <screens>`) is a
data type that represents a single display panel --- the hardware that displays
LVGL-rendered pixels on your device.  During system initialization, you must do the
following for each display panel you want LVGL to use:

- :ref:`create an lv_display_t <creating_a_display>` object for it,
- assign a :ref:`flush_callback` for it, and
- assign its :ref:`draw_buffers`.


.. _multiple_displays:

How Many Displays Can LVGL Use?
*******************************
LVGL can use any number of displays.  It is only limited by available RAM and MCU time.

Why would you want multi-display support?  Here are some examples:

- Have a "normal" TFT display with local UI and create "virtual" screens on VNC
  on demand. (You need to add your VNC driver.)
- Have a large TFT display and a small monochrome display.
- Have some smaller and simple displays in a large instrument or technology.
- Have two large TFT displays: one for a customer and one for the shop assistant.

If you set up LVGL to use more than one display, be aware that some functions use the
:ref:`default_display` during their execution, such as creating :ref:`screens`.



.. _display_features:

Display Features
****************


.. _display_attributes:

Attributes
----------
Once created, a Display object remembers the characteristics of the display hardware
it is representing, as well as other things relevant to its lifetime:

- Resolution (width and height in pixels)
- Color Depth (bits per pixel)
- Color Format (how colors in pixels are laid out)
- DPI (default is configured :c:macro:`LV_DPI_DEF` in ``lv_conf.h``, but can be
  modified with :cpp:expr:`lv_display_set_dpi(disp, new_dpi)`).
- 4 :ref:`screen_layers` automatically created with each display
- All :ref:`screens` created in association with this display (and not yet deleted---only
  one is displayed at any given time)
- The :ref:`draw_buffers` assigned to it
- The :ref:`flush_callback` function that moves pixels from :ref:`draw_buffers` to Display hardware
- What areas of the display have been updated (made "dirty") so rendering logic can
  compute what to render during a :ref:`display refresh <basic_data_flow>`
- Optional custom pointer as :ref:`display_user_data`


.. _screen_layers:

Screen Layers
-------------

When an ``lv_display_t`` object is created, 4 permanent :ref:`screens` that
facilitate layering are created and attached to it.

1.  Bottom Layer       (below Active Screen, transparent, not scroll-able, but click-able)
2.  :ref:`active_screen`
3.  Top Layer          (above Active Screen, transparent and neither scroll-able nor click-able)
4.  System Layer       (above Top Layer, transparent and neither scroll-able nor click-able)

1, 3 and 4 are independent of the :ref:`active_screen` and they will be shown (if
they contain anything that is visible) regardless of which screen is the
:ref:`active_screen`.

.. note::

    For the bottom layer to be visible, the Active Screen's background has to be
    at least partially, if not fully, transparent.

You can get pointers to each of these screens on the :ref:`default_display` by using
(respectively):

- :cpp:func:`lv_screen_active`,
- :cpp:func:`lv_layer_top`,
- :cpp:func:`lv_layer_sys`, and
- :cpp:func:`lv_layer_bottom`.

You can get pointers to each of these screens on a specified display by using
(respectively):

- :cpp:expr:`lv_display_get_screen_active(disp)`,
- :cpp:expr:`lv_display_get_layer_top(disp)`,
- :cpp:expr:`lv_display_get_layer_sys(disp)`, and
- :cpp:expr:`lv_display_get_layer_bottom(disp)`.

To set a Screen you create to be the :ref:`active_screen`, call
:cpp:func:`lv_screen_load` or :cpp:func:`lv_screen_load_anim`.

.. _layers_top_and_sys:

Top and System Layers
~~~~~~~~~~~~~~~~~~~~~

LVGL uses the Top Layer and System Layer two empower you to ensure that certain
:ref:`widgets` are *always* on top of other layers.

You can add "pop-up windows" to the *Top Layer* freely.  The Top Layer was meant to
be used to create Widgets that are visible on all Screens shown on a Display.  But,
the *System Layer* is intended for system-level things (e.g. mouse cursor will be
placed there with :cpp:func:`lv_indev_set_cursor`).

These layers work like any other Widget, meaning they have styles, and any kind of
Widgets can be created in them.

.. note::
    While the Top Layer and System Layer are created by their owning :ref:`display`
    as not scroll-able and not click-able, these behaviors can be overridden the same
    as any other Widget by using :cpp:expr:`lv_obj_set_scrollbar_mode(scr1, LV_SCROLLBAR_MODE_xxx)`
    and :cpp:expr:`lv_obj_add_flag(scr1, LV_OBJ_FLAG_CLICKABLE)` respectively.

If the :cpp:enumerator:`LV_OBJ_FLAG_CLICKABLE` flag is set on the Top Layer, then it will
absorb all user clicks and acts as a modal Widget.

.. code-block:: c

   lv_obj_add_flag(lv_layer_top(), LV_OBJ_FLAG_CLICKABLE);

.. _layers_bottom:

Bottom Layer
~~~~~~~~~~~~

Similar to the Top- and System Layers, the Bottom Layer is also the full size of the
Display, but it is located below the :ref:`active_screen`.  It's visible only if the
Active Screen's background opacity is < 255.



.. _display_events:

Display Events
**************

:cpp:expr:`lv_display_add_event_cb(disp, event_cb, LV_EVENT_..., user_data)` adds
an event handler to a display.

If you added ``user_data`` to the Display, you can retrieve it in an event like this:

.. code-block:: c

    lv_display_t *display1;
    my_type_t    *my_user_data;
    display1 = (lv_display_t *)lv_event_get_current_target(e);
    my_user_data = lv_display_get_user_data(display1);

The following events are sent:

- :cpp:enumerator:`LV_EVENT_INVALIDATE_AREA` An area is invalidated (marked for redraw).
  :cpp:expr:`lv_event_get_param(e)` returns a pointer to an :cpp:struct:`lv_area_t`
  variable with the coordinates of the area to be invalidated. The area can
  be freely modified if needed to adopt it the special requirement of the
  display. Usually needed with monochrome displays to invalidate ``N x 8``
  rows or columns at once.
- :cpp:enumerator:`LV_EVENT_REFR_REQUEST`: Sent when something happened that requires redraw.
- :cpp:enumerator:`LV_EVENT_REFR_START`: Sent when a refreshing cycle starts. Sent even if there is nothing to redraw.
- :cpp:enumerator:`LV_EVENT_REFR_READY`: Sent when refreshing is ready (after rendering and calling the :ref:`flush_callback`). Sent even if no redraw happened.
- :cpp:enumerator:`LV_EVENT_RENDER_START`: Sent when rendering starts.
- :cpp:enumerator:`LV_EVENT_RENDER_READY`: Sent when rendering is ready (before calling the :ref:`flush_callback`)
- :cpp:enumerator:`LV_EVENT_FLUSH_START`: Sent before the :ref:`flush_callback` is called.
- :cpp:enumerator:`LV_EVENT_FLUSH_READY`: Sent when the :ref:`flush_callback` returned.
- :cpp:enumerator:`LV_EVENT_RESOLUTION_CHANGED`: Sent when the resolution changes due
  to :cpp:func:`lv_display_set_resolution` or :cpp:func:`lv_display_set_rotation`.



.. _display_setup:

Display Setup
*************


.. _creating_a_display:

Creating a Display
------------------

To create a display for LVGL:

.. code-block:: c

    lv_display_t * display1 = lv_display_create(hor_res, ver_res)

You can create :ref:`multiple displays <multiple_displays>` with a different driver for
each (see below).

When an ``lv_display_t`` object is created, with it are created 4 Screens set up
to help you manage layering of displayed Widgets.  See :ref:`transparent_screens` and
:ref:`screen_layers` for more information.


.. _default_display:

Default Display
~~~~~~~~~~~~~~~
When the first :ref:`display` object is created, it becomes the Default Display.  If
other Display Objects are created (to service additional Display Panels), the Default
Display remains the first one created.

To set another :ref:`display` as the Default Display, call :cpp:func:`lv_display_set_default`.

See :ref:`multiple_displays` for more information about using multiple displays.

For many ``lv_display_...()`` functions, passing NULL for the ``disp`` argument will
cause the function to target the Default Display.  Check the API documentation for
the function you are calling to be sure.


.. _draw_buffers:

Draw Buffer(s)
--------------

During system initialization, you must set drawing buffers for LVGL to use for each
display.  Do so by calling:

.. code-block:: c

    lv_display_set_buffers(display1, buf1, buf2, buf_size_in_bytes, render_mode)

-  ``buf1`` a buffer to which LVGL can render pixels
-  ``buf2`` a second optional buffer (see below)
-  ``buf_size_in_bytes`` size of buffer(s) in bytes
-  ``render_mode`` is one of the following:

   -  :cpp:enumerator:`LV_DISPLAY_RENDER_MODE_PARTIAL` Use the buffer(s) to render
      to the display using buffers smaller than the size of the display.  Use of
      buffers at least 1/10 display size is recommended.  In :ref:`flush_callback` the rendered
      images needs to be copied to the given area of the display.  In this mode if a
      button is pressed only the button's area will be redrawn.
   -  :cpp:enumerator:`LV_DISPLAY_RENDER_MODE_DIRECT` The buffer size(es) must match
      the size of the display.  LVGL will render into the correct location of the
      buffer.  Using this method the buffer(s) always contain the whole display image.
      If two buffer are used, the rendered areas are automatically copied to the
      other buffer after flushing.  Due to this in :ref:`flush_callback` typically
      only a frame buffer address needs to be changed.  If a button is pressed
      only the button's area will be redrawn.
   -  :cpp:enumerator:`LV_DISPLAY_RENDER_MODE_FULL` The buffer size(es) must match
      the size of the display.  LVGL will always redraw the whole screen even if only
      1 pixel has been changed.  If two display-sized draw buffers are provided,
      LVGL's display handling works like "traditional" double buffering.  This means
      the :ref:`flush_callback` callback only has to update the address of the frame buffer to
      the ``px_map`` parameter.


Simple Example
~~~~~~~~~~~~~~

.. code-block:: c

    /* Declare buffer for 1/10 screen size; BYTES_PER_PIXEL will be 2 for RGB565. */
    #define BYTES_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565))
    static uint8_t buf1[MY_DISP_HOR_RES * MY_DISP_VER_RES / 10 * BYTES_PER_PIXEL];
    /* Set display buffer for display `display1`. */
    lv_display_set_buffers(display1, buf1, NULL, sizeof(buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);

One Buffer
~~~~~~~~~~

If only one buffer is used, LVGL draws the content of the screen into
that draw buffer and sends it to the display via the :ref:`flush_callback`. LVGL
then waits until :cpp:func:`lv_display_flush_ready` is called
(that is, the content of the buffer has been sent to the
display) before drawing something new into it.

Two Buffers
~~~~~~~~~~~

If two buffers are used LVGL can draw into one buffer while the content
of the other buffer is sent to the display in the background.  DMA or
other hardware should be used to transfer data to the display so the MCU
can continue drawing.  Doing so allows *rendering* and *refreshing* the
display to become parallel operations.


.. _flush_callback:

Flush Callback
--------------

Draw buffer(s) are simple array(s) that LVGL uses to render the display's
content.  Once rendering is has been completed, the content of the draw buffer is
sent to the display using a Flush Callback function.

An example looks like this:

.. code-block:: c

    void my_flush_cb(lv_display_t * display, const lv_area_t * area, uint8_t * px_map)
    {
        /* The most simple case (also the slowest) to send all rendered pixels to the
         * screen one-by-one.  `put_px` is just an example.  It needs to be implemented by you. */
        uint16_t * buf16 = (uint16_t *)px_map; /* Let's say it's a 16 bit (RGB565) display */
        int32_t x, y;
        for(y = area->y1; y <= area->y2; y++) {
            for(x = area->x1; x <= area->x2; x++) {
                put_px(x, y, *buf16);
                buf16++;
            }
        }

        /* IMPORTANT!!!
         * Inform LVGL that flushing is complete so buffer can be modified again. */
        lv_display_flush_ready(display);
    }

During system initialization, tell LVGL you want that function to copy pixels from
rendered pixel-buffers to a particular display by doing the following:

.. code-block:: c

    lv_display_set_flush_cb(display1, my_flush_cb)

Note that which display is targeted is passed to the function, so you can use the
same function for multiple displays, or use different functions for multiple
displays.  It's up to you.

.. note::

    :cpp:expr:`lv_display_flush_ready(display1)` needs to be called when flushing is
    complete to inform LVGL that the buffer is available again to render new content
    into it.

LVGL might render the screen in multiple chunks and therefore call your Flush
Callback multiple times.  To see whether the current call is for the last chunk being
rendered, use :cpp:expr:`lv_display_flush_is_last(display1)`.



Advanced Options
****************


Resolution
----------

To set the resolution of the display after creation use
:cpp:expr:`lv_display_set_resolution(display, hor_res, ver_res)`

It's not mandatory to use the whole display for LVGL, however in some
cases the physical resolution is important. For example the touchpad
still sees the whole resolution and the values needs to be converted to
the active LVGL display area. So the physical resolution and the offset
of the active area can be set with
:cpp:expr:`lv_display_set_physical_resolution(disp, hor_res, ver_res)` and
:cpp:expr:`lv_display_set_offset(disp, x, y)`


Flush-Wait Callback
-------------------

By using :cpp:func:`lv_display_flush_ready` LVGL will spin in a loop
while waiting for flushing.

However with the help of :cpp:func:`lv_display_set_flush_wait_cb` a custom
wait callback be set for flushing. This callback can use a semaphore, mutex,
or anything else to optimize waiting for the flush to be completed.

If a Flush-Wait Callback is not set, LVGL assumes that
:cpp:func:`lv_display_flush_ready` is used.


Rotation
--------

LVGL supports rotation of the display in 90 degree increments.

The orientation of the display can be changed with
:cpp:expr:`lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_xxx)` where ``xxx`` is
0, 90, 180 or 270.  This will swap the horizontal and vertical resolutions internally
according to the set degree, however it will not perform the actual rotation.
When changing the rotation, the :cpp:enumerator:`LV_EVENT_SIZE_CHANGED` event is
emitted to allow for hardware reconfiguration.  If your display panel and/or its
driver chip(s) do not support rotation, :cpp:func:`lv_draw_sw_rotate` can be used to
rotate the buffer in the :ref:`flush_callback` function.

:cpp:expr:`lv_display_rotate_area(display, &area)` rotates the rendered area
according to the current rotation settings of the display.

Note that in :cpp:enumerator:`LV_DISPLAY_RENDER_MODE_DIRECT` the small changed areas
are rendered directly in the frame buffer so they cannot be
rotated later. Therefore in direct mode only the whole frame buffer can be rotated.
The same is true for :cpp:enumerator:`LV_DISPLAY_RENDER_MODE_FULL`.

In the case of :cpp:enumerator:`LV_DISPLAY_RENDER_MODE_PARTIAL` the small rendered areas
can be rotated on their own before flushing to the frame buffer.

Below is an example for rotating when the rendering mode is
:cpp:enumerator:`LV_DISPLAY_RENDER_MODE_PARTIAL` and the rotated image should be sent to a
**display controller**.

.. code-block:: c

    /*Rotate a partially rendered area to another buffer and send it*/
    void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
    {
        lv_display_rotation_t rotation = lv_display_get_rotation(disp);
        lv_area_t rotated_area;
        if(rotation != LV_DISPLAY_ROTATION_0) {
            lv_color_format_t cf = lv_display_get_color_format(disp);
            /*Calculate the position of the rotated area*/
            rotated_area = *area;
            lv_display_rotate_area(disp, &rotated_area);
            /*Calculate the source stride (bytes in a line) from the width of the area*/
            uint32_t src_stride = lv_draw_buf_width_to_stride(lv_area_get_width(area), cf);
            /*Calculate the stride of the destination (rotated) area too*/
            uint32_t dest_stride = lv_draw_buf_width_to_stride(lv_area_get_width(&rotated_area), cf);
            /*Have a buffer to store the rotated area and perform the rotation*/
            static uint8_t rotated_buf[500*1014];
            int32_t src_w = lv_area_get_width(area);
            int32_t src_h = lv_area_get_height(area);
            lv_draw_sw_rotate(px_map, rotated_buf, src_w, src_h, src_stride, dest_stride, rotation, cf);
            /*Use the rotated area and rotated buffer from now on*/
            area = &rotated_area;
            px_map = rotated_buf;
        }
        my_set_window(area->x1, area->y1, area->x2, area->y2);
        my_send_colors(px_map);
    }

Below is an example for rotating when the rendering mode is
:cpp:enumerator:`LV_DISPLAY_RENDER_MODE_PARTIAL` and the image can be rotated directly
into a **frame buffer of the LCD peripheral**.

.. code-block:: c

    /*Rotate a partially rendered area to the frame buffer*/
    void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
    {
        lv_color_format_t cf = lv_display_get_color_format(disp);
        uint32_t px_size = lv_color_format_get_size(cf);
        /*Calculate the position of the rotated area*/
        lv_area_t rotated_area = *area;
        lv_display_rotate_area(disp, &rotated_area);
        /*Calculate the properties of the source buffer*/
        int32_t src_w = lv_area_get_width(area);
        int32_t src_h = lv_area_get_height(area);
        uint32_t src_stride = lv_draw_buf_width_to_stride(src_w, cf);
        /*Calculate the properties of the frame buffer*/
        int32_t fb_stride = lv_draw_buf_width_to_stride(disp->hor_res, cf);
        uint8_t * fb_start = my_fb_address;
        fb_start += rotated_area.y1 * fb_stride + rotated_area.x1 * px_size;
        lv_display_rotation_t rotation = lv_display_get_rotation(disp);
        if(rotation == LV_DISPLAY_ROTATION_0) {
            int32_t y;
            for(y = area->y1; y <= area->y2; y++) {
                lv_memcpy(fb_start, px_map, src_stride);
                px_map += src_stride;
                fb_start += fb_stride;
            }
        }
        else {
            lv_draw_sw_rotate(px_map, fb_start, src_w, src_h, src_stride, fb_stride, rotation, cf);
        }
    }

Color Format
------------

The default color format of the display is set according to :c:macro:`LV_COLOR_DEPTH`
(see ``lv_conf.h``)

- :c:macro:`LV_COLOR_DEPTH` ``32``: XRGB8888 (4 bytes/pixel)
- :c:macro:`LV_COLOR_DEPTH` ``24``: RGB888 (3 bytes/pixel)
- :c:macro:`LV_COLOR_DEPTH` ``16``: RGB565 (2 bytes/pixel)
- :c:macro:`LV_COLOR_DEPTH` ``8``: L8 (1 bytes/pixel)
- :c:macro:`LV_COLOR_DEPTH` ``1``: I1 (1 bit/pixel) Only support for horizontal mapped buffers. See :ref:`monochrome` for more details:

The ``color_format`` can be changed with
:cpp:expr:`lv_display_set_color_depth(display, LV_COLOR_FORMAT_...)`.
Besides the default value :c:macro:`LV_COLOR_FORMAT_ARGB8888` can be
used as a well.

It's very important that draw buffer(s) should be large enough for the
selected color format.


Swapping Endian-ness
--------------------

In case of RGB565 color format it might be required to swap the 2 bytes
because the SPI, I2C or 8 bit parallel port periphery sends them in the wrong order.

The ideal solution is configure the hardware to handle the 16 bit data with different byte order,
however if this is not possible :cpp:expr:`lv_draw_sw_rgb565_swap(buf, buf_size_in_px)`
can be called in the :ref:`flush_callback` to swap the bytes.

If you wish you can also write your own function, or use assembly instructions for
the fastest possible byte swapping.

Note that this is not about swapping the Red and Blue channel but converting

``RRRRR GGG | GGG BBBBB``

to

``GGG BBBBB | RRRRR GGG``.

.. _monochrome:


Monochrome Displays
-------------------

LVGL supports rendering directly in a 1-bit format for monochrome displays.
To enable it, set ``LV_COLOR_DEPTH 1`` or use :cpp:expr:`lv_display_set_color_format(display, LV_COLOR_FORMAT_I1)`.

The :cpp:expr:`LV_COLOR_FORMAT_I1` format assumes that bytes are mapped to rows (i.e., the bits of a byte are written next to each other).
The order of bits is MSB first, which means:

.. code-block::

             MSB           LSB
   bits       7 6 5 4 3 2 1 0

are represented on the display as:

.. code-block::

   pixels     0 1 2 3 4 5 6 7
             Left         Right

Ensure that the LCD controller is configured accordingly.

Internally, LVGL rounds the redrawn areas to byte boundaries. Therefore, updated areas will:

- start on an ``Nx8`` coordinate, and
- end on an ``Nx8 - 1`` coordinate.

When setting up the buffers for rendering (:cpp:func:`lv_display_set_buffers`), make the buffer 8 bytes larger.
This is necessary because LVGL reserves 2 x 4 bytes in the buffer, as these are assumed to be used as a palette.

To skip the palette, include the following line in your :ref:`flush_callback` function: ``px_map += 8``.

As usual, monochrome displays support partial, full, and direct rendering modes as well.
In full and direct modes, the buffer size should be large enough for the whole screen,
meaning ``(horizontal_resolution x vertical_resolution / 8) + 8`` bytes.
As LVGL can not handle fractional width make sure to round the horizontal resolution
to 8 bits (for example 90 to 96).

The :cpp:func:`lv_draw_sw_i1_convert_to_vtiled` function is used to convert a draw
buffer in I1 color format from a row-wise (htiled) to a column-wise (vtiled) buffer
layout.  This conversion is necessary for certain display controllers that require a
different draw buffer mapping.  The function assumes that the buffer width and height
are rounded to a multiple of 8.  The bit order of the resulting vtiled buffer can be
specified using the `bit_order_lsb` parameter.

For more details, refer to the implementation in
:cpp:func:`lv_draw_sw_i1_convert_to_vtiled` in :file:`src/draw/sw/lv_draw_sw.c`.

To ensure that the redrawn areas start and end on byte boundaries, you can add a
rounder callback to your display driver.  This callback will round the width and
height to the nearest multiple of 8.

Here is an example of how to implement and set a rounder callback:

.. code:: c

    static void my_rounder_cb(lv_event_t *e)
    {
        lv_area_t *area = lv_event_get_param(e);

        /* Round the height to the nearest multiple of 8 */
        area->y1 = (area->y1 & ~0x7);
        area->y2 = (area->y2 | 0x7);
    }

    lv_display_add_event_cb(display, my_rounder_cb, LV_EVENT_INVALIDATE_AREA, display);

In this example, the `my_rounder_cb` function rounds the coordinates of the redrawn
area to the nearest multiple of 8. The `x1` and `y1` coordinates are rounded down,
while the `x2` and `y2` coordinates are rounded up. This ensures that the width and
height of the redrawn area are always multiples of 8.

Constraints on Redrawn Area
---------------------------

Some display controllers have specific requirements for the window area where the rendered image can be sent
(e.g., `x1` must be even, and `x2` must be odd).

In the case of monochrome displays, `x1` must be `Nx8`, and `x2` must be `Nx8 - 1`.
(If the display uses `LV_COLOR_FORMAT_I1`, LVGL automatically applies this rounding. See :ref:`monochrome`.)

The size of the invalidated (redrawn) area can be controlled as follows:

.. code-block:: c

    void rounder_event_cb(lv_event_t * e)
    {
        lv_area_t * a = lv_event_get_invalidated_area(e);

        a->x1 = a->x1 & (~0x1); /* Ensure x1 is even */
        a->x2 = a->x2 | 0x1;    /* Ensure x2 is odd */
    }

    ...

    lv_display_add_event_cb(disp, rounder_event_cb, LV_EVENT_INVALIDATE_AREA, NULL);


Tiled Rendering
---------------

When multiple CPU cores are available and a large area needs to be redrawn, LVGL must identify independent areas that can be rendered in parallel.

For example, if there are 4 CPU cores, one core can draw the screen's background while the other 3 must wait until it is finished. If there are 2 buttons on the screen, those 2 buttons can be rendered in parallel, but 2 cores will still remain idle.

Due to dependencies among different areas, CPU cores cannot always be fully utilized.

To address this, LVGL can divide large areas that need to be updated into smaller tiles. These tiles are independent, making it easier to find areas that can be rendered concurrently.

Specifically, if there are 4 tiles and 4 cores, there will always be an independent area for each core within one of the tiles.

The maximum number of tiles can be set using the function :cpp:expr:`lv_display_set_tile_cnt(disp, cnt)`. The default value is :cpp:expr:`LV_DRAW_SW_DRAW_UNIT_CNT` (or 1 if software rendering is not enabled).

Small areas are not further divided into smaller tiles because the overhead of spinning up 4 cores would outweigh the benefits.

The ideal tile size is calculated as ``ideal_tile_size = draw_buf_size / tile_cnt``. For example, in :cpp:enumerator:`LV_DISPLAY_RENDER_MODE_DIRECT` mode on an 800x480 screen, the display buffer is 800x480 = 375k pixels. If there are 4 tiles, the ideal tile size is approximately 93k pixels. Based on this, core utilization is as follows:

- 30k pixels: 1 core
- 90k pixels: 1 core
- 95k pixels: 2 cores (above 93k pixels, 2 cores are used)
- 150k pixels: 2 cores
- 200k pixels: 3 cores (above 186k pixels, 3 cores are used)
- 300k pixels: 4 cores (above 279k pixels, 4 cores are used)
- 375k pixels: 4 cores

In :cpp:enumerator:`LV_DISPLAY_RENDER_MODE_DIRECT`, the screen-sized draw buffer is divided by the tile count to determine the ideal tile sizes. If smaller areas are refreshed, it may result in fewer cores being used.

In :cpp:enumerator:`LV_DISPLAY_RENDER_MODE_FULL`, the maximum number of tiles is always created when the entire screen is refreshed.

In :cpp:enumerator:`LV_DISPLAY_RENDER_MODE_PARTIAL`, the partial buffer is divided into tiles. For example, if the draw buffer is 1/10th the size of the screen and there are 2 tiles, then 1/20th + 1/20th of the screen area will be rendered at once.

Tiled rendering only affects the rendering process, and the :ref:`flush_callback` is called once for each invalidated area. Therefore, tiling is not visible from the flushing point of view.


Decoupling the Display Refresh Timer
------------------------------------

Normally the dirty (a.k.a invalid) areas are checked and redrawn in
every :c:macro:`LV_DEF_REFR_PERIOD` milliseconds (set in ``lv_conf.h``).
However, in some cases you might need more control on when the display
refreshing happen, for example to synchronize rendering with VSYNC or
the TE signal.

You can do this in the following way:

.. code-block:: c

   /* Delete original display refresh timer */
   lv_display_delete_refr_timer(display1);

   /* Call this to refresh dirty (changed) areas of the display. */
   _lv_display_refr_timer(NULL);

If you have multiple displays call :cpp:expr:`lv_display_set_default(display1)` to
select the display to refresh before :cpp:expr:`_lv_display_refr_timer(NULL)`.


.. note:: :cpp:func:`lv_timer_handler` and :cpp:func:`_lv_display_refr_timer` must not run at the same time.


If the performance monitor is enabled, the value of :c:macro:`LV_DEF_REFR_PERIOD` needs to be set to be
consistent with the refresh period of the display to ensure that the statistical results are correct.


Force Refreshing
----------------

Normally the invalidated areas (marked for redrawing) are rendered in
:cpp:func:`lv_timer_handler` in every :c:macro:`LV_DEF_REFR_PERIOD` milliseconds.
However, by using :cpp:expr:`lv_refr_now(display)` you can ask LVGL to redraw the
invalid areas immediately. The refreshing will happen in :cpp:func:`lv_refr_now`
which might take longer.

The parameter of :cpp:func:`lv_refr_now` is a display to refresh.  If ``NULL`` is set
the :ref:`default_display` will be updated.


Mirroring a Display
-------------------

To mirror the image of a display to another display, you don't need to use
multi-display support. Just transfer the buffer received in the first display's
:ref:`flush_callback` to the other display as well.


Split Image
-----------

You can create a larger virtual display from an array of smaller ones.
You can create it by:

1. setting the resolution of the displays to the large display's resolution;
2. in :ref:`flush_callback`, truncate and modify the ``area`` parameter for each display; and
3. send the buffer's content to each real display with the truncated area.


.. _display_user_data:

User Data
---------

With :cpp:expr:`lv_display_set_user_data(display1, p)` a custom pointer can be stored
with ``lv_display_t`` object.  This pointer can be used later, e.g. in
:ref:`display_events`.


.. _display_inactivity:

Inactivity Measurement
----------------------

A user's inactivity time is measured and stored with each ``lv_display_t`` object.
Every use of an :ref:`Input Device <indev>` (if :ref:`associated with the display
<indev_other_features>`) counts as an activity.  To get time elapsed since the last
activity, use :cpp:expr:`lv_display_get_inactive_time(display1)`.  If ``NULL`` is
passed, the lowest inactivity time among all displays will be returned (in this case
NULL does *not* mean the :ref:`default_display`).

You can manually trigger an activity using
:cpp:expr:`lv_display_trigger_activity(display1)`.  If ``display1`` is ``NULL``, the
:ref:`default_display` will be used (**not all displays**).


.. admonition::  Further Reading

    -  `lv_port_disp_template.c <https://github.com/lvgl/lvgl/blob/master/examples/porting/lv_port_disp_template.c>`__
       for a template for your own driver.
    -  :ref:`Drawing <draw>` to learn more about how rendering works in LVGL.



API
***
