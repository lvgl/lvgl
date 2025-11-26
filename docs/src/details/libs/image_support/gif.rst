.. include:: /include/substitutions.txt
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

Set the color format of the gif framebuffer with :cpp:expr:`lv_gif_set_color_format(widget, color_format)`.
Use :cpp:enumerator:`LV_COLOR_FORMAT_ARGB8888` for gifs with transparency.
It is set to :cpp:enumerator:`LV_COLOR_FORMAT_ARGB8888` by default.
Significant RAM can be saved by using a smaller color format.
A color format conversion can be saved during rendering if the color format matches the display
color format.
This function can be called before :cpp:func:`lv_gif_set_src` to prevent the initial default ARGB8888
framebuffer from being allocated.

The supported color formats are:

- :cpp:enumerator:`LV_COLOR_FORMAT_RGB565`
- :cpp:enumerator:`LV_COLOR_FORMAT_RGB565_SWAPPED`
- :cpp:enumerator:`LV_COLOR_FORMAT_RGB888`
- :cpp:enumerator:`LV_COLOR_FORMAT_ARGB8888`

:cpp:expr:`lv_gif_set_src(widget, src)` works very similarly to :cpp:func:`lv_image_set_src`.
As source, it also accepts images as variables (:cpp:struct:`lv_image_dsc_t`) or files.


Converting GIF Files to C Arrays
--------------------------------

Converting GIF files to an array of bytes is not supported in the online image
converter since LVGL v9.0.  However, there is still a way to do it through the
``./scripts/LVGLImage.py`` Python script.  This command line:

.. code-block:: bash

   python  ./scripts/LVGLImage.py  --cf RAW  --ofmt C  -o .  --name my_gif_image_array  input.gif

will produce all the bytes from the ``input.gif`` in unaltered form in a C array,
ready to use in this code:

.. code-block:: c

    LV_IMAGE_DECLARE(my_gif_image_array);
    lv_obj_t * img;

    img = lv_gif_create(lv_screen_active());
    lv_gif_set_color_format(img, LV_COLOR_FORMAT_ARGB8888);
    lv_gif_set_src(img, &my_gif_image_array);


Using GIF Images from Files
---------------------------

Example:

.. code-block:: c

   lv_gif_set_src(widget, "S:path/to/example.gif");

Note that, a file system driver needs to be registered to open images
from files.  To do so, follow the instructions in :ref:`file_system`.



Memory Requirements
*******************

To decode and display a GIF animation ~25 kB of RAM is needed plus
(color format pixel size + 1) |times| image width |times| image height.
RGB565 has a pixel size of 2, RGB888 has a pixel size of 3, and
ARGB8888 has a pixel size of 4.


.. _gif_example:

Example
*******

.. include:: /examples/libs/gif/index.rst



.. _gif_api:

API
***

.. API startswith:  lv_gif_

