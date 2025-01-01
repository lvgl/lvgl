.. _draw_descriptors:

================
Draw Descriptors
================

Overview
--------

The ``draw_task``s are created internally when an ``lv_draw_rect/label/image/etc`` function is called.
These functions have the following parameters:
- an ``lv_layer_t`` parameter (can be obtained in the ``LV_EVENT_DRAW_MAIN/POST_...`` events by ``lv_event_get_layer(e)``)
- a draw descriptor, which is a large ``struct`` containing all the information about the drawing
- an area, in some cases, to specify where to draw

Each draw task type has its own draw descriptor. For example, ``lv_draw_label_dsc_t`` for label drawing.

Before using a draw descriptor in a draw event, it needs to be initialized with the related function.
For example, ``lv_draw_label_dsc_init(&my_label_draw_dsc);``

After that, each field of the draw descriptor can be set.
The default values are quite reasonable, so usually only a few fields need to be modified. For example:

.. code-block:: c
   /*In LV_EVENT_DRAW_MAIN*/

   lv_draw_label_dsc_t my_label_draw_dsc;
   lv_draw_label_dsc_init(&my_label_draw_dsc);
   my_label_draw_dsc.font = &my_font;
   my_label_draw_dsc.color = lv_color_hex(0xff0000);
   my_label_draw_dsc.text = "Hello";

   lv_area_t a = {10, 10, 200, 50}; /*Draw the label here*/

   lv_draw_label(lv_event_get_layer(e), &my_label_draw_dsc, &a);

When rendering a part of the widget, helper functions can be used to initialize the draw descriptors
based on the state, styles, and a given part of a widget. For example:

.. code-block:: c
   /*In LV_EVENT_DRAW_MAIN*/
    lv_draw_rect_dsc_t cur_dsc;
    lv_draw_rect_dsc_init(&cur_dsc);
    lv_obj_init_draw_rect_dsc(obj, LV_PART_CURSOR, &cur_dsc);
    cur_dsc.fill_color = lv_color_hex(0xff0000); /*Modify if needed*/
    lv_draw_rect(layer, &cur_dsc, &area);

When an ``lv_draw_...`` function is called, besides creating a draw task, it also copies
the draw descriptor and frees it automatically when needed. Therefore, it's fine to use
local draw descriptor variables.

In ``LV_EVENT_DRAW_TASK_ADDED`` the draw descriptor of the ``draw_task`` can be accessed
(using, for example, ``lv_draw_task_get_label_dsc()``) and modified (e.g., change the color, text, font, etc.).
It also means that in ``LV_EVENT_DRAW_TASK_ADDED`` the ``draw_task``s and draw descriptors are already
initialized. In practice, it looks like this:

.. code-block:: c
   /*In LV_EVENT_DRAW_TASK_ADDED*/
   lv_draw_task_t * t = lv_event_get_draw_task(e);
   lv_draw_label_dsc_t * draw_dsc =  lv_draw_task_get_label_dsc(t);
   /*Make the color lighter for longer texts*/
   draw_dsc->color = lv_color_lighten(draw_dsc->color, LV_MIN(lv_strlen(draw_dsc->text) * 5, 255));

   /*Create new draw tasks if needed here by calling lv_draw_...() functions*/

In the following, a detailed description of each draw task and descriptor type will be shown.

Relation to styles
------------------

In most of the cases the style properties are 1-1 mapped to the draw descriptor fields.
For example ``label_dsc.color`` is the ``text_color`` style property.
Or ``shadow_dsc.width``, ``line_dsc.opa``, ``arc_dsc.width`` are
``shadow_width``, ``line_opa``, and `arc_width` in case of the styles.

Therefor on this page only the differences and special considerations are highlighted.
For a detailed description on the meaning of each field see

- :ref:`style_properties`
- Style examples
- Canvas examples


Base Draw Descriptor
--------------------

Each draw descriptor has a generic "base descriptor" of type ``lv_draw_dsc_base_t`` with the name ``base``. It stores useful information about which widget and part created the draw descriptor.
See all the fields in :cpp:expr:`lv_draw_dsc_base_t`.

The ``lv_obj_init_draw_...`` functions automatically initialize the fields of the base descriptor.

Rectangle Draw Descriptor
-------------------------

The :cpp:expr:`lv_draw_rect_dsc_t` is a helper descriptor that combines:

- fill
- border
- outline (essentially a border but with its own styles)
- shadow
- background image (an image with its own styles)

into one call.

``lv_obj_init_draw_rect_dsc(obj, part, &dsc);`` can be used to initialize a draw descriptor from a widget,
and ``lv_draw_rect(layer, &dsc, area)`` draws the given rectangle in an area.


.. lv_example:: widgets/canvas/lv_example_canvas_3
  :language: c


Fill Draw Descriptor
--------------------

The main fields of :cpp:expr:`lv_draw_fill_dsc_t` fill descriptor are quit straightforward.
It has a radius, opacity, and color to draw a rectangle.

