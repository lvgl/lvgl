.. _changelog:

Changelog
=========

v9.0.0
~~~~~~

Migration guide
^^^^^^^^^^^^^^^

As v9 is a major version it contains API breaking changes too. Most of the conceptual changes in v9 were internal, however the API was affected some widgets reword and refactoring as well.


IMPORTANT
---------

If you are updating a v8 project to v9, special care must be taken as some parts

-  will not result in compiler error, but LVGL might not work due to related issues
-  will result in hard to understand compiler errors

So pay extra attention to these:

- :cpp:func:`lv_display_set_buffers(display, buf1, buf2, buf_size_byte, mode)` is more or less the equivalent of ``lv_disp_draw_buf_init(&draw_buf_dsc, buf1, buf2, buf_size_px)`` from v8, however in **v9 the buffer size is set in bytes**.
- In v9 ``lv_color_t`` is always RGB888 regardless of ``LV_COLOR_DEPTH``.
- ``lv_conf.h`` has been changed a lot, so don't forget to update it from ``lv_conf_template.h``
- Be sure ``<stdint.h>`` is **not** included in ``lv_conf.h``. In v9 we have some assembly parts for even better performance and a random include there would mess up the assembly part.
- The online image converter in not updated yet. Until that use `LVGLImage.py <https://github.com/lvgl/lvgl/blob/master/scripts/LVGLImage.py>`__ .
- Run time dithering is rendering due its complexity and lack of GPU support. Smaller dithered and tiled images can be used as background images as a replacement.
- STM32's DMA2D (Chrom-ART) support is removed for now. It will be added again when an official partnership starts with ST too.
- SJPG was removed as the original TJPGD support decoding tile-by-tile. (typically a tile is 8x8 pixels)
- ``LV_COLOR_DEPTH 8`` is not supported yet. In v8 it meant RGB332, in v9 it will be used for L8.

Main new features
-----------------

- Run time display color format adjustment with RGB888 support
- Built-in support ``pthread``, ``FreeRTOS`` and other (RT)OSes which are used during rendering
- Built-in support LVGL's, C library, and other ``stdlib``s
- Better parallel rendering architecture. See the details :ref:`here <porting_draw>`
- Built in display and touch driver: SDL, Linux Frame buffer,  NuttX LCD and touch drivers, ST7789 and ILI9341 driver are available and more will come soon
- :ref:`observer` allows to bind data to UI elements and create a uniform and easy to maintain API
- GitHub CodeSpace integration makes possible to run LVGL in an Online VSCode editor with 3 click. See more `here <https://blog.lvgl.io/2023-04-13/monthly-newsletter>`__
- Add vector graphics support via ThorVG. It can be used to draw vector graphics to a `Canvas <https://github.com/lvgl/lvgl/blob/master/examples/widgets/canvas/lv_example_canvas_8.c>`__
- :ref:`lv_image` supports aligning, stretching or tiling the image source if the widget is larger or smaller.

General API changes
-------------------


Although `lv_api_map.h <https://github.com/lvgl/lvgl/blob/master/src/lv_api_map.h>`__ address most of the refactoring we encourage you to use the latest API directly.

-  ``lv_disp_...`` is renamed to ``lv_display_...``
-  ``btn_...`` is renamed to ``button_...``
-  ``btnmatrix_...`` is renamed to ``buttonmatrix_...``
-  ``img_...`` is renamed to ``image_...``
-  ``zoom`` is renamed to ``scale``
-  ``angle`` is renamed to ``rotation``
-  ``scr`` is renamed to ``screen``
-  ``act`` is renamed to ``active``
-  ``del`` is renamed to ``delete``
-  ``col`` is renamed to ``column``
-  ``lv_obj_clear_flag`` is renamed to ``lv_obj_remove_flag``
-  ``lv_obj_clear_state`` is renamed to ``lv_obj_remove_state``
-  ``lv_coord_t`` was removed and replaced by ``int32_t``

New color format management
---------------------------

-  ``LV_IMG_CF_...`` was replaced by ``LV_COLOR_FORMAT_...``
-  ``LV_COLOR_DEPTH 24`` is supported for RGB888 rendering
-  ``lv_color_t`` always means RGB888

Display API
-----------

-  ``lv_disp_drv_t`` and ``lv_disp_draw_buf_t`` was removed
-  To create a display and set it up:

.. code:: c

   lv_display_t * disp = lv_display_create(hor_res, ver_res)
   lv_display_set_flush_cb(disp, flush_cb);
   lv_display_set_buffers(disp, buf1, buf2, buf_size_in_bytes, mode);

-  Note that now **buf size is in bytes and not pixels**
-  ``mode`` can be:

   -  ``LV_DISPLAY_RENDER_MODE_PARTIAL`` This way the buffers can be
      smaller then the display to save RAM. At least 1/10 screen sized
      buffer(s) are recommended.
   -  ``LV_DISPLAY_RENDER_MODE_DIRECT`` The buffer(s) has to be screen
      sized and LVGL will render into the correct location of the
      buffer. This way the buffer always contain the whole image. With 2
      buffers the buffers’ content are kept in sync automatically. (Old
      v7 behavior)
   -  ``LV_DISPLAY_RENDER_MODE_FULL`` Just always redraw the whole
      screen. With 2 buffers is a standard double buffering.

-  Similarly to the widgets, now you can attach events to the display
   too, using ``lv_display_add_event()``
-  ``monitor_cb`` is removed and ``LV_EVENT_RENDER_READY`` event is
   fired instead
-  Instead of having display background color and image,
   ``lv_layer_bottom()`` is added where any color can be set or any
   widget can be created.
-  The target color format can be adjusted in the display in runtime by calling
   ``lv_display_set_color_format(disp, LV_COLOR_FORMAT_...)``
-  ``LV_COLOR_16_SWAP`` is removed and ``lv_draw_sw_rgb565_swap()`` can be called manually
   in the ``flush_cb`` if needed to swap the in-place.
-  ``disp_drv.scr_transp`` was removed and
   ``lv_display_set_color_format(disp, LV_COLOR_FORMAT_NATIVE_ALPHA)``
   can be used instead
-  ``set_px_cb`` is removed. You can can convert the rendered image in the ``flush_cb``.
-  For more details check out the docs
   `here /porting/display>`__ and
   `here /overview/display>`__.

Indev API
---------

-  Similarly to the display ``lv_indev_drv_t`` was removed and an input
   device can be created like this:
-  Similarly to the widgets, now you can attach events to the indevs
   too, using ``lv_indev_add_event()``
-  The ``feedback_cb`` was removed, instead ``LV_EVENT_PRESSED/CLICKED/etc``
   events are sent to the input device

.. code:: c

   lv_indev_t * indev = lv_indev_create();
   lv_indev_set_type(indev, LV_INDEV_TYPE_...);
   lv_indev_set_read_cb(indev, read_cb);

Others
~~~~~~

-  ``lv_msg`` is removed and replaced by
   `lv_observer <https://docs.lvgl.io/master/others/observer.html>`__
-  ``lv_chart`` ticks support was removed,
   `lv_scale <https://docs.lvgl.io/master/widgets/scale.html>`__ can be
   used instead
- ``lv_msgbox`` is update to be more flexible. It uses normal button instead of button matrix
- ``lv_tabview`` was updated to user real button instead of a button matrix


v8.3
~~~~

For Other v8.3.x releases visit the `Changelog in the release/v8.3 branch <https://github.com/lvgl/lvgl/blob/release/v8.3/docs/CHANGELOG.md>`__ .

