.. _overview_image:

======
Images
======

An image can be a file or a variable which stores the bitmap itself and
some metadata.

Store images
************

You can store images in two places

- as a variable in internal memory (RAM or ROM)
- as a file

.. _overview_image_variables:

Variables
---------

Images stored internally in a variable are composed mainly of an
:cpp:struct:`lv_image_dsc_t` structure with the following fields:

- **header**:

  - *cf*: Color format. See :ref:`below <overview_image_color_formats>`
  - *w*: width in pixels (<= 2048)
  - *h*: height in pixels (<= 2048)
  - *always zero*: 3 bits which need to be always zero
  - *reserved*: reserved for future use
- **data**: pointer to an array where the image itself is stored
- **data_size**: length of ``data`` in bytes

These are usually stored within a project as C files. They are linked
into the resulting executable like any other constant data.

.. _overview_image_files:

Files
-----

To deal with files you need to add a storage *Drive* to LVGL. In short,
a *Drive* is a collection of functions (*open*, *read*, *close*, etc.)
registered in LVGL to make file operations. You can add an interface to
a standard file system (FAT32 on SD card) or you create your simple file
system to read data from an SPI Flash memory. In every case, a *Drive*
is just an abstraction to read and/or write data to memory. See the
:ref:`File system <overview_file_system>` section to learn more.

Images stored as files are not linked into the resulting executable, and
must be read into RAM before being drawn. As a result, they are not as
resource-friendly as images linked at compile time. However, they are
easier to replace without needing to rebuild the main program.

.. _overview_image_color_formats:

Color formats
*************

Various built-in color formats are supported:

- :cpp:enumerator:`LV_COLOR_FORMAT_NATIVE`: Simply stores the RGB colors (in whatever color depth LVGL is configured for).
- :cpp:enumerator:`LV_COLOR_FORMAT_NATIVE_WITH_ALPHA`: Like :cpp:enumerator:`LV_COLOR_FORMAT_NATIVE` but it also adds an alpha (transparency) byte for every pixel.
- :cpp:enumerator:`LV_COLOR_FORMAT_I1`, :cpp:enumerator:`LV_COLOR_FORMAT_I2`, :cpp:enumerator:`LV_COLOR_FORMAT_I4`, :cpp:enumerator:`LV_COLOR_FORMAT_I8`:
  Uses a palette with 2, 4, 16 or 256 colors and stores each pixel in 1, 2, 4 or 8 bits.
- :cpp:enumerator:`LV_COLOR_FORMAT_A1`, :cpp:enumerator:`LV_COLOR_FORMAT_A2`, :cpp:enumerator:`LV_COLOR_FORMAT_A4`, :cpp:enumerator:`LV_COLOR_FORMAT_A8`:
  **Only stores the Alpha value with 1, 2, 4 or 8 bits.** The pixels take the color of ``style.img_recolor`` and
  the set opacity. The source image has to be an alpha channel. This is
  ideal for bitmaps similar to fonts where the whole image is one color
  that can be altered.

The bytes of :cpp:enumerator:`LV_COLOR_FORMAT_NATIVE` images are stored in the following order.

- 32-bit color depth:
    - **Byte 0**: Blue
    - **Byte 1**: Green
    - **Byte 2**: Red
    - **Byte 3**: Alpha (only with :cpp:enumerator:`LV_COLOR_FORMAT_NATIVE_WITH_ALPHA`)
- 16-bit color depth:
    - **Byte 0**: Green 3 lower bit, Blue 5 bit
    - **Byte 1**: Red 5 bit, Green 3 higher bit
    - **Byte 2**: Alpha byte (only with :cpp:enumerator:`LV_COLOR_FORMAT_NATIVE_WITH_ALPHA`)
- 8-bit color depth:
    - **Byte 0**: Red 3 bit, Green 3 bit, Blue 2 bit
    - **Byte 2**: Alpha byte (only with :cpp:enumerator:`LV_COLOR_FORMAT_NATIVE_WITH_ALPHA`)