If the opacity is 0, no draw task will be created.

- ``lv_draw_fill_dsc_init(&dsc)`` initializes a fill draw task.
- ``lv_draw_fill(layer, &dsc, area)`` creates a draw task to fill an area.
- ``lv_draw_task_get_fill_dsc(draw_task)`` gets the fill descriptor from a draw task.

Gradients
^^^^^^^^^

The ``grad`` field of the fill descriptor (or :cpp_expression:`lv_grad_dsc_t` in general) can describe:

- horizontal
- vertical
- skew
- radial
- conic


TODO:

- gradients with multiple stops.
- Describe each in detail here.

Border Draw Descriptor
-----------------------

The :cpp:expr:`lv_draw_border_dsc_t` border descriptor has a radius, opacity, width, color and side fields.

If the opacity or the width is 0 , no draw task will be created.

 ``side`` can contain ORed vales of :cpp:expr:`lv_border_side_t`, for example :cpp:enum:`LV_BORDER_SIDE_BOTTOM`.
:cpp:enum:`LV_BORDER_SIDE_ALL` is a shorthad meaning all sides.
:cpp:enum:`LV_BORDER_SIDE_INTERNAL` can be used by upper layers (e.g. a table widget) to calculate the border sides
according to some rules, however the drawing routine should receive only the other, simple values.


The following functions can be used in association with border drawing:
- ``lv_draw_border_dsc_init(&dsc)`` initializes a border draw task.
- ``lv_draw_border(layer, &dsc, area)`` creates a draw task to draw a border on an area.
The border is always rendered inwards from its area.
- ``lv_draw_task_get_border_dsc(draw_task)`` gets the border descriptor from a draw task.

Outline Draw Descriptor
-----------------------

The :cpp:expr:`lv_draw_outline_dsc_t` outline descriptor has a radius, opacity, width, color and pad fields.

If the opacity or the width is 0 , no draw task will be created.

The outline is similar to the border, but it's darwn outwards of its draw area.
 ``pad`` means the gap between the target area and the inner side of the outline. It can be negative too.
 If ``pad = -width`` the otline will look like border.



The following functions can be used in association with outine drawing:
- ``lv_draw_outine_dsc_init(&dsc)`` initializes a outine draw task.
- ``lv_draw_outine(layer, &dsc, area)`` creates a draw task to draw a outine on an area.
The outline is rendered outwards from its area.
- ``lv_draw_task_get_outine_dsc(draw_task)`` gets the outine descriptor from a draw task.


Box Shadow Draw Descriptor
---------------------------

The :cpp:expr:`lv_draw_box_shadow_dsc_t` box shadow descriptor is used to describe a
**rounded rectangle shaped shadow**. That is it cannot be used to generate shadow for arbitrary shapes, texts, or images.
It has the following fields to control the shadow:

- ``radius``: Radius, :cpp:expr:`LV_RADIUS_CIRCLE`
- ``color``: Color of the the shadow
- ``width``: Width of the shadow. (radius of the blur)
- ``spread``: Make the rectangle larger with this value in all directions.
Can be negative too.
- ``ofs_x``: Offset the rectangle horizontally
- ``ofs_y``: Offset the rectangle vertically.
- ``opa``:  Opacity in 0...255 range. ``LV_OPA_TRANSP``, ``LV_OPA_10``, ``LV_OPA_20``, .. ``LV_OPA_COVER`` can be used as well*/
- ``bg_cover``:  Set to 1 if the background will cover the shadow.
 It's a hint to the renderer about it might skip some masking.

Note that box shadow rendering might be slow and/or use large anoun t of memory for large shadows.

The following functions can be used in association with box shadow drawing:
- ``lv_draw_box_shadow_dsc_init(&dsc)`` initializes a box_shadow draw task.
- ``lv_draw_box_shadow(layer, &dsc, area)`` creates a draw task to draw a bix shadow for a rectangle on ``area``.
The shadow can be larger and placed to different position based on the width, spread, and offset in the draw descriptor.
- ``lv_draw_task_get_box_shadow(draw_task)`` gets the box_shadow descriptor from a draw task.


Image Draw Descriptor
----------------------

The :cpp:expr:`lv_draw_image_dsc_t` image descriptor describes the parameters of image drawing.
It's a quite complex draw descriptor with the following options:

