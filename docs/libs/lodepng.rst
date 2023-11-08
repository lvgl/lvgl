===============
LODEPNG decoder
===============

Allow the use of PNG images in LVGL. This implementation uses
`lodepng <https://github.com/lvandeve/lodepng>`__ library.

If enabled in ``lv_conf.h`` by :c:macro:`LV_USE_LODEPNG` LVGL will register a new
image decoder automatically so PNG files can be directly used as any
other image sources.

Note that, a file system driver needs to registered to open images from
files. Read more about it :ref:`file-system` or just
enable one in ``lv_conf.h`` with ``LV_USE_FS_...``

The whole PNG image is decoded so during decoding RAM equals to
``image width x image height x 4`` bytes are required.

As it might take significant time to decode PNG images LVGL's :ref:`image-caching` feature can be useful.

Example
-------

.. include:: ../examples/libs/lodepng/index.rst

API
---

:ref:`lodepng`

