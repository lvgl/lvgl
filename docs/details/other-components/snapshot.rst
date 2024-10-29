.. _snapshot:

========
Snapshot
========

Snapshot provides API to take snapshot image for LVGL Widget together
with its children. The image will look exactly like the Widget on display.

.. _snapshot_usage:

Usage
-----

Simply call API :cpp:func:`lv_snapshot_take` to generate the image descriptor
which can be set as image Widget src using :cpp:func:`lv_image_set_src`.

Note, only following color formats are supported for now:

- :cpp:enumerator:`LV_COLOR_FORMAT_RGB565`
- :cpp:enumerator:`LV_COLOR_FORMAT_RGB888`
- :cpp:enumerator:`LV_COLOR_FORMAT_XRGB8888`
- :cpp:enumerator:`LV_COLOR_FORMAT_ARGB8888`

Free the Image
~~~~~~~~~~~~~~

The memory :cpp:func:`lv_snapshot_take` uses are dynamically allocated using
:cpp:func:`lv_draw_buf_create`. Use API :cpp:func:`lv_draw_buf_destroy` to free the memory it
takes. This will firstly free memory the image data takes, then the
image descriptor.

The snapshot image which is the draw buffer returned by :cpp:func:`lv_snapshot_take`
normally won't be added to cache because it can be drawn directly. So you don't need
to invalidate cache by :cpp:func:`lv_image_cache_drop` before destroy the draw buffer.

Below code snippet explains usage of this API.

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

Use Existing Buffer
~~~~~~~~~~~~~~~~~~~

If the snapshot needs update now and then, or simply caller provides memory, use API
``lv_result_t lv_snapshot_take_to_draw_buf(lv_obj_t * widget, lv_color_format_t cf, lv_draw_buf_t * draw_buf);``
for this case. It's caller's responsibility to create and destroy the draw buffer.

If snapshot is generated successfully, the image descriptor is updated
and image data will be stored to provided ``buf``.

Note that snapshot may fail if provided buffer is not enough, which may
happen when Widget size changes. It's recommended to use API
:cpp:func:`lv_snapshot_reshape_draw_buf` to prepare the buffer firstly and if it
fails, destroy the existing draw buffer and call `lv_snapshot_take` directly.

.. _snapshot_example:

Example
-------

.. include:: ../../examples/others/snapshot/index.rst

.. _snapshot_api:

API
---

