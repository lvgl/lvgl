.. include:: /include/external_links.txt

.. _images_color_formats:

=============
Color Formats
=============

Built-in Input Color Formats
****************************

The following image pixel color formats have built-in decoding support:

.. note::

    All of the below format names are used in code by prefixing them with
    ``LV_COLOR_FORMAT_`` (e.g. :cpp:enumerator:`LV_COLOR_FORMAT_RGB565`).

.. container:: tighter-table-3

    +------------------------+-----+------------+-----------------------------------------------------------------+
    | Format                 | BPP | Supported  | Description                                                     |
    |                        |     | by offline |                                                                 |
    |                        |     | converter  |                                                                 |
    +========================+=====+============+=================================================================+
    | I1 [1]_                | 1   | Yes        | Indexed with 2-color palette                                    |
    +------------------------+-----+------------+-----------------------------------------------------------------+
    | I2 [1]_                | 2   | Yes        | Indexed with 4-color palette                                    |
    +------------------------+-----+------------+-----------------------------------------------------------------+
    | I4 [1]_                | 4   | Yes        | Indexed with 16-color palette                                   |
    +------------------------+-----+------------+-----------------------------------------------------------------+
    | I8 [1]_                | 8   | Yes        | Indexed with 256-color palette                                  |
    +------------------------+-----+------------+-----------------------------------------------------------------+
    | A8                     | 8   | Yes        | Pixels express alpha only (used where color is supplied         |
    |                        |     |            | elsewhere as with fonts)                                        |
    +------------------------+-----+------------+-----------------------------------------------------------------+
    | L8                     | 8   | Yes        | Luminance:  pixels express gray-scale value in range [0-255]    |
    +------------------------+-----+------------+-----------------------------------------------------------------+
    | AL88                   | 16  | Not yet    | L8 with an alpha byte                                           |
    +------------------------+-----+------------+-----------------------------------------------------------------+
    | RGB565                 | 16  | Yes        | Standard RGB565 (little-endian format)                          |
    +------------------------+-----+------------+-----------------------------------------------------------------+
    | ARGB8565               | 24  | Yes        | RGB565 preceded by alpha byte                                   |
    +------------------------+-----+------------+-----------------------------------------------------------------+
    | RGB565A8               | 24  | Yes        | RGB565 + trailing alpha byte                                    |
    +------------------------+-----+------------+-----------------------------------------------------------------+
    | RGB565_SWAPPED         | 16  | Yes        | RGB565 big-endian format                                        |
    +------------------------+-----+------------+-----------------------------------------------------------------+
    | RGB888                 | 24  | Yes        | Standard RGB with 8 bits per color channel                      |
    +------------------------+-----+------------+-----------------------------------------------------------------+
    | ARGB8888               | 32  | Yes        | Standard ARGB with 8 bits per color channel                     |
    +------------------------+-----+------------+-----------------------------------------------------------------+
    | XRGB8888               | 32  | Yes        | Like ARGB but alpha bytes are ignored                           |
    +------------------------+-----+------------+-----------------------------------------------------------------+
    | ARGB8888_PREMULTIPLIED | 32  | Yes        | ARGB8888 with RGB pre-darkened according to alpha channel value |
    +------------------------+-----+------------+-----------------------------------------------------------------+
    | RAW [2]_               | ?   | n/a        | See :ref:`custom_image_formats`                                 |
    +------------------------+-----+------------+-----------------------------------------------------------------+
    | RAW_ALPHA [2]_         | ?   | n/a        | See :ref:`custom_image_formats`                                 |
    +------------------------+-----+------------+-----------------------------------------------------------------+

.. [1]  I1, I2, I4 and I8 indexed formats require ``LV_DRAW_SW_SUPPORT_ARGB8888`` to
        be configured to ``1`` in your ``lv_conf.h`` file.  Palette colors are
        always ``ARGB8888``.

.. [2]  Custom formats can be supported by specifying one of the RAW color formats and
        using an external :ref:`image decoder <image_decoders>` to decode it.
        See :ref:`image_decoders`.



Built-In Input and Output Formats
*********************************

+-------------------------+----------------+------------+-------------+
| Format                  | BPP            | Supported  | Description |
|                         |                | by offline |             |
|                         |                | converter  |             |
+=========================+================+============+=============+
| NATIVE [3]_             | LV_COLOR_DEPTH | No         | See below   |
+-------------------------+----------------+------------+-------------+
| NATIVE_WITH_ALPHA [3]_  | LV_COLOR_DEPTH | No         | See below   |
+-------------------------+----------------+------------+-------------+

