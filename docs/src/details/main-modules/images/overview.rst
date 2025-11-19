.. _images overview:

========
Overview
========

In LVGL, the term "image" can refer to two things:

- an :ref:`lv_image` Widget (capitalized), or
- data that is used to draw an image to a Display.

This section of LVGL documentation is about the latter:  it describes how to use the
LVGL images module to draw images (in various forms) to a Display.



What is an Image?
*****************

An image can be thought of as set of pixels used to create an image on a Display.
Images can be:

- an :cpp:type:`lv_image_dsc_t` object plus its pixel data in RAM or ROM in any of the
  :ref:`supported pixel formats <images_color_formats>`, or

- a stored image in a file on an external file system, or loaded into RAM or ROM.
  Such files can contain color formats other than the supported formats (e.g. JPEG,
  BMP, PNG, SVG, WEBP, GIF or other formats).

When the color format of the image is not in the same format as the :ref:`Display <display>` it
will be sent to, the drawing logic converts those pixels to the target Display's
format (into a buffer) when it is drawn.



Using Images
************

You make use of images by using any of these Widgets:

+-----------------------+---------------------------------+
| Widget                | Via                             |
+=======================+=================================+
| :ref:`lv_image`       | setting image source            |
+-----------------------+---------------------------------+
| :ref:`lv_imagebutton` | setting image source            |
+-----------------------+---------------------------------+
| :ref:`lv_animimg`     | setting image source            |
+-----------------------+---------------------------------+
| :ref:`lv_canvas`      | direct access to drawing buffer |
+-----------------------+---------------------------------+
| any                   | background image style          |
+-----------------------+---------------------------------+

There are also a number of Widgets that use Image Widgets internally for optional icons:

- :ref:`lv_list`
- :ref:`lv_menu`
- :ref:`lv_msgbox`
- :ref:`lv_win`

