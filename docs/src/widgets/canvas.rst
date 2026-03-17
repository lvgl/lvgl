.. _lv_canvas:

==================
Canvas (lv_canvas)
==================


Overview
********

A Canvas inherits from :ref:`Image <lv_image>` and extends it, enabling the user to draw
anything. Rectangles, text, images, lines, arcs, etc. can be drawn here using
LVGL's extensive drawing engine.


.. _lv_canvas_parts_and_styles:

Parts and Styles
****************

-  :cpp:enumerator:`LV_PART_MAIN` Uses the :ref:`typical background <typical bg props>`
   and image style properties.


.. _lv_canvas_usage:

Usage
*****

Buffer
------

The Canvas needs a buffer in which to store the drawn image. To assign a
buffer to a Canvas, use
:cpp:expr:`lv_canvas_set_buffer(canvas, buffer, width, height, LV_COLOR_FORMAT_...)`.

Where ``buffer`` has to be valid for the entire lifecycle of the object created by :cpp:expr:`lv_canvas_create`.
It can be allocated by using :cpp:func:`lv_malloc` or a statically allocated array.

For example, for a 100x50 ARGB8888 buffer you can use:

-  ``uint8_t* buffer = lv_malloc(100 * 50 * 4)``

-  ``static uint8_t buffer[100 * 50 * 4]``

-  ``static uint8_t buffer[LV_CANVAS_BUF_SIZE(width, height, bits_per_pixel, stride_in_bytes)]``

In case you choose the :cpp:expr:`lv_malloc` way, it is up to the programmer to free the memory area, for example, on :cpp:enumerator:`LV_EVENT_DELETE` event.

Canvas supports all the color formats like
:cpp:enumerator:`LV_COLOR_FORMAT_ARGB8888` or :cpp:enumerator:`LV_COLOR_FORMAT_I2`. See the full
list in the :ref:`Color formats <images_color_formats>` section.

Indexed colors
--------------

For indexed color formats (``LV_COLOR_FORMAT_I1/2/4/8``), the palette needs to be
populated for all palette indices that will be used using
:cpp:expr:`lv_canvas_set_palette(canvas, index, color)`.  For example, the following
sets pixels with *index==3* to red.

.. code-block:: c

    lv_canvas_set_palette(canvas, 3, lv_color_hex(0xff0000))

Drawing
-------

To set an individual pixel's color on the Canvas, use
:cpp:expr:`lv_canvas_set_px(canvas, x, y, color, opa)`.  With indexed color formats
(``LV_COLOR_FORMAT_I1/2/4/8``) pass the color index as the ``color`` argument by using
the *blue* channel in the ``color`` value, e.g. :cpp:expr:`lv_color_make(0, 0, index)`.

.. tip::

    To prevent the canvas from being redundantly invalidated for every single pixel update in a loop:

    1. Use :cpp:func:`lv_display_enable_invalidation` to disable intermediate object invalidation
    2. After the loop is done, re-enable the display invalidation (via :cpp:func:`lv_display_enable_invalidation`)
    3. With the invalidation re-enabled, call :cpp:func:`lv_obj_invalidate` once.


:cpp:expr:`lv_canvas_fill_bg(canvas, lv_color_hex(0x00ff00), LV_OPA_50)` fills the whole
Canvas to blue with 50% opacity. Note that if the current color format
doesn't support colors (e.g. :cpp:enumerator:`LV_COLOR_FORMAT_A8`) the color will be
ignored. Similarly, if opacity is not supported
(e.g. :cpp:enumerator:`LV_COLOR_FORMAT_RGB565`), it will be ignored.

An array of pixels can be copied to the Canvas with
:cpp:expr:`lv_canvas_copy_buf(canvas, canvas_area, src_buf, src_area)`. The
color format of the buffer and Canvas need to match. If the canvas area and source buffer
are the same size, the source area can be left NULL.

To draw something to the Canvas use LVGL's draw functions directly. See the examples for more details.

The draw functions can draw to any color format to which LVGL can render. Typically this means
:cpp:enumerator:`LV_COLOR_FORMAT_RGB565`, :cpp:enumerator:`LV_COLOR_FORMAT_RGB888`,
:cpp:enumerator:`LV_COLOR_FORMAT_XRGB888`, and :cpp:enumerator:`LV_COLOR_FORMAT_ARGB8888`.



.. _lv_canvas_events:

Events
******

No special events are sent by Canvas Widgets.

.. admonition::  Further Reading

    Learn more about :ref:`lv_obj_events` emitted by all Widgets.

    Learn more about :ref:`events`.



.. _lv_canvas_keys:

Keys
****

No *Keys* are processed by Canvas Widgets.

.. admonition::  Further Reading

    Learn more about :ref:`indev_keys`.



.. _lv_canvas_example:

Examples
********

.. include:: /examples/widgets/canvas/index.rst



.. _lv_canvas_api:

API
***
