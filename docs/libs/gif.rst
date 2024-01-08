.. _gif:

===========
GIF decoder
===========

Allow using GIF images in LVGL.

Detailed introduction: https://github.com/lecram/gifdec

When enabled in ``lv_conf.h`` with :c:macro:`LV_USE_GIF`
:cpp:expr:`lv_gif_create(parent)` can be used to create a gif widget.

:cpp:expr:`lv_gif_set_src(obj, src)` works very similarly to :cpp:func:`lv_image_set_src`.
As source, it also accepts images as variables (:c:struct:`lv_image_dsc_t`) or
files.

Convert GIF files to C array
----------------------------

To convert a GIF file to byte values array use `LVGL's online
converter <https://lvgl.io/tools/imageconverter>`__. Select "Raw" color
format and "C array" Output format.

Use GIF images from file
------------------------

For example:

.. code:: c

   lv_gif_set_src(obj, "S:path/to/example.gif");

Note that, a file system driver needs to be registered to open images
from files. Read more about it :ref:`overview_file_system` or just
enable one in ``lv_conf.h`` with ``LV_USE_FS_...``

Memory requirements
-------------------

To decode and display a GIF animation the following amount of RAM is
required: 

- :c:macro:`LV_COLOR_DEPTH` ``8``: 3 x image width x image height
- :c:macro:`LV_COLOR_DEPTH` ``16``: 4 x image width x image height
- :c:macro:`LV_COLOR_DEPTH` ``32``: 5 x image width x image height

.. _gif_example:

Example
-------

.. include:: ../examples/libs/gif/index.rst

.. _gif_api:

API
---

:ref:`gifdec`

