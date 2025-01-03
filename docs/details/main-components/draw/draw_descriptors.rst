.. _draw_descriptors:

================
Draw Descriptors
================

Overview
--------

Each draw task type has its own draw descriptor type. For example,
:cpp:expr:`lv_draw_label_dsc_t` is used for label drawing,
:cpp:expr:`lv_draw_image_dsc_t` is used for image drawing.


When an ``lv_draw_...`` function is called, it creates a draw task,
copies the draw descriptor into a ``malloc``ed memory, and frees it automatically
when needed. Therefore, local draw descriptor variables can be safely used.

Relation to Styles
------------------

In most cases, style properties map 1-to-1 to draw descriptor fields. For example:

- ``label_dsc.color`` corresponds to the ``text_color`` style property.
- ``shadow_dsc.width``, ``line_dsc.opa``, and ``arc_dsc.width`` map to
  ``shadow_width``, ``line_opa``, and ``arc_width`` in styles.

This page focuses on the draw descriptors. Check out the :ref:`style_properties`
page to see all the style property options.


Base Draw Descriptor
--------------------

In each draw descriptor there is a generic "base descriptor" with
:cpp:expr:`lv_draw_dsc_base_t` type and with ``base`` name. For example
``label_dsc.base``. It stores useful information about which widget
and part created the draw descriptor. See all the fields in
:cpp:expr:`lv_draw_dsc_base_t`.

In an :cpp:enumerator:`LV_EVENT_DRAW_TASK_ADDED` event the elements of the base draw
descriptor are very useful to identify the draw task. For example:

.. code-block:: c

   /* In LV_EVENT_DRAW_TASK_ADDED */
   lv_draw_task_t * t = lv_event_get_draw_task(e);
   lv_draw_base_dsc_t * draw_dsc = lv_draw_task_get_draw_dsc(t);
   draw_dsc.obj;  /*The widget for which the draw descriptor was created */
   draw_dsc.part; /*The widget part for which the draw descriptor was created
                    E.g. LV_PART_INDICATOR*/
   draw_dsc.id1;  /*A widget type specific ID (e.g. table row index).
                    See the docs of the given widget.*/
   draw_dsc.id2;

   draw_dsc. layer; /*The target layer.
                      Required when a new draw tasks are also created */


Simple Initilialzation
^^^^^^^^^^^^^^^^^^^^^^

Before using a draw descriptor it needs to be initialized with
the related function. For example, ``lv_draw_label_dsc_init(&my_label_draw_dsc);``.

After initialization, each field of the draw descriptor can be set. The default
values are quite sane and reasonable, so usually only a few fields need modification.
For example:

.. code-block:: c

   /* In LV_EVENT_DRAW_MAIN */
   lv_draw_label_dsc_t my_label_draw_dsc;
   lv_draw_label_dsc_init(&my_label_draw_dsc);
   my_label_draw_dsc.font = &my_font;
   my_label_draw_dsc.color = lv_color_hex(0xff0000);
   my_label_draw_dsc.text = "Hello";

   lv_area_t a = {10, 10, 200, 50}; /* Draw the label here */

   lv_draw_label(lv_event_get_layer(e), &my_label_draw_dsc, &a);


Initilialzation for Widgets
^^^^^^^^^^^^^^^^^^^^^^^^^^^

When rendering a part of a widget, helper functions can initialize draw
descriptors based on the styles, and a specific widget part in the current state.

For example:

.. code-block:: c

   /* In LV_EVENT_DRAW_MAIN */
   lv_draw_rect_dsc_t cur_dsc;
   lv_draw_rect_dsc_init(&cur_dsc);
   lv_obj_init_draw_rect_dsc(obj, LV_PART_CURSOR, &cur_dsc);
   cur_dsc.fill_color = lv_color_hex(0xff0000); /* Modify if needed */
   lv_draw_rect(layer, &cur_dsc, &area);



The ``lv_obj_init_draw_...`` functions automatically initialize the fields of
the base descriptor.


Modify the draw descriptors
^^^^^^^^^^^^^^^^^^^^^^^^^^^

In :cpp:enumerator:`LV_EVENT_DRAW_TASK_ADDED`, the draw descriptor of the ``draw_task`` can be
accessed (using :cpp:expr:`lv_draw_task_get_label_dsc()` and similar functions)
and modified (change the color, text, font, etc.). This means that in
:cpp:enumerator:`LV_EVENT_DRAW_TASK_ADDED`, the ``draw_task``s and draw descriptors are already
initialized and it's enough to change only a few specific values.

