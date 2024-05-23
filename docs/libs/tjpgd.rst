.. _tjpgd:

================================
Tiny JPEG Decompressor (TJpgDec)
================================

Allow the use of JPEG (JPG) images in LVGL.

Detailed introduction: `TJpgDec <http://elm-chan.org/fsw/tjpgd/>`__

.. _tjpgd_overview:

Overview
--------

-  JPEG is decoded in 8x8 tiles.
-  Only baseline JPEG files are supported (no progressive JPEG support).
-  Read from file and C array are implemented.
-  Only the required portions of the JPEG images are decoded,
   therefore they can't be zoomed or rotated.

.. _tjpgd_usage:

Usage
-----

If enabled in ``lv_conf.h`` by :c:macro:`LV_USE_TJPGD` LVGL will register a new
image decoder automatically so JPEG files can be used directly
as image sources.

For example:

.. code:: c

   lv_image_set_src(my_img, "S:path/to/picture.jpg");

:Note: a file system driver needs to be registered to open images from
files. Read more about :ref:`overview_file_system` or just
enable one in ``lv_conf.h`` with ``LV_USE_FS_...`` config.

Converter
---------

Converting JPEG to C array
~~~~~~~~~~~~~~~~~~~~~~~~~

-  Use lvgl online tool https://lvgl.io/tools/imageconverter
-  Color format = RAW, output format = C Array

.. _tjpgd_example:

Example
-------

.. include:: ../examples/libs/tjpgd/index.rst

.. _tjpgd_api:

API
---

:ref:`lv_tjpgd`

:ref:`tjpgd`

:ref:`tjpgdcnf`
