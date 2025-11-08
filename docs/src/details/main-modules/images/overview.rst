.. _images overview:

========
Overview
========

In LVGL, the term "image" can refer to two things:

- an Image Widget (capitalized), or
- data that is used to draw an image to a Display.

This section of LVGL documentation is about the latter:  it describes how to use the
LVGL images module to draw images (in various forms) to a Display.



What is an Image?
*****************

Any image used by the LVGL images module is a :cpp:type:`lv_image_dsc_t` object plus
its data in any of the :ref:`supported pixel formats
<images_color_formats>`.  When the pixel format of the image is not in the
same format as the :ref:`Display` it will be sent to, the images module converts those
pixels to the target Display's format (into a buffer) each time it has to be drawn.



Using Images
************

You make use of images module by using any of these Widgets:

- :ref:`lv_image` Widgets
- :ref:`lv_imagebutton`  Widgets
- :ref:`lv_canvas` Widgets

There are also a number of Widgets that use Image Widgets internally for optional icons:

- :ref:`lv_list`
- :ref:`lv_menu`
- :ref:`lv_msgbox`
- :ref:`lv_win`

