.. _image caching:

=============
Image Caching
=============

Sometimes it takes a lot of time to open an image. Repeatedly decoding
a PNG/JPEG image or loading images from a slow external memory would be
inefficient and detrimental to the user experience.

Therefore, LVGL caches image data. Caching means some
images will be left open, hence LVGL can quickly access them from
``dsc->decoded`` instead of needing to decode them again.

Of course, caching images is resource intensive as it uses more RAM to
store the decoded image. LVGL tries to optimize the process as much as
possible (see below), but you will still need to evaluate if this would
be beneficial for your platform or not. Image caching may not be worth
it if you have a deeply embedded target which decodes small images from
a relatively fast storage medium.



Cache Size
**********

The size of cache (in bytes) can be defined with
:c:macro:`LV_CACHE_DEF_SIZE` in *lv_conf.h*. The default value is 0, so
no image is cached.

The size of cache can be changed at run-time with
:cpp:expr:`lv_cache_set_max_size(size_t size)`,
and retrieved with :cpp:expr:`lv_cache_get_max_size()`.



Value of Images
***************

When you use more images than available cache size, LVGL can't cache all the
images. Instead, the library will close one of the cached images to free
space.

To decide which image to close, LVGL uses a measurement it previously
made of how long it took to open the image. Cache entries that hold
slower-to-open images are considered more valuable and are kept in the
cache as long as possible.

If you want or need to override LVGL's measurement, you can manually set
the *weight* value in the cache entry in
``cache_entry->weight = time_ms`` to give a higher or lower value. (Leave
it unchanged to let LVGL control it.)

Every cache entry has a *"life"* value. Every time an image is opened
through the cache, the *life* value of all entries is increased by their
*weight* values to make them older.
When a cached image is used, its *usage_count* value is increased
to make it more alive.

If there is no more space in the cache, the entry with *usage_count == 0*
and lowest life value will be dropped.



Memory Usage
************

Note that a cached image might continuously consume memory. For example,
if three PNG images are cached, they will consume memory while they are
open.

Therefore, it's the user's responsibility to be sure there is enough RAM
to cache even the largest images at the same time.



Cleaning the Cache
******************

Let's say you have loaded a PNG image into a :cpp:struct:`lv_image_dsc_t` ``my_png``
variable and use it in an ``lv_image`` Widget. If the image is already
cached and you then change the underlying PNG file, you need to notify
LVGL to cache the image again. Otherwise, there is no easy way of
detecting that the underlying file has changed and LVGL will still draw the
old image from cache.

To do this, use
:cpp:expr:`lv_cache_invalidate(lv_cache_find(&my_png, LV_CACHE_SRC_TYPE_PTR, 0, 0))`.



Custom Cache Algorithm
**********************

If you want to implement your own cache algorithm, you can refer to the
following code to replace the LVGL built-in cache manager:

.. code-block:: c

    static lv_cache_entry_t * my_cache_add_cb(size_t size)
    {
        ...
    }

    static lv_cache_entry_t * my_cache_find_cb(const void * src, lv_cache_src_type_t src_type,
                                               uint32_t param1, uint32_t param2)
    {
        ...
    }

    static void my_cache_invalidate_cb(lv_cache_entry_t * entry)
    {
        ...
    }

    static const void * my_cache_get_data_cb(lv_cache_entry_t * entry)
    {
        ...
    }

    static void my_cache_release_cb(lv_cache_entry_t * entry)
    {
        ...
    }

    static void my_cache_set_max_size_cb(size_t new_size)
    {
        ...
    }

    static void my_cache_empty_cb(void)
    {
        ...
    }

    void my_cache_init(void)
    {
        /* Initialize new cache manager. */
        lv_cache_manager_t my_manager;
        my_manager.add_cb = my_cache_add_cb;
        my_manager.find_cb = my_cache_find_cb;
        my_manager.invalidate_cb = my_cache_invalidate_cb;
        my_manager.get_data_cb = my_cache_get_data_cb;
        my_manager.release_cb = my_cache_release_cb;
        my_manager.set_max_size_cb = my_cache_set_max_size_cb;
        my_manager.empty_cb = my_cache_empty_cb;

        /* Replace existing cache manager with the new one. */
        lv_cache_lock();
        lv_cache_set_manager(&my_manager);
        lv_cache_unlock();
    }