For example:

.. code-block:: c

   /* In LV_EVENT_DRAW_TASK_ADDED */
   lv_draw_task_t * t = lv_event_get_draw_task(e);
   lv_draw_label_dsc_t * draw_dsc = lv_draw_task_get_label_dsc(t);

   /*Check a few things in `draw_dsc->base`*/

   /* Make the color lighter for longer texts */
   draw_dsc->color = lv_color_lighten(draw_dsc->color,
                                      LV_MIN(lv_strlen(draw_dsc->text) * 5, 255));

   /* Create new draw tasks if needed by calling
    * `lv_draw_...(draw_dsc->base.layer, ...)` functions */


Rectangle Draw Descriptor
-------------------------

The :cpp:expr:`lv_draw_rect_dsc_t` is a helper descriptor that combines:

- Fill
- Border
- Outline (a border with its own styles)
- Shadow
- Background image (an image with its own styles)

into a single call.

``lv_obj_init_draw_rect_dsc(obj, part, &dsc);`` initializes a draw descriptor
from a widget, and ``lv_draw_rect(layer, &dsc, area)`` draws the rectangle in a
specified area.

.. lv_example:: widgets/canvas/lv_example_canvas_3
  :language: c

Fill Draw Descriptor
--------------------

The main fields of :cpp:expr:`lv_draw_fill_dsc_t` are straightforward. It has
a radius, opacity, and color to draw a rectangle. If the opacity is 0, no draw
task will be created.

- ``lv_draw_fill_dsc_init(&dsc)`` initializes a fill draw task.
- ``lv_draw_fill(layer, &dsc, area)`` creates a draw task to fill an area.
- ``lv_draw_task_get_fill_dsc(draw_task)`` retrieves the fill descriptor from a
  draw task.

Gradients
^^^^^^^^^

The ``grad`` field of the fill descriptor (or :cpp:expr:`lv_grad_dsc_t` in
general) supports:

- Horizontal
- Vertical
- Skew
- Radial
- Conic

TODO:

- Gradients with multiple stops
- Detailed descriptions of each type

.. lv_example:: styles/lv_example_style_2
  :language: c

.. lv_example:: styles/lv_example_style_16
  :language: c

.. lv_example:: styles/lv_example_style_17
  :language: c

.. lv_example:: styles/lv_example_style_18
  :language: c

Border Draw Descriptor
-----------------------

The :cpp:expr:`lv_draw_border_dsc_t` border descriptor has radius, opacity,
width, color, and side fields. If the opacity or width is 0, no draw task will
be created.

``side`` can contain ORed values of :cpp:expr:`lv_border_side_t`, such as
:cpp:enumerator:`LV_BORDER_SIDE_BOTTOM`. :cpp:enumerator:`LV_BORDER_SIDE_ALL` applies to all
sides, while :cpp:enumerator:`LV_BORDER_SIDE_INTERNAL` is used by higher layers
(e.g., a table widget) to calculate border sides. However, the drawing routine
receives only simpler values.

The following functions are used for border drawing:

- ``lv_draw_border_dsc_init(&dsc)`` initializes a border draw task.
- ``lv_draw_border(layer, &dsc, area)`` creates a draw task to draw a border
  inward from its area.
- ``lv_draw_task_get_border_dsc(draw_task)`` retrieves the border descriptor
  from a draw task.

.. lv_example:: styles/lv_example_style_3
  :language: c

Outline Draw Descriptor
-----------------------

The :cpp:expr:`lv_draw_outline_dsc_t` outline descriptor has radius, opacity,
width, color, and pad fields. If the opacity or width is 0, no draw task will
be created.

The outline is similar to the border but is drawn outward from its draw area.
``pad`` specifies the gap between the target area and the inner side of the
outline. It can be negative. For example, if ``pad = -width``, the outline will
resemble a border.

The following functions are used for outline drawing:

- ``lv_draw_outline_dsc_init(&dsc)`` initializes an outline draw task.
- ``lv_draw_outline(layer, &dsc, area)`` creates a draw task to draw an outline
  outward from an area.
- ``lv_draw_task_get_outline_dsc(draw_task)`` retrieves the outline descriptor
  from a draw task.