- ``src``: The image source: pointer to `lv_image_dsc_t` or a path to a file
- ``opa``:  Opacity in 0...255 range.
``LV_OPA_TRANSP``, ``LV_OPA_10``, ``LV_OPA_20``, .. ``LV_OPA_COVER`` can be used as well
- ``clip_radius``: Clip the corner of the image with this radius. Use `LV_RADIUS_CIRCLE` for max. radius
- ``rotation``: The rotation of the image in 0.1 degree unit. E.g. 234 means 23.4°
- ``scale_x``: Horizontal scale (zoom) of the image.
256 (LV_SCALE_NONE): means no zoom, 512 double size, 128 half size.
- ``scale_y``:  Same as ``scale_y`` but vertically
- ``skew_x``: Parallelogram like transformation of the image horizontally in 0.1 degree unit. E.g. 456 means 45.6°.
- ``skew_y``: Same as ``skew_x`` but vertically
- ``pivot``: The pivot point of transformation (scale and rotation).
0;0 is the top left corner of the image. Can be outside of the image too.
- ``bitmap_mask_src``: Pointer to an A8 or L8 image descriptor to mask the image with.
The mask is always center aligned.
- ``recolor``: Mix this color to the images. In case of :cpp:enum:`LV_COLOR_FORMAT_A8` it will be the color of the visible pixels
- ``recolor_opa``: The intensity of recoloring. 0 means, no recolor, 255 means full cover (transparent pixels remain transparent)
- ``blend_mode``: Describes how to blend the pixels of the image to the background.
See cpp:expr:`lv_blend_mode_t` for more details.
- ``antialias`` 1: perform the transformation with anti-alaising
- ``tile``: If the image is smaller than the `image_area`  field of `lv_draw_image_dsc_t`
tile the image (repeat is both horizontally and vertically) to fill the ``image_area`` area.
- ``image_area``: Used to indicate the entire original, non-clipped area where the image is to be drawn.
This is important for:

  1. Layer rendering, where it might happen that only a smaller area of the layer is rendered.
  2. Tiled images, where the target draw area is larger than the image to be tiled.

- ``sup``: Used internally to store some information about the palette or the color of A8 images*/


The following functions can be used in association with image drawing:
- ``lv_draw_image_dsc_init(&dsc)`` initializes a image draw task.
- ``lv_draw_image(layer, &dsc, area)`` creates a draw task to image an area.
- ``lv_draw_task_get_image_dsc(draw_task)`` gets the image descriptor from a draw task.


.. lv_example:: widgets/canvas/lv_example_canvas_6
  :language: c

Layers - Special Images
^^^^^^^^^^^^^^^^^^^^^^^

Layers are treated as images, therefore an :cpp:expr:`lv_draw_image_dsc_t` can
be used to described how the layers are blended to their parent layer.

All the images features can be used on layers as well.

``lv_draw_layer(layer, &dsc, area)`` initialites the blending of the layer back to the parent layer.
Other then that the image draw related functions can be used for layers too.

For more details see :ref:`layers`.

Label Draw Descriptor
---------------------


The :cpp:expr:`lv_draw_label_dsc_t` label descriptor provides a lot of options to control how the texts are rendered:

- ``text``: The text to draw
- ``font``: The font to use. Fallback fonts are also handled.
- ``color``: Color of the text.
- ``opa``: Opacity of the text.
- ``line_space``: Extra space between the lines.
- ``letter_space``: Extra space between the characters.
- ``ofs_x``: Offset the text with this value horizontally.
- ``ofs_y``: Offset the text with this value vertically.
- ``sel_start``: The first characters index for selection (not byte index). ``LV_DRAW_LABEL_NO_TXT_SEL`` for no selection.
- ``sel_end``: The lastcharacters index for selection (not byte index). ``LV_DRAW_LABEL_NO_TXT_SEL`` for no selection.
- ``sel_color``: Color of the selected characters.
- ``sel_bg_color``: Background color of the selected characters.
- ``align``: The alignment of the text ``LV_TEXT_ALIGN_LEFT/RIGHT/CENTER``. See :cpp:enum:`lv_text_align_t`.
- ``bidi_dir``: The base direction. Used when type setting Right-to-left (e.g. Arabic) texts. See :cpp:enum:`lv_base_dir_t`.
- ``decor``: Text decoration, e.g. underline. See :cpp:enum:`lv_text_decor_t`.
- ``flag``: Some flags to control type setting. See :cpp:enum:`lv_text_flag_t`.
- ``text_length``: The number of characters to render. 0: means render until reaching the ``\0`` termination.
- ``text_local``: 1: malloc a buffer and copy ``text`` there.
0: ``text`` will be valid during rendering.
- ``text_static``: Indicate that the text is constant and its pointer can be safely saved e.g. in a cache.
- ``hint``: Pointer to an externally stored struct where some data can be cached to speed up rendering.
See :cpp:enum:`lv_draw_label_hint_t`.


.. lv_example:: widgets/canvas/lv_example_canvas_4
  :language: c

Arc Draw Descriptor
--------------------

.. lv_example:: widgets/canvas/lv_example_canvas_5
  :language: c


Line Draw Descriptor
-------------------------

.. lv_example:: widgets/canvas/lv_example_canvas_7
  :language: c

Triangle Draw Descriptor
-------------------------

Mask Draw Descriptor
--------------------

Rectangle Mask
^^^^^^^^^^^^^^

Bitmap Mask
^^^^^^^^^^^

Vector Draw Descriptor
-----------------------

TODO

