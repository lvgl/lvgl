=============
TJPGD decoder
=============

Allow the use of JPG (JPEG) images in LVGL.

Overview
--------

-  JPG is decoded in 8x8 tiles.
-  Read from file and C array are implemented.
-  Only the required portions of the JPG images are decoded,
   therefore they can't be zoomed or rotated.

Usage
-----

If enabled in ``lv_conf.h`` by :c:macro:`LV_USE_TJPGD` LVGL will register a new
image decoder automatically so JPG files can be used directly
as image sources. For example:

.. code:: c

   lv_image_set_src(my_img, "S:path/to/picture.jpg");

Note that a file system driver needs to be registered to open images from
files. Read more about it :ref:`file-system` or just
enable one in ``lv_conf.h`` with ``LV_USE_FS_...``

Converter
---------

Converting JPG to C array
~~~~~~~~~~~~~~~~~~~~~~~~~

-  Use lvgl online tool https://lvgl.io/tools/imageconverter
-  Color format = RAW, output format = C Array

Example
-------

.. include:: ../examples/libs/tjpgd/index.rst


API
---

:ref:`lv_tjpgd`

:ref:`tjpgd`

:ref:`tjpgdcnf`
