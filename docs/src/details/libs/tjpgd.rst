.. _tjpgd:

================================
Tiny JPEG Decompressor (TJpgDec)
================================

**Tiny JPEG Decompressor** is an LVGL interface to the TJpgDec library --- a generic
JPEG image decompressor module that highly optimized for small embedded systems.  It
works with very low memory consumption, so that it can be incorporated into tiny
microcontrollers, such as AVR, 8051, PIC, Z80, Cortex-M0, etc..

For a detailed introduction, see: `TJpgDec <http://elm-chan.org/fsw/tjpgd/>`__.



.. _tjpgd_overview:

Overview
********

Features and restrictions:

- JPEG is decoded in 8x8 tiles.
- Only baseline JPEG files are supported (no progressive JPEG support).
- Read from file and C array are implemented.
- Only the required portions of the JPEG images are decoded,
  therefore they cannot be zoomed or rotated.



.. _tjpgd_usage:

Usage
*****

Set :c:macro:`LV_USE_TJPGD` to ``1`` in ``lv_conf.h``.  LVGL will register a new
image decoder automatically so JPEG files can be used directly as image sources.

For example:

.. code-block:: c

   lv_image_set_src(my_img, "S:path/to/picture.jpg");

.. note::

    A file system driver needs to be registered to open images from files.  To do so,
    follow the instructions in :ref:`file_system`.



Converter
*********

Converting JPEG to C array
--------------------------

-  Use lvgl online tool https://lvgl.io/tools/imageconverter
-  Color format = RAW, output format = C Array



.. _tjpgd_example:

Example
*******

.. include:: ../../examples/libs/tjpgd/index.rst



.. _tjpgd_api:

API
***

.. API startswith:  lv_tjpgd_

