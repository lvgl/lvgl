.. _bmp:

===========
BMP Decoder
===========

This BMP Decoder utility allows you to use images from .BMP files in LVGL.

Library source: https://github.com/caj-johnson/bmp-decoder

Instead of loading the whole image at once, BMP pixels are read on demand,
so using BMP images requires very little RAM.

If enabled in ``lv_conf.h`` by setting :c:macro:`LV_USE_BMP` to ``1``, LVGL will
register a new image decoder automatically so BMP files can be directly used as image
sources.  Example:

.. code-block:: c

   lv_image_set_src(my_img, "S:path/to/picture.bmp");

Note that, a :ref:`file_system` driver needs to registered to open images from
files.  Follow the instructions in :ref:`file_system`.



.. _bmp_limitations:

Limitations
***********

-  Only BMP files are supported.  BMP images as C arrays
   (:c:struct:`lv_image_dsc_t`) are not.  This is because there is no practical
   differences between how the BMP files and LVGL's image format stores
   the image data.
-  BMP files can be loaded only from .BMP files. If you want to store them in
   flash it's better to convert them to a C array with `LVGL's image converter <https://lvgl.io/tools/imageconverter>`__.
-  The BMP file's color format needs to match the configured :c:macro:`LV_COLOR_DEPTH`
   of the display on which it will be rendered.  You can use GIMP to save the image
   in the required format.  Both RGB888 and ARGB888 works with
   :c:macro:`LV_COLOR_DEPTH` ``32``
-  Color palettes are not supported.
-  Because the whole image is not loaded, it cannot be zoomed or rotated.



.. _bmp_example:

Example
*******

.. include:: ../../examples/libs/bmp/index.rst



.. _bmp_api:

API
***

