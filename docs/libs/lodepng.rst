.. _lodepng:

===============
LodePNG decoder
===============

Allow the use of PNG images in LVGL.

Detailed introduction: https://github.com/lvandeve/lodepng

If enabled in ``lv_conf.h`` by :c:macro:`LV_USE_LODEPNG` LVGL will register a new
image decoder automatically so PNG files can be directly used as any
other image sources.

:Note: a file system driver needs to be registered to open images from 
files. Read more about it :ref:`overview_file_system` or just
enable one in ``lv_conf.h`` with ``LV_USE_FS_...``

The whole PNG image is decoded, so ``width x height x 4`` bytes free RAM space is required. 
The decoded image is stored in RGBA pixel format.

As it might take significant time to decode PNG images LVGL's :ref:`overview_image_caching` feature can be useful.

Compress PNG files
------------------

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
-------

.. include:: ../examples/libs/lodepng/index.rst

.. _lodepng_api:

API
---

:ref:`lodepng`