You can store images in a *Raw* format to indicate that it's not encoded
with one of the built-in color formats and an external :ref:`Image decoder <overview_image_decoder>`
needs to be used to decode the image.

- :cpp:enumerator:`LV_COLOR_FORMAT_RAW`: Indicates a basic raw image (e.g. a PNG or JPG image).
- :cpp:enumerator:`LV_COLOR_FORMAT_RAW_ALPHA`: Indicates that an image has alpha and an alpha byte is added for every pixel.

Add and use images
******************

You can add images to LVGL in two ways:

- using the online converter
- manually create images

Online converter
----------------

The online Image converter is available here:
https://lvgl.io/tools/imageconverter

Adding an image to LVGL via the online converter is easy.

1. You need to select a *BMP*, *PNG* or *JPG* image first.
2. Give the image a name that will be used within LVGL.
3. Select the :ref:`Color format <overview_image_color_formats>`.
4. Select the type of image you want. Choosing a binary will generate a
   ``.bin`` file that must be stored separately and read using the :ref:`file support <overview_image_files>`.
   Choosing a variable will generate a standard C file that can be linked into your project.
5. Hit the *Convert* button. Once the conversion is finished, your
   browser will automatically download the resulting file.

In the generated C arrays (variables), bitmaps for all the color depths
(1, 8, 16 or 32) are included in the C file, but only the color depth
that matches :c:macro:`LV_COLOR_DEPTH` in *lv_conf.h* will actually be linked
into the resulting executable.

In the case of binary files, you need to specify the color format you
want:

- RGB332 for 8-bit color depth
- RGB565 for 16-bit color depth
- RGB565 Swap for 16-bit color depth (two bytes are swapped)
- RGB888 for 32-bit color depth

Manually create an image
------------------------

If you are generating an image at run-time, you can craft an image
variable to display it using LVGL. For example:

.. code:: c

   uint8_t my_img_data[] = {0x00, 0x01, 0x02, ...};

   static lv_image_dsc_t my_img_dsc = {
       .header.always_zero = 0,
       .header.w = 80,
       .header.h = 60,
       .data_size = 80 * 60 * LV_COLOR_DEPTH / 8,
       .header.cf = LV_COLOR_FORMAT_NATIVE,          /*Set the color format*/
       .data = my_img_data,
   };

Another (possibly simpler) option to create and display an image at
run-time is to use the :ref:`Canvas <lv_canvas>` object.

Use images
----------

The simplest way to use an image in LVGL is to display it with an
:ref:`lv_image` object:

.. code:: c

   lv_obj_t * icon = lv_image_create(lv_screen_active(), NULL);

   /*From variable*/
   lv_image_set_src(icon, &my_icon_dsc);

   /*From file*/
   lv_image_set_src(icon, "S:my_icon.bin");

If the image was converted with the online converter, you should use
:cpp:expr:`LV_IMAGE_DECLARE(my_icon_dsc)` to declare the image in the file where
you want to use it.

.. _overview_image_decoder:

Image decoder
*************

As you can see in the :ref:`overview_image_color_formats` section, LVGL
supports several built-in image formats. In many cases, these will be
all you need. LVGL doesn't directly support, however, generic image
formats like PNG or JPG.

To handle non-built-in image formats, you need to use external libraries
and attach them to LVGL via the *Image decoder* interface.

An image decoder consists of 4 callbacks:

- **info** get some basic info about the image (width, height and color format).
- **open** open an image:
    - store a decoded image
    - set it to ``NULL`` to indicate the image can be read line-by-line.
- **get_area** if *open* didn't fully open an image this function should give back part of image as decoded data.
- **close** close an opened image, free the allocated resources.

You can add any number of image decoders. When an image needs to be
drawn, the library will try all the registered image decoders until it
finds one which can open the image, i.e. one which knows that format.

