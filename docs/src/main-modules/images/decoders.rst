.. _image_decoders:

==============
Image Decoders
==============

What is an Image Decoder?
*************************

Images that are encoded (i.e. outside of the list of built-in supported
:ref:`images_color_formats`) are dealt with through an Image Decoder.  An Image
Decoder is a body of logic that can convert a coded image into one of the
recognized formats.



.. _built-in image decoders:

Built-In Image Decoders
***********************

LVGL comes with a number of image decoders to support a number of popular image formats:

.. container:: tighter-table-5

    +---------+----------------------+--------------------+-----------+-----------------------+---------------------+
    | Format  | ``lv_conf.h``        | Reference          | External  | Form                  | RAM Cost            |
    |         | Symbol to Set        |                    | Library   |                       |                     |
    |         |                      |                    | Required? |                       |                     |
    +=========+======================+====================+===========+=======================+=====================+
    | BMP     | LV_USE_BMP           | :ref:`bmp`         | No        | File only             | Low (1 draw buffer) |
    +---------+----------------------+--------------------+-----------+-----------------------+---------------------+
    | PNG     | LV_USE_LODEPNG       | :ref:`lodepng_rst` | No        | File or variable      | Full image          |
    +---------+----------------------+--------------------+-----------+-----------------------+---------------------+
    | PNG     | LV_USE_LIBPNG        | :ref:`libpng`      | Yes       | File or variable      | Full image          |
    +---------+----------------------+--------------------+-----------+-----------------------+---------------------+
    | JPG     | LV_USE_LIBJPEG_TURBO | :ref:`libjpeg`     | Yes       | File only             | 8x8 Pixel Tiles     |
    +---------+----------------------+--------------------+-----------+-----------------------+---------------------+
    | JPG     | LV_USE_TJPGD         | :ref:`tjpgd`       | Yes       | File or variable      | 8x8 Pixel Tiles     |
    +---------+----------------------+--------------------+-----------+-----------------------+---------------------+
    | WEBP    | LV_USE_LIBWEBP       | :ref:`libwebp`     | Yes       | File only             | Full image          |
    +---------+----------------------+--------------------+-----------+-----------------------+---------------------+
    | SVG     | LV_USE_SVG           | :ref:`svg`         | No        | File or variable      | Full image          |
    +---------+----------------------+--------------------+-----------+-----------------------+---------------------+
    | GIF     | LV_USE_GIF           | :ref:`gif`         | No        | Use GIF Widget (File) | Widget + 1 frame    |
    +---------+----------------------+--------------------+-----------+-----------------------+---------------------+
    | Lottie  | See reference        | :ref:`lv_lottie`   | No        | Use Lottie Widget     | Widget + 1 frame    |
    +---------+----------------------+--------------------+-----------+-----------------------+---------------------+

Once you have the appropriate symbol set to ``1`` in ``lv_conf.h``, to use a file,
simply make the file accessible on an external storage device.  You will need to then
pass the :ref:`file-system <file_system>` path to the file containing your image as
the ``src`` argument to :cpp:expr:`lv_image_set_src(icon, "S:my_icon.png")`, and LVGL
takes care of the rest.

To use the encoded file as a variable, choose one of these approaches:

- Use the online- or offline converter to convert the file to an
  :cpp:type:`lv_image_dsc_t` object + data into a ``.c`` file and compile and link it
  into your project.  The color format is stored in the ``header.cf`` field of the
  :cpp:type:`lv_image_dsc_t` struct.

- There could be case where it was needed to load an image file into
  dynamically-allocated RAM at run-time and free it later, thus avoiding storing the
  images as part of the application.  In this case, you could manually create a
  :cpp:type:`lv_image_dsc_t` and point the ``data`` field to the byte array containing
  the file content and set the ``header.cf`` field to :c:macro:`LV_COLOR_FORMAT_RAW`
  or :c:macro:`LV_COLOR_FORMAT_RAW_ALPHA`, and set the image source to this
  :cpp:type:`lv_image_dsc_t` object using :cpp:expr:`lv_image_set_src(icon, &my_img_dsc)`,
  and when it is time to decode, the registered decoder that recognizes the image
  format will be used to decode it.

  The types having the word "variable" in the "Form" column in the table above would
  support this approach if it was needed.



