.. include:: /include/external_links.txt

.. _adding images to your project:

=============================
Adding Images to Your Project
=============================

You can add images to LVGL in three ways:

- using the :ref:`online converter <images_online_converter>`
- using the :ref:`offline converter <images_offline_converter>`
- :ref:`manually create images <images_manually_creating>`



.. _images_online_converter:

Online Converter
****************

Adding an image to LVGL via the `LVGL Online Image Converter`_ is easy:

1. Click [Select image file(s)] and select a BMP, PNG, JPG, WEBP or SVG image.
   Also supported:  GIF, SVGZ, TIF, TIFF, AI, DRW, PCT, PSP, XCF, PSD, RAW and HEIC.
2. Select the :ref:`Color format <images_color_formats>`.  Currently supported:

   - RGB565
   - RGB565A8
   - RGB888
   - XRGB8888
   - ARGB8888

   If you need another color format, use the :ref:`offline converter
   <images_offline_converter>`.

   .. note::  BMP images are currently supported in file form only.
              See :ref:`images_bmp_files` below.

3. Click the [Convert] button.  This will convert the file and open a "Save As..."
   dialog box.  The default filename will be the filename of the selected image file
   with a ``.c`` extension.

In the case of binary (.bin) files, you need to specify the color format you want:

- RGB332 for 8-bit color depth
- RGB565 for 16-bit color depth
- RGB565 Swap for 16-bit color depth (two bytes are swapped)
- RGB888 for 24-bit color depth (8-bit channels without an alpha channel)



.. _images_offline_converter:

Offline Converter
*****************

The offline converter is contained in ``./scripts/LVGLImage.py``.  Run it with no
arguments to see its command-line options.



.. _images_bmp_files:

BMP and WEBP Files
******************

Currently, support for BMP and WEBP images is limited to using them as files on an
external file system.  For a BMP file, follow the instructions in :ref:`bmp`.  For
WEBP files, follow the instructions in :ref:`libwebp`.



.. _images_bin_files:

BIN Files
*********

Another way you can add an image to your project is by using the
:ref:`images_offline_converter` to create a ``.bin`` file.  Typically, you would do
this with a target color format matching the Display the image is going to be sent
to. Optionally, you can use a command-line argument to cause the image contents to
be compressed using RLE or LZ4 compression as well.  Of course, the decompression
logic must be part of your project by enabling appropriate ``LV_USE_RLE``,
``LV_USE_LZ4_INTERNAL``, and/or ``LV_USE_LZ4_EXTERNAL`` macros in your ``lv_conf.h``
file.

This enables the drawing logic to have a very low RAM footprint because the pixels
are extracted directly from the file similar to how they are extracted from BMP files.

See :ref:`rle` and :ref:`lz4`



.. _images_manually_creating:

Manually Creating an Image
**************************

If you are generating an image at run-time, you can craft an image
variable to display it using LVGL. For example:

.. code-block:: c

    uint8_t my_img_data[] = {0x00, 0x01, 0x02, ...};

    static lv_image_dsc_t my_img_dsc = {
       .header.magic = LV_IMAGE_HEADER_MAGIC,
       .header.cf = LV_COLOR_FORMAT_RGB565,          /* Set color format */
       .header.flags = 0,
       .header.w = 80,
       .header.h = 60,
       .header.stride = 80 * LV_COLOR_DEPTH / 8,
       .header.reserved_2 = 0,
       .data_size = 80 * 60 * LV_COLOR_DEPTH / 8,
       .data = my_img_data,
    };

Another (possibly simpler) option to create and display an image at
run-time is to use the :ref:`Canvas <lv_canvas>` Widget.


