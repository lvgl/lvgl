.. _snapshot:

========
Snapshot
========

Snapshot provides an API to take a snapshot image for an LVGL Widget together
with its children.  The image will look exactly like the Widget on the display.



.. _snapshot_usage:

Usage
*****

Simply call function :cpp:expr:`lv_snapshot_take(widget, color_format)` to generate
the image descriptor which can be used as an Image Widget's image source using
:cpp:func:`lv_image_set_src`.

Note, only following color formats are supported at this time:

- :cpp:enumerator:`LV_COLOR_FORMAT_RGB565`
- :cpp:enumerator:`LV_COLOR_FORMAT_RGB888`
- :cpp:enumerator:`LV_COLOR_FORMAT_XRGB8888`
- :cpp:enumerator:`LV_COLOR_FORMAT_ARGB8888`

Freeing the Image
-----------------

The memory :cpp:func:`lv_snapshot_take` uses is dynamically allocated using
:cpp:func:`lv_draw_buf_create`. Use :cpp:func:`lv_draw_buf_destroy` to free the
memory it allocated.

The snapshot image which is the draw buffer returned by :cpp:func:`lv_snapshot_take`
normally won't be added to the cache because it can be drawn directly. So you don't need
to invalidate the cache by calling :cpp:func:`lv_image_cache_drop` before destroying
the draw buffer.

The below code snippet demonstrates correct use of :cpp:func:`lv_snapshot_take`:

.. code-block:: c

   void update_snapshot(lv_obj_t * widget, lv_obj_t * img_snapshot)
   {
       lv_draw_buf_t* snapshot = (void*)lv_image_get_src(img_snapshot);
       if(snapshot) {
           lv_draw_buf_destroy(snapshot);
       }
       snapshot = lv_snapshot_take(widget, LV_COLOR_FORMAT_ARGB8888);
       lv_image_set_src(img_snapshot, snapshot);
   }

Using an Existing Buffer
------------------------

If the snapshot needs to be updated repeatedly, or if the caller provides the draw
buffer, use :cpp:expr:`lv_snapshot_take_to_draw_buf(widget, color_format, draw_buf)`.
In this case, the caller is responsible for creating and destroying the draw buffer.

If snapshot is generated successfully, the image descriptor is updated,
the image data will be stored to the provided ``draw_buf``, and the function will
return :cpp:enumerator:`LV_RESULT_OK`.

Note that snapshot may fail if the provided buffer is not large enough, which can
happen if the Widget's size changes.  It's recommended to use
:cpp:expr:`lv_snapshot_reshape_draw_buf(widget, draw_buf)` to first ensure the buffer
is large enough, and if it fails, destroy the existing draw buffer and call
`lv_snapshot_take` directly.



.. _snapshot_example:

Example
*******

.. include:: ../../examples/others/snapshot/index.rst



.. _snapshot_api:

API
***