.. lv_example:: styles/lv_example_style_4
  :language: c

Box Shadow Draw Descriptor
---------------------------

The :cpp:expr:`lv_draw_box_shadow_dsc_t` box shadow descriptor describes a
**rounded rectangle-shaped shadow**. It cannot generate shadows for arbitrary
shapes, text, or images. It includes the following fields:

- ``radius``: Radius, :cpp:expr:`LV_RADIUS_CIRCLE`.
- ``color``: Shadow color.
- ``width``: Shadow width (blur radius).
- ``spread``: Expands the rectangle in all directions; can be negative.
- ``ofs_x``: Horizontal offset.
- ``ofs_y``: Vertical offset.
- ``opa``: Opacity (0–255 range). Values like ``LV_OPA_TRANSP``, ``LV_OPA_10``,
  etc., can also be used.
- ``bg_cover``: Set to 1 if the background will cover the shadow (a hint for the
  renderer to skip masking).

Note: Rendering large shadows may be slow or memory-intensive.

The following functions are used for box shadow drawing:

- ``lv_draw_box_shadow_dsc_init(&dsc)`` initializes a box shadow draw task.
- ``lv_draw_box_shadow(layer, &dsc, area)`` creates a draw task for a rectangle's
  shadow. The shadow's size and position depend on the width, spread, and offset.
- ``lv_draw_task_get_box_shadow(draw_task)`` retrieves the box shadow descriptor
  from a draw task.

.. lv_example:: styles/lv_example_style_5
  :language: c

Image Draw Descriptor
----------------------

The :cpp:expr:`lv_draw_image_dsc_t` image descriptor defines the parameters for
image drawing. It is a complex descriptor with the following options:

- ``src``: The image source, either a pointer to `lv_image_dsc_t` or a file path.
- ``opa``: Opacity in the 0...255 range. Options like
  ``LV_OPA_TRANSP``, ``LV_OPA_10``, etc., can also be used.
- ``clip_radius``: Clips the corners of the image with this radius. Use
  `LV_RADIUS_CIRCLE` for the maximum radius.
- ``rotation``: Image rotation in 0.1-degree units (e.g., 234 means 23.4°).
- ``scale_x``: Horizontal scaling (zoom) of the image.
  256 (LV_SCALE_NONE) means no zoom, 512 doubles the size, and 128 halves it.
- ``scale_y``: Same as ``scale_x`` but for vertical scaling.
- ``skew_x``: Horizontal skew (parallelogram-like transformation) in 0.1-degree
  units (e.g., 456 means 45.6°).
- ``skew_y``: Vertical skew, similar to ``skew_x``.
- ``pivot``: The pivot point for transformations (scaling and rotation).
  0;0 is the top-left corner of the image and can be set outside the image.
- ``bitmap_mask_src``: Pointer to an A8 or L8 image descriptor used to mask the
  image. The mask is always center-aligned.
- ``recolor``: Mixes this color with the image. For :cpp:enumerator:`LV_COLOR_FORMAT_A8`,
  this will be the visible pixels' color.
- ``recolor_opa``: Intensity of recoloring (0 means no recoloring, 255 means full cover).
- ``blend_mode``: Defines how to blend image pixels with the background.
  See :cpp:expr:`lv_blend_mode_t` for more details.
- ``antialias``: Set to 1 to enable anti-aliasing for transformations.
- ``tile``: Tiles the image (repeats it both horizontally and vertically) if the
  image is smaller than the `image_area` field in `lv_draw_image_dsc_t`.
- ``image_area``: Indicates the original, non-clipped area where the image
  is drawn. This is essential for:

  1. Layer rendering, where only part of a layer may be rendered and
  ``clip_radius`` needs the original image dimensions.
  2. Tiling, where the draw area is larger than the image.

- ``sup``: Internal field to store information about the palette or color of A8 images.

Functions for image drawing:

- ``lv_draw_image_dsc_init(&dsc)`` initializes an image draw descriptor.
- ``lv_draw_image(layer, &dsc, area)`` creates a task to draw an image in a given area.
- ``lv_draw_task_get_image_dsc(draw_task)`` retrieves the image descriptor from a task.

.. lv_example:: widgets/canvas/lv_example_canvas_6
  :language: c

.. lv_example:: styles/lv_example_style_6
  :language: c

Layers - Special Images
^^^^^^^^^^^^^^^^^^^^^^^

