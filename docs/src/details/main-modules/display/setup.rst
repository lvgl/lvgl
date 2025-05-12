.. _display_setup:

==========================
Setting Up Your Display(s)
==========================

During system initialization, you must do the following for each physical display
panel you want LVGL to use:

- :ref:`create an lv_display_t <creating_a_display>` object for it,
- assign its :ref:`draw_buffers`, and
- assign a :ref:`flush_callback` for it.



.. _creating_a_display:

Creating a Display
******************

To create a display for LVGL:

.. code-block:: c

    lv_display_t * display1 = lv_display_create(hor_res, ver_res)

You can create :ref:`multiple displays <multiple_displays>` with a different driver for
each (see below).

When an ``lv_display_t`` object is created, with it are created 4 Screens set up
to help you manage layering of displayed Widgets.  See :ref:`transparent_screens` and
:ref:`display_screen_layers` for more information.


.. _default_display:

Default Display
---------------
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
**************

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
--------------

.. code-block:: c

    /* Declare buffer for 1/10 screen size; BYTES_PER_PIXEL will be 2 for RGB565. */
    #define BYTES_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565))
    static uint8_t buf1[MY_DISP_HOR_RES * MY_DISP_VER_RES / 10 * BYTES_PER_PIXEL];
    /* Set display buffer for display `display1`. */
    lv_display_set_buffers(display1, buf1, NULL, sizeof(buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);

One Buffer
----------

If only one buffer is used, LVGL draws the content of the screen into
that draw buffer and sends it to the display via the :ref:`flush_callback`. LVGL
then waits until :cpp:func:`lv_display_flush_ready` is called
(that is, the content of the buffer has been sent to the
display) before drawing something new into it.

Two Buffers
-----------

If two buffers are used LVGL can draw into one buffer while the content
of the other buffer is sent to the display in the background.  DMA or
other hardware should be used to transfer data to the display so the MCU
can continue drawing.  Doing so allows *rendering* and *refreshing* the
display to become parallel operations.

Three Buffers
-------------

Triple buffering enhances parallelism between rendering and data transfer compared
to double buffering. When one buffer has completed rendering and another is actively
undergoing DMA transfer, the third buffer enables immediate rendering of the next frame,
eliminating CPU/GPU idle time caused by waiting for DMA completion.
The third buffer is configured using the :cpp:func:`lv_display_set_3rd_draw_buffer` function.


.. _flush_callback:

Flush Callback
**************

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



.. _display_flush-wait_callback:

Flush-Wait Callback
*******************

By using :cpp:func:`lv_display_flush_ready` LVGL will normally spin in a loop
while waiting for flushing.

However with the help of :cpp:func:`lv_display_set_flush_wait_cb` a custom
wait callback be set for flushing.  This callback can use a semaphore, mutex,
or anything else to optimize waiting for the flush to be completed.  The callback
need not call :cpp:func:`lv_display_flush_ready` since the caller takes care of
that (clearing the display's ``flushing`` flag) when your callback returns.

However, if a Flush-Wait Callback is not set, LVGL assumes that
:cpp:func:`lv_display_flush_ready` is called after the flush has completed.


API
***

.. API equals:
    lv_display_create,
    lv_display_flush_is_last,
    lv_display_flush_ready,
    lv_display_set_buffers,
    lv_display_set_default,
    lv_display_set_flush_cb,
    lv_display_set_flush_wait_cb
    lv_display_t,
