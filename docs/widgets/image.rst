.. _lv_image:

================
Image (lv_image)
================

Overview
********

Images are the basic object to display images from flash (as arrays) or
from files. Images can display symbols (``LV_SYMBOL_...``) too.

Using the :ref:`Image decoder interface <overview_image_decoder>` custom image formats
can be supported as well.

.. _lv_image_parts_and_styles:

Parts and Styles
****************

- :cpp:enumerator:`LV_PART_MAIN` A background rectangle that uses the typical
  background style properties and the image itself using the image
  style properties.

.. _lv_image_usage:

Usage
*****

Image source
------------

To provide maximum flexibility, the source of the image can be:

- a variable in code (a C array with the pixels).
- a file stored externally (e.g. on an SD card).
- a text with :ref:`Symbols <fonts_symbols>`.

To set the source of an image, use :cpp:expr:`lv_image_set_src(img, src)`.

To generate a pixel array from a PNG, JPG or BMP image, use the `Online image converter tool <https://lvgl.io/tools/imageconverter>`__
and set the converted image with its pointer  :cpp:expr:`lv_image_set_src(img1, &converted_img_var)`
To make the variable visible in the C file, you need to declare it with
:cpp:macro:`LV_IMAGE_DECLARE(converted_img_var)`.

To use external files, you also need to convert the image files using
the online converter tool but now you should select the binary output
format. You also need to use LVGL's file system module and register a
driver with some functions for the basic file operation. Go to the
:ref:`File system <overview_file_system>` to learn more. To set an image sourced
from a file, use :cpp:expr:`lv_image_set_src(img, "S:folder1/my_img.bin")`.

You can also set a symbol similarly to :ref:`Labels <lv_label>`. In
this case, the image will be rendered as text according to the *font*
specified in the style. It enables to use of light-weight monochrome
"letters" instead of real images. You can set symbol like
:cpp:expr:`lv_image_set_src(img1, LV_SYMBOL_OK)`.

Label as an image
-----------------

Images and labels are sometimes used to convey the same thing. For
example, to describe what a button does. Therefore, images and labels
are somewhat interchangeable, that is the images can display texts by
using :c:macro:`LV_SYMBOL_DUMMY` as the prefix of the text. For example,
:cpp:expr:`lv_image_set_src(img, LV_SYMBOL_DUMMY, "Some text")`.

Transparency
------------

The internal (variable) and external images support 2 transparency
handling methods:

-  **Alpha byte**: An alpha byte is added to every pixel that contains
   the pixel's opacity

Palette and Alpha index
-----------------------

Besides the *True color* (RGB) color format, the following formats are
supported:

- **Indexed**: Image has a palette.
- **Alpha indexed**: Only alpha values are stored.

These options can be selected in the image converter. To learn more
about the color formats, read the :ref:`Images <overview_image>` section.

Recolor
-------

A color can be mixed with every pixel of an image with a given
intensity. This can be useful to show different states (checked,
inactive, pressed, etc.) of an image without storing more versions of
the same image. This feature can be enabled in the style by setting
``img_recolor_opa`` between :cpp:enumerator:`LV_OPA_TRANSP` (no recolor, value: 0) and
:cpp:enumerator:`LV_OPA_COVER` (full recolor, value: 255). The default value is
:cpp:enumerator:`LV_OPA_TRANSP` so this feature is disabled.

The color to mix is set by ``img_recolor``.

Offset
------

With :cpp:expr:`lv_image_set_offset_x(img, x_ofs)` and
:cpp:expr:`lv_image_set_offset_y(img, y_ofs)`, you can add some offset to the
displayed image. Useful if the object size is smaller than the image
source size. Using the offset parameter a `Texture atlas <https://en.wikipedia.org/wiki/Texture_atlas>`__
or a "running image" effect can be created by :ref:`Animating <animations>` the x or y offset.

Transformations
---------------

Using the :cpp:expr:`lv_image_set_scale(img, factor)` the images will be zoomed.
Set ``factor`` to ``256`` or :c:macro:`LV_SCALE_NONE` to disable zooming. A
larger value enlarges the images (e.g. ``512`` double size), a smaller
value shrinks it (e.g. ``128`` half size). Fractional scale works as
well. E.g. ``281`` for 10% enlargement.