The following formats are understood by the built-in decoder:
- ``LV_COLOR_FORMAT_I1``
- ``LV_COLOR_FORMAT_I2``
- ``LV_COLOR_FORMAT_I4``
- ``LV_COLOR_FORMAT_I8``
- ``LV_COLOR_FORMAT_RGB888``
- ``LV_COLOR_FORMAT_XRGB8888``
- ``LV_COLOR_FORMAT_ARGB8888``
- ``LV_COLOR_FORMAT_RGB565``
- ``LV_COLOR_FORMAT_RGB565A8``


Custom image formats
--------------------

The easiest way to create a custom image is to use the online image
converter and select ``Raw`` or ``Raw with alpha`` format.
It will just take every byte of the
binary file you uploaded and write it as an image "bitmap". You then
need to attach an image decoder that will parse that bitmap and generate
the real, renderable bitmap.

``header.cf`` will be :cpp:enumerator:`LV_COLOR_FORMAT_RAW`, :cpp:enumerator:`LV_COLOR_FORMAT_RAW_ALPHA`
accordingly. You should choose the correct format according to your needs:
a fully opaque image, using an alpha channel.

After decoding, the *raw* formats are considered *True color* by the
library. In other words, the image decoder must decode the *Raw* images
to *True color* according to the format described in the :ref:`overview_image_color_formats` section.


Register an image decoder
-------------------------

Here's an example of getting LVGL to work with PNG images.

First, you need to create a new image decoder and set some functions to
open/close the PNG files. It should look like this:

.. code:: c

   /*Create a new decoder and register functions */
   lv_image_decoder_t * dec = lv_image_decoder_create();
   lv_image_decoder_set_info_cb(dec, decoder_info);
   lv_image_decoder_set_open_cb(dec, decoder_open);
   lv_image_decoder_set_close_cb(dec, decoder_close);


   /**
    * Get info about a PNG image
    * @param decoder   pointer to the decoder where this function belongs
    * @param src       can be file name or pointer to a C array
    * @param header    image information is set in header parameter
    * @return          LV_RESULT_OK: no error; LV_RESULT_INVALID: can't get the info
    */
   static lv_result_t decoder_info(lv_image_decoder_t * decoder, const void * src, lv_image_header_t * header)
   {
     /*Check whether the type `src` is known by the decoder*/
     if(is_png(src) == false) return LV_RESULT_INVALID;

     /* Read the PNG header and find `width` and `height` */
     ...

     header->cf = LV_COLOR_FORMAT_ARGB8888;
     header->w = width;
     header->h = height;
   }

   /**
    * Open a PNG image and decode it into dsc.decoded
    * @param decoder   pointer to the decoder where this function belongs
    * @param dsc       image descriptor
    * @return          LV_RESULT_OK: no error; LV_RESULT_INVALID: can't open the image
    */
   static lv_result_t decoder_open(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
   {
     (void) decoder; /*Unused*/

     /*Check whether the type `src` is known by the decoder*/
     if(is_png(dsc->src) == false) return LV_RESULT_INVALID;

     /*Decode and store the image. If `dsc->decoded` is `NULL`, the `read_line` function will be called to get the image data line-by-line*/
     dsc->decoded = my_png_decoder(dsc->src);

     /*Change the color format if decoded image format is different than original format. For PNG it's usually decoded to ARGB8888 format*/
     dsc->decoded.header.cf = LV_COLOR_FORMAT_...

     /*Call a binary image decoder function if required. It's not required if `my_png_decoder` opened the image in true color format.*/
     lv_result_t res = lv_bin_decoder_open(decoder, dsc);

     return res;
   }

   /**
    * Decode an area of image
    * @param decoder      pointer to the decoder where this function belongs
    * @param dsc          image decoder descriptor
    * @param full_area    full image area information
    * @param decoded_area area information to decode (x1, y1, x2, y2)
    * @return             LV_RESULT_OK: no error; LV_RESULT_INVALID: can't decode image area
    */
   static lv_result_t decoder_get_area(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc,
                                    const lv_area_t * full_area, lv_area_t * decoded_area)
   {
   }

   /**
    * Close PNG image and free data
    * @param decoder   pointer to the decoder where this function belongs
    * @param dsc       image decoder descriptor
    * @return          LV_RESULT_OK: no error; LV_RESULT_INVALID: can't open the image
    */
   static void decoder_close(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
   {
     /*Free all allocated data*/

     /*Call the built-in close function if the built-in open/read_line was used*/
     lv_bin_decoder_close(decoder, dsc);

   }

So in summary:

- In ``decoder_info``, you should collect some basic information about the image and store it in ``header``.
- In ``decoder_open``, you should try to open the image source pointed by
  ``dsc->src``. Its type is already in ``dsc->src_type == LV_IMG_SRC_FILE/VARIABLE``.
  If this format/type is not supported by the decoder, return :cpp:enumerator:`LV_RESULT_INVALID`.
  However, if you can open the image, a pointer to the decoded image should be
  set in ``dsc->decoded``. If the format is known, but you don't want to
  decode the entire image (e.g. no memory for it), set ``dsc->decoded = NULL`` and
  use ``decoder_get_area`` to get the image area pixels.
- In ``decoder_close`` you should free all allocated resources.
- ``decoder_get_area`` is optional. In this case you should decode the whole image In
  ``decoder_open`` function and store image data in ``dsc->decoded``.
  Decoding the whole image requires extra memory and some computational overhead.


Manually use an image decoder
-----------------------------

LVGL will use registered image decoders automatically if you try and
draw a raw image (i.e. using the ``lv_image`` object) but you can use them
manually too. Create an :cpp:type:`lv_image_decoder_dsc_t` variable to describe
the decoding session and call :cpp:func:`lv_image_decoder_open`.

The ``color`` parameter is used only with ``LV_COLOR_FORMAT_A1/2/4/8``
images to tell color of the image.

.. code:: c


   lv_result_t res;
   lv_image_decoder_dsc_t dsc;
   lv_image_decoder_args_t args = { 0 }; /*Custom decoder behavior via args*/
   res = lv_image_decoder_open(&dsc, &my_img_dsc, &args);

   if(res == LV_RESULT_OK) {
     /*Do something with `dsc->decoded`. You can copy out the decoded image by `lv_draw_buf_dup(dsc.decoded)`*/
     lv_image_decoder_close(&dsc);
   }


Image post-processing
---------------------

Considering that some hardware has special requirements for image formats,
such as alpha premultiplication and stride alignment, most image decoders (such as PNG decoders)
may not directly output image data that meets hardware requirements.

For this reason, LVGL provides a solution for image post-processing.
First, call a custom post-processing function after ``lv_image_decoder_open`` to adjust the data in the image cache,
and then mark the processing status in ``cache_entry->process_state`` (to avoid repeated post-processing).

See the detailed code below:

- Stride alignment and premultiply post-processing example:

.. code:: c

   /* Define post-processing state */
   typedef enum {
     IMAGE_PROCESS_STATE_NONE = 0,
     IMAGE_PROCESS_STATE_STRIDE_ALIGNED = 1 << 0,
     IMAGE_PROCESS_STATE_PREMULTIPLIED_ALPHA = 1 << 1,
   } image_process_state_t;

   lv_result_t my_image_post_process(lv_image_decoder_dsc_t * dsc)
   {
     lv_color_format_t color_format = dsc->header.cf;
     lv_result_t res = LV_RESULT_OK;

     if(color_format == LV_COLOR_FORMAT_ARGB8888) {
       lv_cache_lock();
       lv_cache_entry_t * entry = dsc->cache_entry;

       if(!(entry->process_state & IMAGE_PROCESS_STATE_PREMULTIPLIED_ALPHA)) {
         lv_draw_buf_premultiply(dsc->decoded);
         LV_LOG_USER("premultiplied alpha OK");

         entry->process_state |= IMAGE_PROCESS_STATE_PREMULTIPLIED_ALPHA;
       }

       if(!(entry->process_state & IMAGE_PROCESS_STATE_STRIDE_ALIGNED)) {
          uint32_t stride_expect = lv_draw_buf_width_to_stride(decoded->header.w, decoded->header.cf);
          if(decoded->header.stride != stride_expect) {
              LV_LOG_WARN("Stride mismatch");
              lv_draw_buf_t * aligned = lv_draw_buf_adjust_stride(decoded, stride_expect);
              if(aligned == NULL) {
                  LV_LOG_ERROR("No memory for Stride adjust.");
                  return NULL;
              }

              decoded = aligned;
          }

          entry->process_state |= IMAGE_PROCESS_STATE_STRIDE_ALIGNED;
       }

   alloc_failed:
       lv_cache_unlock();
     }

     return res;
   }

- GPU draw unit example:

.. code:: c

  void gpu_draw_image(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc, const lv_area_t * coords)
  {
    ...
    lv_image_decoder_dsc_t decoder_dsc;
    lv_result_t res = lv_image_decoder_open(&decoder_dsc, draw_dsc->src, NULL);
    if(res != LV_RESULT_OK) {
      LV_LOG_ERROR("Failed to open image");
      return;
    }

    res = my_image_post_process(&decoder_dsc);
    if(res != LV_RESULT_OK) {
      LV_LOG_ERROR("Failed to post-process image");
      return;
    }
    ...
  }

.. _overview_image_caching:

Image caching
*************

Sometimes it takes a lot of time to open an image. Continuously decoding
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

Cache size
----------

The size of cache (in bytes) can be defined with
:c:macro:`LV_CACHE_DEF_SIZE` in *lv_conf.h*. The default value is 0, so
no image is cached.

The size of cache can be changed at run-time with
:cpp:expr:`lv_cache_set_max_size(size_t size)`,
and get with :cpp:expr:`lv_cache_get_max_size()`.

Value of images
---------------

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

Memory usage
------------

Note that a cached image might continuously consume memory. For example,
if three PNG images are cached, they will consume memory while they are
open.

Therefore, it's the user's responsibility to be sure there is enough RAM
to cache even the largest images at the same time.

Clean the cache
---------------

Let's say you have loaded a PNG image into a :cpp:struct:`lv_image_dsc_t` ``my_png``
variable and use it in an ``lv_image`` object. If the image is already
cached and you then change the underlying PNG file, you need to notify
LVGL to cache the image again. Otherwise, there is no easy way of
detecting that the underlying file changed and LVGL will still draw the
old image from cache.

To do this, use :cpp:expr:`lv_cache_invalidate(lv_cache_find(&my_png, LV_CACHE_SRC_TYPE_PTR, 0, 0));`.

Custom cache algorithm
----------------------

If you want to implement your own cache algorithm, you can refer to the
following code to replace the LVGL built-in cache manager:

.. code:: c

   static lv_cache_entry_t * my_cache_add_cb(size_t size)
   {
     ...
   }

   static lv_cache_entry_t * my_cache_find_cb(const void * src, lv_cache_src_type_t src_type, uint32_t param1, uint32_t param2)
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
    /*Initialize new cache manager.*/
    lv_cache_manager_t my_manager;
    my_manager.add_cb = my_cache_add_cb;
    my_manager.find_cb = my_cache_find_cb;
    my_manager.invalidate_cb = my_cache_invalidate_cb;
    my_manager.get_data_cb = my_cache_get_data_cb;
    my_manager.release_cb = my_cache_release_cb;
    my_manager.set_max_size_cb = my_cache_set_max_size_cb;
    my_manager.empty_cb = my_cache_empty_cb;

    /*Replace existing cache manager with the new one.*/
    lv_cache_lock();
    lv_cache_set_manager(&my_manager);
    lv_cache_unlock();
   }

.. _overview_image_api:

API
***
