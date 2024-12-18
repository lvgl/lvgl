.. _display_basics:

======
Basics
======


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
