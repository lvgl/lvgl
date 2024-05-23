.. _lv_canvas:

==================
Canvas (lv_canvas)
==================

Overview
********

A Canvas inherits from :ref:`Image <lv_image>` where the user can draw
anything. Rectangles, texts, images, lines, arcs can be drawn here using
lvgl's drawing engine.

.. _lv_canvas_parts_and_styles:

Parts and Styles
****************

-  :cpp:enumerator:`LV_PART_MAIN` Uses the typical rectangle style properties and image
style properties.

.. _lv_canvas_usage:

Usage
*****

Buffer
------

The Canvas needs a buffer in which it stores the drawn image. To assign a
buffer to a Canvas, use
:cpp:expr:`lv_canvas_set_buffer(canvas, buffer, width, height, LV_COLOR_FORMAT_...)`.
Where ``buffer`` is a static buffer (not just a local variable) to hold
the image of the canvas. For example for a 100x50 ARGB8888 buffer:
``static uint8_t buffer[100 * 50 * 4]``.

Or you can use
``static uint8_t buffer[LV_CANVAS_BUF_SIZE(width, height, bit_per_pixel, stride_in_bytes)]``.

The canvas supports all the color formats like
:cpp:enumerator:`LV_COLOR_FORMAT_ARGB8888` or :cpp:enumerator:`LV_COLOR_FORMAT_I2`. See the full
list in the :ref:`Color formats <overview_image_color_formats>` section.

Indexed colors
--------------

For ``LV_COLOR_FORMAT_I1/2/4/8`` color formats a palette needs to be
initialized with :cpp:expr:`lv_canvas_set_palette(canvas, 3, lv_color_hex(0xff0000))`. It
sets pixels with *index=3* to red.

Drawing
-------

To set a pixel's color on the canvas, use
:cpp:expr:`lv_canvas_set_px_color(canvas, x, y, color, opa)`. With
``LV_COLOR_FORMAT_I1/2/4/8`` the index of the color needs to be passed as
color like this ``lv_color_from_int(13);``. It passes index 13 as a color.


:cpp:expr:`lv_canvas_fill_bg(canvas, lv_color_hex(0x00ff00), LV_OPA_50)` fills the whole
canvas to blue with 50% opacity. Note that if the current color format
doesn't support colors (e.g. :cpp:enumerator:`LV_COLOR_FORMAT_A8`) the color will be
ignored. Similarly, if opacity is not supported
(e.g. :cpp:enumerator:`LV_COLOR_FORMAT_RGB565`) it will be ignored.

An array of pixels can be copied to the canvas with
:cpp:expr:`lv_canvas_copy_buf(canvas, buffer_to_copy, x, y, width, height)`. The
color format of the buffer and the canvas need to match.

To draw something to the canvas use LVGL's draw functions directly. See the examples for more details.

The draw function can draw to any color format to which LVGL can render. Typically it means
:cpp:enumerator:`LV_COLOR_FORMAT_RGB565`, :cpp:enumerator:`LV_COLOR_FORMAT_RGB888`,
:cpp:enumerator:`LV_COLOR_FORMAT_XRGB888`, and :cpp:enumerator:`LV_COLOR_FORMAT_ARGB8888`.

.. _lv_canvas_events:

Events
******

No special events are sent by canvas objects. The same events are sent
as for the

See the events of the :ref:`lv_image` too.

Learn more about :ref:`events`.

.. _lv_canvas_keys:

Keys
****

No *Keys* are processed by the object type.

Learn more about :ref:`indev_keys`.

.. _lv_canvas_example:

Example
*******

.. include:: ../examples/widgets/canvas/index.rst

.. _lv_canvas_api:

API
***