Layers are treated as images, so an :cpp:expr:`lv_draw_image_dsc_t` can describe
how layers are blended into their parent layers. All image features apply to
layers as well.

``lv_draw_layer(layer, &dsc, area)`` initializes the blending of a layer back to
its parent layer. Additionally, image-drawing-related functions can be used for
layers.

For more details, see :ref:`layers`.

Label Draw Descriptor
---------------------

The :cpp:expr:`lv_draw_label_dsc_t` label descriptor provides extensive options
for controlling text rendering:

- ``text``: The text to render.
- ``font``: Font to use, with support for fallback fonts.
- ``color``: Text color.
- ``opa``: Text opacity.
- ``line_space``: Additional space between lines.
- ``letter_space``: Additional space between characters.
- ``ofs_x``: Horizontal text offset.
- ``ofs_y``: Vertical text offset.
- ``sel_start``: Index of the first character for selection (not byte index).
  ``LV_DRAW_LABEL_NO_TXT_SEL`` means no selection.
- ``sel_end``: Index of the last character for selection.
- ``sel_color``: Color of selected characters.
- ``sel_bg_color``: Background color for selected characters.
- ``align``: Text alignment. See :cpp:expr:`lv_text_align_t`.
- ``bidi_dir``: Base direction for right-to-left text rendering (e.g., Arabic).
  See :cpp:expr:`lv_base_dir_t`.
- ``decor``: Text decoration, e.g., underline. See :cpp:expr:`lv_text_decor_t`.
- ``flag``: Flags for text rendering. See :cpp:expr:`lv_text_flag_t`.
- ``text_length``: Number of characters to render (0 means render until `\0`).
- ``text_local``: Set to 1 to allocate a buffer and copy the text.
- ``text_static``: Indicates the text is constant and its pointer can be cached.
- ``hint``: Pointer to externally stored data to speed up rendering.
  See :cpp:expr:`lv_draw_label_hint_t`.

Functions for text drawing:

- ``lv_draw_label_dsc_init(&dsc)`` initializes a label draw descriptor.
- ``lv_draw_label(layer, &dsc, area)`` creates a task to render text in an area.
- ``lv_draw_character(layer, &dsc, point, unicode_letter)`` creates a task to
  draw a character at a specific point.
- ``lv_draw_task_get_label_dsc(draw_task)`` retrieves the label descriptor from a task.

For character-specific drawing in draw units, use
:cpp:func:`lv_draw_label_iterate_characters(draw_unit, draw_dsc, area, callback)`.
This iterates through all characters, calculates their positions, and calls the
callback for rendering each character. For callback details, see
:cpp:expr:`lv_draw_glyph_cb_t`.

.. lv_example:: widgets/canvas/lv_example_canvas_4
  :language: c

.. lv_example:: styles/lv_example_style_8
  :language: c

Arc Draw Descriptor
--------------------

The :cpp:expr:`lv_draw_arc_dsc_t` arc descriptor defines arc rendering with
these fields:

- ``color``: Arc color.
- ``img_src``: Image source for the arc, or `NULL` if unused.
- ``width``: Arc thickness.
- ``start_angle``: Starting angle in degrees (e.g., 0° is 3 o'clock, 90° is 6 o'clock).
- ``end_angle``: Ending angle.
- ``center``: Arc center point.
- ``radius``: Arc radius.
- ``opa``: Arc opacity (0...255).
- ``rounded``: Rounds the arc ends.

Functions for arc drawing:

- ``lv_draw_arc_dsc_init(&dsc)`` initializes an arc descriptor.
- ``lv_draw_arc(layer, &dsc)`` creates a task to render an arc.
- ``lv_draw_task_get_arc_dsc(draw_task)`` retrieves the arc descriptor from a task.

.. lv_example:: widgets/canvas/lv_example_canvas_5
  :language: c

.. lv_example:: styles/lv_example_style_7
  :language: c

Line Draw Descriptor
--------------------

The :cpp:expr:`lv_draw_line_dsc_t` line descriptor defines line rendering with
these fields:

- ``p1``: First point of the line (supports floating-point coordinates).
- ``p2``: Second point of the line (supports floating-point coordinates).
- ``color``: Line color.
- ``width``: Line thickness.
- ``opa``: Line opacity (0...255).
- ``dash_width``: Length of dashes (0 means no dashes).
- ``dash_gap``: Length of gaps between dashes (0 means no dashes).
- ``round_start``: Rounds the line start.
- ``round_end``: Rounds the line end.
- ``raw_end``: Set to 1 to skip end calculations if they are unnecessary.