Using Custom Image Formats
**************************

If you have a file-based image type that is not in the above list, you can set up LVGL
to successfully handle it by implementing a custom image decoder.  You can use an
external decoding library or write your own.  To "connect" it to LVGL, use
LVGL's *Image Decoder* interface.

An image decoder consists of 4 callbacks:

:info:     Get some basic info about the image (width, height and color format).
:open:     Open an image:

           - optionally store entire decoded image;
           - set ``dsc->decoded`` to ``NULL`` to indicate the image can be decoded incrementally;
           - return :cpp:enumerator:`LV_RESULT_OK` if decoder can decode the given
             image, :cpp:enumerator:`LV_RESULT_INVALID` otherwise.  (This is normally
             done by reading the image header from the file and determining
             compatibility by reading the header content.)

:get_area: If *open* didn't fully open an image this function should decode the
           indicated area of the image into the draw buffer.
:close:    Close an opened image, and free the allocated resources.

You can add any number of image decoders.  When an image needs to be drawn, the
library will try all the registered image decoders until it finds one which can open
the image, i.e. one which knows that format.

The built-in decoder understands all the formats in :ref:`images_color_formats` minus
the ``RAW`` formats.


.. _custom_image_formats:

Custom Image Formats
--------------------

The easiest way to create a custom image is to use the online image converter and
select ``RAW`` or ``RAW_WITH_ALPHA`` format. It will just take every byte of the
binary file you uploaded and write it as an image "bitmap". You then need to attach
an image decoder that will parse that bitmap and generate the real, render-able
bitmap.

``header.cf`` will be :cpp:enumerator:`LV_COLOR_FORMAT_RAW`,
:cpp:enumerator:`LV_COLOR_FORMAT_RAW_ALPHA` accordingly.  Use the format according
to your needs:  a fully opaque image, or one using an alpha channel.

The decoded format of a RAW image depends on the decoder.  Example:  JPG images are
decoded to RGB888 and PNG images are decoded to ARGB8888.  See
:ref:`images_color_formats` for more details.


Registering an Image Decoder
----------------------------

Here's an example of getting LVGL to work with a custom format using the PNG decoder
as an example.  In ``lv_libpng.c``, see the following functions as examples to follow.

.. container:: tighter-table-1

    +----------------------------------------------+-------------------------+
    | Action                                       | Function                |
    +==============================================+=========================+
    | Create and register image decoder            | lv_libpng_init()        |
    +----------------------------------------------+-------------------------+
    | De-initialize image decoder                  | lv_libpng_deinit()      |
    +----------------------------------------------+-------------------------+
    | Gather basic information about the image     | decoder_info()          |
    | and store it in ``header``.                  |                         |
    +----------------------------------------------+-------------------------+
    | Open a image and generate decoded image [1]_ | decoder_open()          |
    +----------------------------------------------+-------------------------+
    | Free any allocated resources                 | decoder_close()         |
    +----------------------------------------------+-------------------------+
    | Partially decode based on specified area     | decoder_get_area() [2]_ |
    | (Optional: use if ``decoder_open()`` does    |                         |
    | not decode whole image.)                     |                         |
    +----------------------------------------------+-------------------------+

.. [1]

    In ``decoder_open()``, you should try to open the image source pointed by
    ``dsc->src``.  Its type is already in ``dsc->src_type == LV_IMG_SRC_FILE/VARIABLE``.
    If this format/type is not supported by the decoder, return :cpp:enumerator:`LV_RESULT_INVALID`.
    However, if you can open the image, a pointer to the decoded image should be
    set in ``dsc->decoded``.  If the format is known, but you don't want to
    decode the entire image (e.g. no memory for it), set ``dsc->decoded = NULL`` and
    use ``decoder_get_area()`` to get the image area pixels.

.. [2]

    ``lv_bmp.c`` has an example of ``decoder_get_area()``.


Manually Using an Image Decoder
-------------------------------

LVGL will use registered image decoders automatically if you try and
draw a raw image (i.e. using the ``lv_image`` Widget) but you can use them
manually as well. Create an :cpp:type:`lv_image_decoder_dsc_t` variable to describe
the decoding session and call :cpp:func:`lv_image_decoder_open`.

