.. _image_decoders:

==============
Image Decoders
==============

What is an Image Decoder?
*************************

Images that are encoded (i.e. outside of the list of built-in supported
:ref:`images_color_formats`) are dealt with through an "Image Decoder".  A Decoder
is very simply a body of logic that can convert a coded image into one of the
recognized formats.



.. _built-in image decoders:

Built-In Image Decoders
***********************

LVGL comes with a number of image decoders to support generic image formats:

.. container:: tighter-table-3

    +--------+----------------------+--------------------+
    | Format | ``lv_conf.h``        | Reference          |
    |        | Symbol to Set        |                    |
    +========+======================+====================+
    | BMP    | LV_USE_BMP           | :ref:`bmp`         |
    +--------+----------------------+--------------------+
    | PNG    | LV_USE_LODEPNG       | :ref:`lodepng_rst` |
    +--------+----------------------+--------------------+
    | PNG    | LV_USE_LIBPNG        | :ref:`libpng`      |
    +--------+----------------------+--------------------+
    | JPG    | LV_USE_LIBJPEG_TURBO | :ref:`libjpeg`     |
    +--------+----------------------+--------------------+
    | JPG    | LV_USE_TJPGD         | :ref:`tjpgd`       |
    +--------+----------------------+--------------------+
    | WEBP   | LV_USE_LIBWEBP       | :ref:`libwebp`     |
    +--------+----------------------+--------------------+
    | SVG    | LV_USE_SVG           | :ref:`svg`         |
    +--------+----------------------+--------------------+

Once you have the appropriate symbol set to ``1`` in ``lv_conf.h``, you simply pass
the file-system path to the file containing your image as the ``src`` argument to
:cpp:expr:`lv_image_set_src(icon, "S:my_icon.png")`, and LVGL takes care of the rest.



Using Custom Image Formats
**************************

If you have a file-based image type that is not in this list, you can set up LVGL to
successfully handle it by implementing a custom image decoder.  You can use an
external decoding library or write your own.  To "connect" it to LVGL, you simply
use LVGL's *Image Decoder* interface.

An image decoder consists of 4 callbacks:

:info:     get some basic info about the image (width, height and color format).
:open:     open an image:

           - store a decoded image
           - set it to ``NULL`` to indicate the image can be read line-by-line.

:get_area: if *open* didn't fully open an image this function should give back part
           of image as decoded data.
:close:    close an opened image, and free the allocated resources.

You can add any number of image decoders. When an image needs to be
drawn, the library will try all the registered image decoders until it
finds one which can open the image, i.e. one which knows that format.

The following formats are understood by the built-in decoder:

- :cpp:enumerator:`LV_COLOR_FORMAT_I1`
- :cpp:enumerator:`LV_COLOR_FORMAT_I2`
- :cpp:enumerator:`LV_COLOR_FORMAT_I4`
- :cpp:enumerator:`LV_COLOR_FORMAT_I8`
- :cpp:enumerator:`LV_COLOR_FORMAT_RGB888`
- :cpp:enumerator:`LV_COLOR_FORMAT_XRGB8888`
- :cpp:enumerator:`LV_COLOR_FORMAT_ARGB8888`
- :cpp:enumerator:`LV_COLOR_FORMAT_RGB565`
- :cpp:enumerator:`LV_COLOR_FORMAT_RGB565A8`


.. _custom_image_formats:

Custom Image Formats
--------------------

The easiest way to create a custom image is to use the online image converter and
select ``RAW`` or ``RAW_WITH_ALPHA`` format. It will just take every byte of the
binary file you uploaded and write it as an image "bitmap". You then need to attach
an image decoder that will parse that bitmap and generate the real, render-able
bitmap.

``header.cf`` will be :cpp:enumerator:`LV_COLOR_FORMAT_RAW`,
:cpp:enumerator:`LV_COLOR_FORMAT_RAW_ALPHA` accordingly.  You should choose the
correct format according to your needs:  a fully opaque image, using an alpha channel.

The decoded format of a RAW image depends on the decoder.  Example:  JPG images are
decoded to RGB888 and PNG images are decoded to ARGB8888.  See
:ref:`images_color_formats` for more details.


Registering an Image Decoder
----------------------------

Here's an example of getting LVGL to work with PNG images.

First, you need to create a new image decoder and set some functions to
open/close the PNG files. It should look like this:

.. code-block:: c

   /* Create a new decoder and register functions */
   lv_image_decoder_t * dec = lv_image_decoder_create();
   lv_image_decoder_set_info_cb(dec, decoder_info);
   lv_image_decoder_set_open_cb(dec, decoder_open);
   lv_image_decoder_set_get_area_cb(dec, decoder_get_area);
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
     /* Check whether the type `src` is known by the decoder */
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
     (void) decoder; /* Unused */

     /* Check whether the type `src` is known by the decoder */
     if(is_png(dsc->src) == false) return LV_RESULT_INVALID;

     /* Decode and store the image. If `dsc->decoded` is `NULL`, the `decoder_get_area` function will be called to get the image data line-by-line */
     dsc->decoded = my_png_decoder(dsc->src);

     /* Change the color format if decoded image format is different than original format. For PNG it's usually decoded to ARGB8888 format */
     dsc->decoded.header.cf = LV_COLOR_FORMAT_...

     /* Call a binary image decoder function if required. It's not required if `my_png_decoder` opened the image in ARGB8888 format. */
     lv_result_t res = lv_bin_decoder_open(decoder, dsc);

     return res;
   }

   /**
    * Decode an area of image
    * @param decoder      pointer to the decoder where this function belongs
    * @param dsc          image decoder descriptor
    * @param full_area    input parameter. the full area to decode after enough subsequent calls
    * @param decoded_area input+output parameter. set the values to `LV_COORD_MIN` for the first call and to reset decoding.
    *                     the decoded area is stored here after each call.
    * @return             LV_RESULT_OK: ok; LV_RESULT_INVALID: failed or there is nothing left to decode
    */
   static lv_result_t decoder_get_area(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc,
                                    const lv_area_t * full_area, lv_area_t * decoded_area)
   {
     /**
     * If `dsc->decoded` is always set in `decoder_open` then `decoder_get_area` does not need to be implemented.
     * If `dsc->decoded` is only sometimes set or never set in `decoder_open` then `decoder_get_area` is used to
     * incrementally decode the image by calling it repeatedly until it returns `LV_RESULT_INVALID`.
     * In the example below the image is decoded line-by-line but the decoded area can have any shape and size
     * depending on the requirements and capabilities of the image decoder.
     */

     my_decoder_data_t * my_decoder_data = dsc->user_data;

     /* if `decoded_area` has a field set to `LV_COORD_MIN` then reset decoding */
     if(decoded_area->y1 == LV_COORD_MIN) {
       decoded_area->x1 = full_area->x1;
       decoded_area->x2 = full_area->x2;
       decoded_area->y1 = full_area->y1;
       decoded_area->y2 = decoded_area->y1; /* decode line-by-line, starting with the first line */

       /* create a draw buf the size of one line */
       bool reshape_success = NULL != lv_draw_buf_reshape(my_decoder_data->partial,
                                                          dsc->decoded.header.cf,
                                                          lv_area_get_width(full_area),
                                                          1,
                                                          LV_STRIDE_AUTO);
       if(!reshape_success) {
         lv_draw_buf_destroy(my_decoder_data->partial);
         my_decoder_data->partial = lv_draw_buf_create(lv_area_get_width(full_area),
                                                       1,
                                                       dsc->decoded.header.cf,
                                                       LV_STRIDE_AUTO);

         my_png_decode_line_reset(full_area);
       }
     }
     /* otherwise decoding is already in progress. decode the next line */
     else {
       /* all lines have already been decoded. indicate completion by returning `LV_RESULT_INVALID` */
       if (decoded_area->y1 >= full_area->y2) return LV_RESULT_INVALID;
       decoded_area->y1++;
       decoded_area->y2++;
     }

     my_png_decode_line(my_decoder_data->partial);

     return LV_RESULT_OK;
   }

   /**
    * Close PNG image and free data
    * @param decoder   pointer to the decoder where this function belongs
    * @param dsc       image decoder descriptor
    * @return          LV_RESULT_OK: no error; LV_RESULT_INVALID: can't open the image
    */
   static void decoder_close(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
   {
     /* Free all allocated data */
     my_png_cleanup();

     my_decoder_data_t * my_decoder_data = dsc->user_data;
     lv_draw_buf_destroy(my_decoder_data->partial);

     /* Call the built-in close function if the built-in open/get_area was used */
     lv_bin_decoder_close(decoder, dsc);

   }

In summary:

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


Manually Using an Image Decoder
-------------------------------

LVGL will use registered image decoders automatically if you try and
draw a raw image (i.e. using the ``lv_image`` Widget) but you can use them
manually as well. Create an :cpp:type:`lv_image_decoder_dsc_t` variable to describe
the decoding session and call :cpp:func:`lv_image_decoder_open`.

The ``color`` parameter is used only with ``LV_COLOR_FORMAT_A1/2/4/8``
images to tell color of the image.

.. code-block:: c


   lv_result_t res;
   lv_image_decoder_dsc_t dsc;
   lv_image_decoder_args_t args = { 0 }; /* Custom decoder behavior via args */
   res = lv_image_decoder_open(&dsc, &my_img_dsc, &args);

   if(res == LV_RESULT_OK) {
     /* Do something with `dsc->decoded`. You can copy out the decoded image by `lv_draw_buf_dup(dsc.decoded)`*/
     lv_image_decoder_close(&dsc);
   }


Image Post-Processing
---------------------

Considering that some hardware has special requirements for image formats, such as
alpha premultiplication and stride alignment, most image decoders (such as PNG
decoders) may not directly output image data that meets hardware requirements.

For this reason, LVGL provides a solution for image post-processing.  First, call a
custom post-processing function after ``lv_image_decoder_open`` to adjust the data in
the image cache, and then mark the processing status in
``cache_entry->process_state`` (to avoid repeated post-processing).

See the detailed code below:

- Stride alignment and premultiply post-processing example:

.. code-block:: c

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

.. code-block:: c

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