Functions for line drawing:

- ``lv_draw_line_dsc_init(&dsc)`` initializes a line descriptor.
- ``lv_draw_line(layer, &dsc)`` creates a task to draw a line.
- ``lv_draw_task_get_line_dsc(draw_task)`` retrieves the line descriptor.

.. lv_example:: widgets/canvas/lv_example_canvas_7
  :language: c

.. lv_example:: styles/lv_example_style_9
  :language: c

Triangle Draw Descriptor
------------------------

Triangles are defined by :cpp:expr:`lv_draw_triangle_dsc_t`, which includes:

- 3 points for the triangle's vertices.
- ``color``: Triangle color.
- ``opa``: Triangle opacity.
- ``grad``: Gradient options. If ``grad.dir`` is not ``LV_GRAD_DIR_NONE``, the
  ``color`` field is ignored. The ``opa`` field adjusts overall opacity.

Functions for triangle drawing:
- ``lv_draw_triangle_dsc_init(&dsc)`` initializes a triangle descriptor.
- ``lv_draw_triangle(layer, &dsc)`` creates a task to draw a triangle.
- ``lv_draw_task_get_triangle_dsc(draw_task)`` retrieves the triangle descriptor.

.. lv_example:: widgets/canvas/lv_example_canvas_9
  :language: c

Vector Draw Descriptor
-----------------------

TODO

Masking Operation
-----------------

There are several options to mask parts of a layer, widget, or drawing:

1. **Radius of Rectangles**:
   Set the `radius` style property or the `radius` in the draw descriptors. This
   creates rounded rectangles, borders, outlines, etc. However, the content of
   subsequent renderings will not be masked out in the corners.

2. **Clip Radius of Images**:
   Similar to rectangles, images can also be rendered with a `radius`. Since
   layer drawing and image drawing are handled the same way, this works for
   layers as well.

   You can draw various content on a layer and then render the layer with a
   ``clip_radius``, masking out all the content on the corners.

3. **Rectangle Mask Draw Task**:
   A special draw task can mask out a rectangle from a layer by setting the alpha
   channel of certain pixels to 0. To achieve this:

   - Create an :cpp:expr:`lv_draw_mask_rect_dsc_t` descriptor.
   - Set the ``area``, ``radius``, and ``keep_outside`` parameters. If
     ``keep_outside`` is set to 1, areas outside of ``area`` remain unchanged.
     Otherwise, they are cleared.
   - Call :cpp:expr:`lv_draw_mask_rect(layer, &dsc)`.

   Note: The layer must have a color format with an alpha channel, typically
   :cpp:expr:`LV_COLOR_FORMAT_ARGB8888`.

   In most cases, the *"Clip Radius of Images"* method is better because it
   blends the layer with a radius mask on the fly, avoiding a dedicated masking
   step. However, the *"Rectangle Mask Draw Task"* is useful when multiple areas
   need clearing or when the area to be masked differs from the layer area.

4. **Clip Corner Style Property**:
   Enabling ``..._style_clip_corner`` in a local or global style allows LVGL to
   create a layer for the top and bottom corner areas of a widget. It renders the
   children there and blends it by setting ``clip_radius`` to the layer.

5. **Bitmap Masking for Images**:
   Using ``..._style_bitmap_mask`` or ``bitmap_mask`` in
   :cpp:expr:`lv_draw_image_dsc_t` allows setting an A8 or L8 image as a mask
   for an image/layer during blending.

   - Limitation: The mask always aligns to the center, and only one bitmap mask
     can be used for an image/layer.
   - When ``..._style_bitmap_mask`` is used, LVGL automatically creates a layer,
     renders the widgets there, and applies the bitmap mask during blending.
   - Alternatively, the ``bitmap_mask`` property in the draw descriptor can be
     used directly for image drawing.

   By using the canvas widget with an :cpp:enumerator:`LV_COLOR_FORMAT_L8` buffer,
   bitmap masks can be rendered dynamically.

.. lv_example:: widgets/canvas/lv_example_label_4
  :language: c

.. lv_example:: widgets/canvas/lv_example_roller_3
  :language: c
