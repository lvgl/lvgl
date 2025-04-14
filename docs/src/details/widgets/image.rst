.. _lv_image:

================
Image (lv_image)
================

.. |deg|    unicode:: U+000B0 .. DEGREE SIGN

Overview
********

.. |deg|    unicode:: U+000B0 .. DEGREE SIGN

Images are Widgets that display images from flash (as arrays) or
from files. Images can display symbols (``LV_SYMBOL_...``) as well.

Using the :ref:`Image decoder interface <overview_image_decoder>`, custom image formats
can be supported as well.

.. _lv_image_parts_and_styles:

Parts and Styles
****************

-  :cpp:enumerator:`LV_PART_MAIN` A background rectangle that uses the :ref:`typical
   background style properties <typical bg props>`, and the image itself uses the image
   style properties.

.. _lv_image_usage:

Usage
*****

Image source
------------

To provide maximum flexibility, the source of the image can be:

- a variable in code (a C array containing the pixels).
- a file stored externally (e.g. on an SD card).
- a :ref:`Symbol <fonts_symbols>` as text.

To set the source of an image, use :cpp:expr:`lv_image_set_src(img, src)`.

To generate a pixel array from a PNG, JPG or BMP image, use the `Online image converter tool <https://lvgl.io/tools/imageconverter>`__
and set the converted image as the image source with its pointer with
:cpp:expr:`lv_image_set_src(img1, &converted_img_var)`.
To make the converted image variable accessible from the C file, declare it with
:cpp:expr:`LV_IMAGE_DECLARE(converted_img_var)`.

To use external files, you also need to convert the image files using
the online converter tool, but select the binary output
format. You also need to use LVGL's file system module and register a
driver with some functions for basic file operations.  See
:ref:`File system <file_system>` to learn more.  Then set the translated
image as the image source with :cpp:expr:`lv_image_set_src(img, "S:folder1/my_img.bin")`.

You can also set a symbol as an image source similar to a :ref:`Labels <lv_label>`. In
this case, the image will be rendered as text according to the *font*
specified in the style. It enables the use of light-weight monochrome
"characters" instead of real images. You can set a symbol as an image source with
:cpp:expr:`lv_image_set_src(img1, LV_SYMBOL_OK)`.

Label as an image
-----------------

Images and labels are sometimes used to convey the same thing, such as
describing what a button does.  In this context, images and labels
are somewhat interchangeable:  images can display text by
using the macro :c:macro:`LV_SYMBOL_DUMMY` (which equates to a 3-byte C string
containing a special code) as the prefix of the text.  For example,
``lv_image_set_src(img, LV_SYMBOL_DUMMY "Some text")``.

Transparency
------------

The internal (pixel array) and external images support 2 transparency
handling methods:

-  **Alpha byte**: An alpha channel is added to every pixel that contains
   its opacity, typically a byte.  It is the 'A' in the the various color formats
   that contain an alpha channel, such as ARGB8888, ARGB8565, ARGB1555, etc.
-  **Indexed transparent color**:  a specific index in a color palette serves to
   signal transparency for each pixel that uses it.

Palette and Alpha index
-----------------------

Besides the *True color* (RGB) color format, the following formats are
supported:

- **Indexed**: Image has a color palette, and each pixel is an index into that palette.
- **Alpha indexed**: The values stored at pixel positions are alpha (opacity) values.

These options can be selected in the image converter. Learn more
about color formats in the :ref:`overview_image_color_formats` section.

Recolor
-------

A color can be mixed with every pixel of an image with a given
intensity. This can be useful to show different states (checked,
inactive, pressed, etc.) of an image without storing more versions of
the same image. This feature can be enabled in the style by setting
``img_recolor_opa`` between :cpp:enumerator:`LV_OPA_TRANSP` (no recolor, value: 0) and
:cpp:enumerator:`LV_OPA_COVER` (full recolor, value: 255). The default value is
:cpp:enumerator:`LV_OPA_TRANSP` causing this feature to be disabled.

The color to mix is set by ``img_recolor``.

Offset
------

