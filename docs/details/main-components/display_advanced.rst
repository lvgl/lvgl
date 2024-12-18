.. _display_advanced:

=================
Advanced Features
=================


Resolution
**********

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
*******************

By using :cpp:func:`lv_display_flush_ready` LVGL will spin in a loop
while waiting for flushing.

However with the help of :cpp:func:`lv_display_set_flush_wait_cb` a custom
wait callback be set for flushing. This callback can use a semaphore, mutex,
or anything else to optimize waiting for the flush to be completed.

If a Flush-Wait Callback is not set, LVGL assumes that
:cpp:func:`lv_display_flush_ready` is used.



Rotation
********

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
************

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
********************

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
*******************

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
***************************

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
***************

When multiple CPU cores are available and a large area needs to be redrawn, LVGL must
identify independent areas that can be rendered in parallel.

For example, if there are 4 CPU cores, one core can draw the screen's background
while the other 3 must wait until it is finished. If there are 2 buttons on the
screen, those 2 buttons can be rendered in parallel, but 2 cores will still remain
idle.

Due to dependencies among different areas, CPU cores cannot always be fully utilized.

To address this, LVGL can divide large areas that need to be updated into smaller
tiles. These tiles are independent, making it easier to find areas that can be
rendered concurrently.

Specifically, if there are 4 tiles and 4 cores, there will always be an independent
area for each core within one of the tiles.

The maximum number of tiles can be set using the function
:cpp:expr:`lv_display_set_tile_cnt(disp, cnt)`. The default value is
:cpp:expr:`LV_DRAW_SW_DRAW_UNIT_CNT` (or 1 if software rendering is not enabled).

Small areas are not further divided into smaller tiles because the overhead of
spinning up 4 cores would outweigh the benefits.

The ideal tile size is calculated as ``ideal_tile_size = draw_buf_size / tile_cnt``.
For example, in :cpp:enumerator:`LV_DISPLAY_RENDER_MODE_DIRECT` mode on an 800x480
screen, the display buffer is 800x480 = 375k pixels. If there are 4 tiles, the ideal
tile size is approximately 93k pixels. Based on this, core utilization is as follows:

- 30k pixels: 1 core
- 90k pixels: 1 core
- 95k pixels: 2 cores (above 93k pixels, 2 cores are used)
- 150k pixels: 2 cores
- 200k pixels: 3 cores (above 186k pixels, 3 cores are used)
- 300k pixels: 4 cores (above 279k pixels, 4 cores are used)
- 375k pixels: 4 cores

In :cpp:enumerator:`LV_DISPLAY_RENDER_MODE_DIRECT`, the screen-sized draw buffer is
divided by the tile count to determine the ideal tile sizes. If smaller areas are
refreshed, it may result in fewer cores being used.

In :cpp:enumerator:`LV_DISPLAY_RENDER_MODE_FULL`, the maximum number of tiles is
always created when the entire screen is refreshed.

In :cpp:enumerator:`LV_DISPLAY_RENDER_MODE_PARTIAL`, the partial buffer is divided
into tiles. For example, if the draw buffer is 1/10th the size of the screen and
there are 2 tiles, then 1/20th + 1/20th of the screen area will be rendered at once.

Tiled rendering only affects the rendering process, and the :ref:`flush_callback` is
called once for each invalidated area. Therefore, tiling is not visible from the
flushing point of view.



Decoupling the Display Refresh Timer
************************************

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
****************

Normally the invalidated areas (marked for redrawing) are rendered in
:cpp:func:`lv_timer_handler` in every :c:macro:`LV_DEF_REFR_PERIOD` milliseconds.
However, by using :cpp:expr:`lv_refr_now(display)` you can ask LVGL to redraw the
invalid areas immediately. The refreshing will happen in :cpp:func:`lv_refr_now`
which might take longer.

The parameter of :cpp:func:`lv_refr_now` is a display to refresh.  If ``NULL`` is set
the :ref:`default_display` will be updated.



Mirroring a Display
*******************

To mirror the image of a display to another display, you don't need to use
multi-display support. Just transfer the buffer received in the first display's
:ref:`flush_callback` to the other display as well.



Split Image
***********

You can create a larger virtual display from an array of smaller ones.
You can create it by:

1. setting the resolution of the displays to the large display's resolution;
2. in :ref:`flush_callback`, truncate and modify the ``area`` parameter for each display; and
3. send the buffer's content to each real display with the truncated area.



.. _display_user_data:

User Data
*********

With :cpp:expr:`lv_display_set_user_data(display1, p)` a custom pointer can be stored
with ``lv_display_t`` object.  This pointer can be used later, e.g. in
:ref:`display_events`.



.. _display_inactivity:

Inactivity Measurement
**********************

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

