.. _snapshot:

========
Snapshot
========

Snapshot provides API to take snapshot image for LVGL object together
with its children. The image will look exactly like the object on display.

.. _snapshot_usage:

Usage
-----

Simply call API :cpp:func:`lv_snapshot_take` to generate the image descriptor
which can be set as image object src using :cpp:func:`lv_image_set_src`.

Note, only following color formats are supported for now:

- :cpp:enumerator:`LV_COLOR_FORMAT_RGB565`
- :cpp:enumerator:`LV_COLOR_FORMAT_RGB888`
- :cpp:enumerator:`LV_COLOR_FORMAT_XRGB8888`
- :cpp:enumerator:`LV_COLOR_FORMAT_ARGB8888`

Free the Image
~~~~~~~~~~~~~~

The memory :cpp:func:`lv_snapshot_take` uses are dynamically allocated using
:cpp:func:`lv_malloc`. Use API :cpp:func:`lv_snapshot_free` to free the memory it
takes. This will firstly free memory the image data takes, then the
image descriptor.

Take caution to free the snapshot but not delete the image object.
Before free the memory, be sure to firstly unlink it from image object,
using :cpp:expr:`lv_image_set_src(NULL)` and :cpp:expr:`lv_cache_invalidate(lv_cache_find(src, LV_CACHE_SRC_TYPE_PTR, 0, 0));`.

Below code snippet explains usage of this API.

.. code:: c

   void update_snapshot(lv_obj_t * obj, lv_obj_t * img_snapshot)
   {
       lv_image_dsc_t* snapshot = (void*)lv_image_get_src(img_snapshot);
       if(snapshot) {
           lv_snapshot_free(snapshot);
       }
       snapshot = lv_snapshot_take(obj, LV_COLOR_FORMAT_ARGB8888);
       lv_image_set_src(img_snapshot, snapshot);
   }

Use Existing Buffer
~~~~~~~~~~~~~~~~~~~

If the snapshot needs update now and then, or simply caller provides memory, use API
``lv_result_t lv_snapshot_take_to_buf(lv_obj_t * obj, lv_color_format_t cf, lv_image_dsc_t * dsc, void * buf, uint32_t buf_size);``
for this case. It's caller's responsibility to alloc/free the memory.

If snapshot is generated successfully, the image descriptor is updated
and image data will be stored to provided ``buf``.

Note that snapshot may fail if provided buffer is not enough, which may
happen when object size changes. It's recommended to use API
:cpp:func:`lv_snapshot_buf_size_needed` to check the needed buffer size in byte
firstly and resize the buffer accordingly.

.. _snapshot_example:

Example
-------

.. include:: ../examples/others/snapshot/index.rst

.. _snapshot_api:

API
---