.. [3]

    What they mean (from ``lv_color.h``):

    .. code-block::

        #if LV_COLOR_DEPTH == 1
            LV_COLOR_FORMAT_NATIVE            = LV_COLOR_FORMAT_I1,
            LV_COLOR_FORMAT_NATIVE_WITH_ALPHA = LV_COLOR_FORMAT_I1,
        #elif LV_COLOR_DEPTH == 8
            LV_COLOR_FORMAT_NATIVE            = LV_COLOR_FORMAT_L8,
            LV_COLOR_FORMAT_NATIVE_WITH_ALPHA = LV_COLOR_FORMAT_AL88,
        #elif LV_COLOR_DEPTH == 16
            LV_COLOR_FORMAT_NATIVE            = LV_COLOR_FORMAT_RGB565,
            LV_COLOR_FORMAT_NATIVE_WITH_ALPHA = LV_COLOR_FORMAT_RGB565A8,
        #elif LV_COLOR_DEPTH == 24
            LV_COLOR_FORMAT_NATIVE            = LV_COLOR_FORMAT_RGB888,
            LV_COLOR_FORMAT_NATIVE_WITH_ALPHA = LV_COLOR_FORMAT_ARGB8888,
        #elif LV_COLOR_DEPTH == 32
            LV_COLOR_FORMAT_NATIVE            = LV_COLOR_FORMAT_XRGB8888,
            LV_COLOR_FORMAT_NATIVE_WITH_ALPHA = LV_COLOR_FORMAT_ARGB8888,
        #else
        #error "LV_COLOR_DEPTH should be 1, 8, 16, 24 or 32"
        #endif



GPU Input Formats
*****************

The following color formats are supported by certain GPUs and are retained as
color-format enumeration values that can be used when a GPU is going to be doing the
drawing.


Alpha-Only and Special ARGB Formats
-----------------------------------

+----------+-----+------------+---------------------------------------+
| Format   | BPP | Supported  | Description                           |
|          |     | by offline |                                       |
|          |     | converter  |                                       |
+==========+=====+============+=======================================+
| A1       | 1   | No         | Like A8 but only 255 alpha or 0 alpha |
+----------+-----+------------+---------------------------------------+
| A2       | 2   | No         | Like A8 but 4 alpha values            |
+----------+-----+------------+---------------------------------------+
| A4       | 4   | No         | Like A8 but 16 alpha values           |
+----------+-----+------------+---------------------------------------+
| ARGB2222 | 8   | No         | ARGB2222                              |
+----------+-----+------------+---------------------------------------+
| ARGB1555 | 16  | No         | ARGB1555                              |
+----------+-----+------------+---------------------------------------+
| ARGB4444 | 16  | No         | ARGB4444                              |
+----------+-----+------------+---------------------------------------+


YUV Planar Formats
------------------

Reference: https://wiki.videolan.org/YUV/

.. container:: tighter-table-3

    +--------+-----+------------+---------------------------------------------------+
    | Format | BPP | Supported  | Description                                       |
    |        |     | by offline |                                                   |
    |        |     | converter  |                                                   |
    +========+=====+============+===================================================+
    | I420   |     | No         | YUV420 planar (3 plane)                           |
    +--------+-----+------------+---------------------------------------------------+
    | I422   |     | No         | YUV422 planar (3 plane)                           |
    +--------+-----+------------+---------------------------------------------------+
    | I444   |     | No         | YUV444 planar (3 plane)                           |
    +--------+-----+------------+---------------------------------------------------+
    | I400   |     | No         | YUV400 no chroma channel                          |
    +--------+-----+------------+---------------------------------------------------+
    | NV21   |     | No         | YUV420 planar (2 plane), UV plane in 'V, U, V, U' |
    +--------+-----+------------+---------------------------------------------------+
    | NV12   |     | No         | YUV420 planar (2 plane), UV plane in 'U, V, U, V' |
    +--------+-----+------------+---------------------------------------------------+


YUV Packed Formats
------------------

+--------+-----+------------+------------------------------+
| Format | BPP | Supported  | Description                  |
|        |     | by offline |                              |
|        |     | converter  |                              |
+========+=====+============+==============================+
| YUY2   |     | No         | YUV422 packed like 'Y U Y V' |
+--------+-----+------------+------------------------------+
| UYVY   |     | No         | YUV422 packed like 'U Y V Y' |
+--------+-----+------------+------------------------------+


Proprietary Formats
-------------------

+-------------+-----+------------+-------------+
| Format      | BPP | Supported  | Description |
|             |     | by offline |             |
|             |     | converter  |             |
+=============+=====+============+=============+
| NEMA_TSC4   |     | No         | NEMA TSC4   |
+-------------+-----+------------+-------------+
| NEMA_TSC6   |     | No         | NEMA TSC6   |
+-------------+-----+------------+-------------+
| NEMA_TSC6A  |     | No         | NEMA TSC6A  |
+-------------+-----+------------+-------------+
| NEMA_TSC6AP |     | No         | NEMA TSC6AP |
+-------------+-----+------------+-------------+
| NEMA_TSC12  |     | No         | NEMA TSC12  |
+-------------+-----+------------+-------------+
| NEMA_TSC12A |     | No         | NEMA TSC12A |
+-------------+-----+------------+-------------+

