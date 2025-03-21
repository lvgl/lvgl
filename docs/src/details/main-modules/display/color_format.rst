.. _display_color_format:

============
Color Format
============

The default color format of the display is set according to :c:macro:`LV_COLOR_DEPTH`
(see ``lv_conf.h``)

- :c:macro:`LV_COLOR_DEPTH` ``32``: XRGB8888 (4 bytes/pixel)
- :c:macro:`LV_COLOR_DEPTH` ``24``: RGB888 (3 bytes/pixel)
- :c:macro:`LV_COLOR_DEPTH` ``16``: RGB565 (2 bytes/pixel)
- :c:macro:`LV_COLOR_DEPTH` ``8``: L8 (1 bytes/pixel)
- :c:macro:`LV_COLOR_DEPTH` ``1``: I1 (1 bit/pixel) Only support for horizontal mapped buffers. See :ref:`display_monochrome` for more details:

The ``color_format`` can be changed with
:cpp:expr:`lv_display_set_color_depth(display, LV_COLOR_FORMAT_...)`.
Besides the default value :c:macro:`LV_COLOR_FORMAT_ARGB8888` can be
used as a well.

It's very important that draw buffer(s) should be large enough for the
selected color format.



.. _display_endianness:

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



.. _display_monochrome:

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



.. _transparent_screens:

Transparent Screens
*******************

Usually, the opacity of the Screen is :cpp:enumerator:`LV_OPA_COVER` to provide a
solid background for its children. If this is not the case (opacity <
100%) the display's ``bottom_layer`` will be visible.  If the bottom layer's
opacity is also not :cpp:enumerator:`LV_OPA_COVER` LVGL will have no solid background
to draw.

This configuration (transparent Screen) could be useful to create, for example,
on-screen display (OSD) menus where a video is played on a different hardware layer
of the display panel, and a menu is overlaid on a higher layer.

To properly render a UI on a transparent Screen the Display's color format needs to
be set to one with an alpha channel (for example LV_COLOR_FORMAT_ARGB8888).

In summary, to enable transparent screens and displays for OSD menu-like UIs:

- Set the screen's ``bg_opa`` to transparent:
  :cpp:expr:`lv_obj_set_style_bg_opa(lv_screen_active(), LV_OPA_TRANSP, LV_PART_MAIN)`
- Set the bottom layer's ``bg_opa`` to transparent:
  :cpp:expr:`lv_obj_set_style_bg_opa(lv_layer_bottom(), LV_OPA_TRANSP, LV_PART_MAIN)`
- Set a color format with alpha channel. E.g.
  :cpp:expr:`lv_display_set_color_format(disp, LV_COLOR_FORMAT_ARGB8888)`



API
***

.. API equals:
    LV_COLOR_DEPTH
    LV_COLOR_FORMAT_ARGB8888
    lv_display_set_buffers
    lv_display_set_color_depth
    lv_display_set_color_format
    lv_draw_sw_i1_convert_to_vtiled
    lv_draw_sw_rgb565_swap
    lv_obj_set_style_bg_opa