:cpp:expr:`lv_image_set_scale_x(img, factor)` and
:cpp:expr:`lv_image_set_scale_y(img, factor)` also can be used to
the scale independently horizontally and vertically (non-uniform scale).

To rotate the image use :cpp:expr:`lv_image_set_rotation(img, angle)`. Angle has 0.1
degree precision, so for 45.8° set 458.

By default, the pivot point of the rotation is the center of the image.
It can be changed with :cpp:expr:`lv_image_set_pivot(img, pivot_x, pivot_y)`.
``0;0`` is the top left corner.

The quality of the transformation can be adjusted with
:cpp:expr:`lv_image_set_antialias(img, true)`. With enabled anti-aliasing
the transformations are higher quality but slower.

The transformations require the whole image to be available. Therefore
indexed images (``LV_COLOR_FORMAT_I1/2/4/8_...``), alpha only images cannot be transformed.
In other words transformations work only on normal (A)RGB or A8 images stored as
C array, or if a custom :ref:`overview_image_decoder`
returns the whole image.

Note that the real coordinates of image objects won't change during
transformation. That is :cpp:expr:`lv_obj_get_width/height/x/y()` will return
the original, non-zoomed coordinates.

**IMPORTANT** The transformation of the image is independent of the
transformation properties coming from styles. (See
:ref:`here <styles_opacity_blend_modes_transformations>`). The main
differences are that pure image widget transformation

- doesn't transform the children of the image widget
- image is transformed directly without creating an intermediate layer (buffer) to snapshot the widget

Inner align
-----------

By default the image widget's width and height is :cpp:enumerator:`LV_SIZE_CONTENT`.
It means that the widget will be sized automatically according to the image source.

If the widget's width or height is set the larger value the ``inner_align`` property tells
how to align the image source inside the widget.

The alignment set any of these:

- :cpp:enumerator:`LV_IMAGE_ALIGN_DEFAULT`: Meaning top left
- :cpp:enumerator:`LV_IMAGE_ALIGN_TOP_LEFT`
- :cpp:enumerator:`LV_IMAGE_ALIGN_TOP_MID`
- :cpp:enumerator:`LV_IMAGE_ALIGN_TOP_RIGHT`
- :cpp:enumerator:`LV_IMAGE_ALIGN_BOTTOM_LEFT`
- :cpp:enumerator:`LV_IMAGE_ALIGN_BOTTOM_MID`
- :cpp:enumerator:`LV_IMAGE_ALIGN_BOTTOM_RIGHT`
- :cpp:enumerator:`LV_IMAGE_ALIGN_LEFT_MID`
- :cpp:enumerator:`LV_IMAGE_ALIGN_RIGHT_MID`
- :cpp:enumerator:`LV_IMAGE_ALIGN_CENTER`
- :cpp:enumerator:`LV_IMAGE_ALIGN_STRETCH`
- :cpp:enumerator:`LV_IMAGE_ALIGN_TILE`

The ``offset`` value is applied after the image source is aligned. For example setting an ``y=-10``
and :cpp:enumerator:`LV_IMAGE_ALIGN_CENTER` will move the image source up a little bit
from the center of the widget.

Or to automatically scale or tile the image

- :cpp:enumerator:`LV_IMAGE_ALIGN_STRETCH` Set X and Y scale to fill the widget's area
- :cpp:enumerator:`LV_IMAGE_ALIGN_TILE` Tile the image to will the widget area. Offset is applied to shift the tiling.

The alignment can be set by :cpp:func:`lv_image_set_inner_align`

.. _lv_image_events:

Events
******

No special events are sent by image objects.

See the events of the :ref:`Base object <lv_obj>` too.

Learn more about :ref:`events`.

.. _lv_image_keys:

Keys
****

No *Keys* are processed by the object type.

Learn more about :ref:`indev_keys`.

.. _lv_image_example:

Example
*******

.. include:: ../examples/widgets/image/index.rst

.. _lv_image_api:

API
***