.. code-block:: c

    lv_result_t res;
    lv_image_decoder_dsc_t dsc;
    lv_image_decoder_args_t args = { 0 }; /* Custom decoder behavior via args */
    res = lv_image_decoder_open(&dsc, &my_img_dsc, &args);

    if(res == LV_RESULT_OK) {
        /* Do something with `dsc->decoded`. You can copy out the decoded image by `lv_draw_buf_dup(dsc.decoded)`*/
        lv_image_decoder_close(&dsc);
   }

.. note::

    You would need to set :c:macro:`LV_USE_PRIVATE_API` to ``1`` in ``lv_conf.h``
    in order to do this since the definition of the :cpp:type:`lv_image_decoder_dsc_t`
    and :cpp:type:`_lv_image_decoder_args_t` structs are both in a private header file.


Image Post-Processing
---------------------

Considering that some hardware has special requirements for image formats, such as
alpha premultiplication and stride alignment, most image decoders (such as PNG
decoders) may not directly output image data that meets hardware requirements.

For this reason, LVGL provides a method for implementing custom image post-processing
to address unpredicted future GPU requirements (over and above the premultiplication
and stride alignment provided by :cpp:func:`lv_image_decoder_post_process`):

- In your custom GPU :ref:`draw unit <draw units>`, call a custom post-processing
  function after ``lv_image_decoder_open`` to adjust the data in the image cache, and
- then mark the processing status in ``cache_entry->process_state`` (to avoid repeated
  post-processing).

The below code example assumes the image was opened using the decoder interface
(e.g. using :ref:`libpng` and thus has already called
:cpp:func:`lv_image_decoder_post_process` to perform stride alignment and/or
premultiplication via the decoder descriptor's ``args`` field).

Example (requires :c:macro:`LV_USE_PRIVATE_API` to ``1`` in ``lv_conf.h``):

.. code-block:: c

    /* Define post-processing state */
    typedef enum {
        MY_IMAGE_PROCESS_STATE_NONE = 0,
        MY_IMAGE_PROCESS_STATE_BIT_SHIFTED = 1 << 4,
    } image_process_state_t;

    lv_result_t my_image_post_process(lv_image_decoder_dsc_t * dsc)
    {
        lv_color_format_t color_format = dsc->header.cf;
        lv_result_t res = LV_RESULT_OK;

        if (color_format == LV_COLOR_FORMAT_ARGB8888) {
            lv_cache_t * cache_p = dsc->cache;
            lv_cache_entry_t * entry = dsc->cache_entry;
            lv_mutex_lock(&cache_p->lock);

            if (!(entry->flags & MY_IMAGE_PROCESS_STATE_BIT_SHIFTED)) {
                lv_draw_buf_t * shifted_buf = NULL; /* Insert allocation call here. */
                if (shifted_buf == NULL) {
                    LV_LOG_ERROR("No memory for bit-shifting adjustment.");
                    res = LV_RESULT_INVALID;
                    goto alloc_failed;
                }

                /* Handle additional GPU requirement here. */

                lv_free(dsc->decoded);
                dsc->decoded = shifted_buf;
                entry->flags |= MY_IMAGE_PROCESS_STATE_BIT_SHIFTED;
                LV_LOG_USER("Bit shifting completed.");
            }

        alloc_failed:
            lv_mutex_unlock(&cache_p->lock);
        }

        return res;
    }

    /* GPU draw unit */

    void gpu_draw_image(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc, const lv_area_t * coords)
    {
        /* ... */
        lv_image_decoder_dsc_t decoder_dsc;
        lv_image_decoder_args_t args;
        lv_memzero(&args, sizeof(args));
        args.premultiply = true;
        args.stride_align = true;
        lv_result_t res = lv_image_decoder_open(&decoder_dsc, draw_dsc->src, &args);

        if (res != LV_RESULT_OK) {
            LV_LOG_ERROR("Failed to open image");
            return;
        }

        /* Pre-multiplication and stride alignment are now done from the call to
         * lv_image_decoder_open() above.  But our additional GPU requirement (which we
         * are calling bit-shifting above) isn't handled yet, so we do it below. */
        res = my_image_post_process(&decoder_dsc);
        if (res != LV_RESULT_OK) {
            LV_LOG_ERROR("Failed to post-process image");
            return;
        }
        /* ... */
    }



API
***

.. API equals:  lv_image_decoder_open


