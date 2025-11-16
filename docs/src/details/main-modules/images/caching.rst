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
images. Instead, the library will close one of the cached images to free space.

To decide which image to close, LVGL uses an LRU (least-recently-used) algorithm.
Most-recently-used images are prioritized to keep in the cache as long as possible,
while the oldest (images not used recently) are disposed of to make room for new
cache content.



Memory Usage
************

Note that a cached image might continuously consume memory. For example,
if three PNG images are cached, they will consume memory while they are
open.

Therefore, it's the user's responsibility to be sure there is enough RAM
to cache even the largest images at the same time.



Invalidating Cache Entries
**************************

Let's say you have loaded a PNG image into a :cpp:struct:`lv_image_dsc_t` ``my_png``
variable and use it in an ``lv_image`` Widget. If the image is already
cached and you then change the underlying PNG file, you need to notify
LVGL to cache the image again. Otherwise, there is no easy way of
detecting that the underlying file has changed and LVGL will still draw the
old image from cache.  To do this, use :cpp:expr:`lv_image_cache_drop(&my_png)`.

To invalidate all cached images:  :cpp:expr:`lv_image_cache_drop(NULL)`.

