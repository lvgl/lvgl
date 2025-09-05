.. _gif:

===========
GIF Decoder
===========

**GIF Decoder** is an LVGL extension that enables you to use GIF images in your LVGL UI.

The implementation uses the `AnimatedGIF <https://github.com/bitbank2/AnimatedGIF/tree/master>`__
library.


Usage
*****

Once enabled in ``lv_conf.h`` by setting :c:macro:`LV_USE_GIF` to ``1``,
:cpp:expr:`lv_gif_create(parent)` can be used to create a gif widget.

:cpp:expr:`lv_gif_set_src(widget, src)` works very similarly to :cpp:func:`lv_image_set_src`.
As source, it also accepts images as variables (:c:struct:`lv_image_dsc_t`) or files.

Converting GIF Files to C Arrays
--------------------------------

To convert a GIF file to an array of bytes, use `LVGL's online
converter <https://lvgl.io/tools/imageconverter>`__. Select "Raw" color
format and "C array" Output format.

Using GIF Images from Files
---------------------------

Example:

.. code-block:: c

   lv_gif_set_src(widget, "S:path/to/example.gif");

Note that, a file system driver needs to be registered to open images
from files.  To do so, follow the instructions in :ref:`file_system`.



Memory Requirements
*******************

.. |times|  unicode:: U+000D7 .. MULTIPLICATION SIGN

To decode and display a GIF animation ~25 kB of RAM is needed plus
5 |times| image width |times| image height.



.. _gif_example:

Example
*******

.. include:: ../../examples/libs/gif/index.rst



.. _gif_api:

API
***

.. API startswith:  lv_gif_

