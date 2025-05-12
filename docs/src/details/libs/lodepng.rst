.. _lodepng_rst:

===============
LodePNG Decoder
===============

**LodePNG** is an LVGL interface to the LodePNG library --- a PNG encoder and decoder
in C and C++, without any dependencies, giving you an alternate way to use PNG images
in your LVGL UI.

For a detailed introduction, see:  https://github.com/lvandeve/lodepng .

If enabled in ``lv_conf.h`` by setting :c:macro:`LV_USE_LODEPNG` to ``1``, LVGL will
register a new image decoder automatically so PNG files can be used directly as an
image source.

.. note::

    A file system driver needs to be registered to open images from files.  To do so,
    follow the instructions in :ref:`file_system`.

.. |times|  unicode:: U+000D7 .. MULTIPLICATION SIGN

The whole PNG image is decoded, so

    width |times| height |times| 4

bytes of RAM is required from the LVGL heap.  The decoded image is stored in RGBA
pixel format.

Since it might take significant time to decode PNG images LVGL's
:ref:`overview_image_caching` feature can be useful.



Compressing PNG Files
*********************

PNG file format supports True color (24/32 bit), and 8-bit palette colors.
Usually cliparts, drawings, icons and simple graphics are stored in PNG format,
that do not use the whole color space, so it is possible to compress further
the image by using 8-bit palette colors, instead of 24/32 bit True color format.
Because embedded devices have limited (flash) storage, it is recommended
to compress images.

One option is to use a free online PNG compressor site,
for example Compress PNG: https://compresspng.com/



.. _lodepng_example:

Example
*******

.. include:: ../../examples/libs/lodepng/index.rst



.. _lodepng_api:

API
***

.. API startswith:  lv_lodepng_