With :cpp:expr:`lv_image_set_offset_x(img, x_ofs)` and
:cpp:expr:`lv_image_set_offset_y(img, y_ofs)`, you can add some offset to the
displayed image. Useful if the Widget size is smaller than the image
source size. Using the offset parameter a `Texture atlas <https://en.wikipedia.org/wiki/Texture_atlas>`__
or a "running image" effect can be created by :ref:`Animating <animation>` the x or y offset.

Transformations
---------------

You can zoom images in or out by using :cpp:expr:`lv_image_set_scale(img, factor)`.
Set ``factor`` to ``256`` or :c:macro:`LV_SCALE_NONE` to disable zooming. A
larger value enlarges the images (e.g. ``512`` double size), a smaller
value shrinks it (e.g. ``128`` half size). Fractional scaling works using a value
that is proportionally larger or smaller, e.g. ``281`` for 10% enlargement.

:cpp:expr:`lv_image_set_scale_x(img, factor)` and
:cpp:expr:`lv_image_set_scale_y(img, factor)` can also be used to
set the horizontal and vertical scaling independently.  They can be different values.

To rotate the image use :cpp:expr:`lv_image_set_rotation(img, angle_x10)`.
The ``angle_x10`` argument is an ``int32_t`` containing the angle (in degrees)
multiplied by 10.  This gives 0.1-degree resolution.  Example:  458 means 45.8\ |deg|\ .

By default, the pivot point of the rotation is the center of the image.
This can be changed with :cpp:expr:`lv_image_set_pivot(img, pivot_x, pivot_y)` where
the coordinates ``(0,0)`` represent the top left corner.

The quality of the transformation can be adjusted with
:cpp:expr:`lv_image_set_antialias(img, true)`. Enabling anti-aliasing
causes the transformations to be of higher quality, but slower.

Transformations require the whole image to be available. Therefore
indexed images (``LV_COLOR_FORMAT_I1/2/4/8_...``) and alpha only images cannot be transformed.
In other words transformations work only on normal (A)RGB or A8 images stored as a
C array, or on images provided by a custom :ref:`overview_image_decoder`
that returns the whole image.

Note that the real coordinates of image Widgets do not change with a
transformation. That is :cpp:expr:`lv_obj_get_width/height/x/y()` will return
the original, non-zoomed coordinates.

**IMPORTANT**:  The transformation of the image is independent of the transformation
properties :ref:`coming from styles <style_opacity_blend_modes_transformations>`.
The main differences are that pure Image Widget transformations:

- do not transform the children of the Image Widget, and
- the image is transformed directly without creating an intermediate layer (buffer) to snapshot the Widget.

Inner align
-----------

By default the image Widget's width and height are :cpp:enumerator:`LV_SIZE_CONTENT`,
meaning that the Widget will be sized automatically to the size of its image source.

If the Widget's width or height is set to a different value, the value of the ``inner_align``
property (set using :cpp:expr:`lv_image_set_inner_align(widget, align)`) governs how
the image source is aligned inside the Widget.

``align`` can be any of these values:

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

Any ``offset`` value is applied after the image source is aligned. For example setting
an offset of ``y=-10`` with ``align`` == :cpp:enumerator:`LV_IMAGE_ALIGN_CENTER` will
move the image source up 10 pixels from the center of the Widget.

To automatically scale or tile the image, pass one of these ``align`` values:

- :cpp:enumerator:`LV_IMAGE_ALIGN_STRETCH` Set X and Y scale to fill the Widget's area
- :cpp:enumerator:`LV_IMAGE_ALIGN_TILE` Tile image to fill Widget's area. Offset is applied to shift the tiling.



.. _lv_image_events:

Events
******

No special events are sent by Image Widgets.  By default, Image Widgets are created
without the LV_OBJ_FLAG_CLICKABLE flag, but you can add it to make an Image Widget
detect and emit LV_EVENT_CLICKED events if desired.

.. admonition::  Further Reading

    Learn more about :ref:`lv_obj_events` emitted by all Widgets.

    Learn more about :ref:`events`.



.. _lv_image_keys:

Keys
****

No *Keys* are processed by Image Widgets.

.. admonition::  Further Reading

    Learn more about :ref:`indev_keys`.



.. _lv_image_example:

Example
*******

.. include:: ../../examples/widgets/image/index.rst



.. _lv_image_api:

API
***
