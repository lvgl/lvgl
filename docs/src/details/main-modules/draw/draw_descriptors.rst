.. _draw_descriptors:

================
Draw Descriptors
================

Overview
********

Each :ref:`Draw Task <draw tasks>` type has its own draw descriptor type.  For
example, :cpp:type:`lv_draw_label_dsc_t` is used for label drawing,
:cpp:type:`lv_draw_image_dsc_t` is used for image drawing.


When an ``lv_draw_...`` function is called, it creates a Draw Task, copies the draw
descriptor into a ``malloc``\ ed memory block, and frees it automatically when
needed.  Therefore, local draw descriptor variables can be safely used.



Relation to Styles
******************

In most cases, style properties map 1-to-1 to draw descriptor fields.  For example:

- ``label_dsc.color`` corresponds to the ``text_color`` style property.
- ``shadow_dsc.width``, ``line_dsc.opa``, and ``arc_dsc.width`` map to
  ``shadow_width``, ``line_opa``, and ``arc_width`` in styles.

See :ref:`style_properties` to see the list of style properties and what they mean.



Base Draw Descriptor
********************

In each draw descriptor there is a generic "base descriptor" with
:cpp:type:`lv_draw_dsc_base_t` type and with ``base`` in its name.  For example
``label_dsc.base``.  This ``struct`` stores useful information about which Widget
and part created the draw descriptor.  See all the fields in
:cpp:type:`lv_draw_dsc_base_t`.

In an :cpp:enumerator:`LV_EVENT_DRAW_TASK_ADDED` event, the elements of the base draw
descriptor are very useful to identify the Draw Task.  For example:

.. code-block:: c

    /* In LV_EVENT_DRAW_TASK_ADDED */
    lv_draw_task_t * t = lv_event_get_draw_task(e);
    lv_draw_base_dsc_t * draw_dsc = lv_draw_task_get_draw_dsc(t);
    draw_dsc.obj;  /* The Widget for which the draw descriptor was created */
    draw_dsc.part; /* The Widget part for which the draw descriptor was created
                      E.g. LV_PART_INDICATOR */
    draw_dsc.id1;  /* A Widget type specific ID (e.g. table row index).
                      See the docs of the given Widget. */
    draw_dsc.id2;

    draw_dsc.layer; /* The target layer.
                       Required when a new Draw Tasks are also created */


Simple Initialization
----------------------

Before using a draw descriptor it needs to be initialized with
the related function.  For example, :cpp:expr:`lv_draw_label_dsc_init(&my_label_draw_dsc)`.

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

    lv_area_t a = {10, 10, 200, 50}; /* Draw label here */

    lv_draw_label(lv_event_get_layer(e), &my_label_draw_dsc, &a);


Initialization for Widgets
---------------------------

When rendering a part of a Widget, helper functions can initialize draw
descriptors based on the styles, and a specific Widget part in the current state.

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
---------------------------

In :cpp:enumerator:`LV_EVENT_DRAW_TASK_ADDED`, the draw descriptor of the ``draw_task`` can be
accessed (using :cpp:type:`lv_draw_task_get_label_dsc()` and similar functions)
and modified (to change color, text, font, etc.).  This means that in
:cpp:enumerator:`LV_EVENT_DRAW_TASK_ADDED`, the ``draw_task``\ s and draw descriptors
are already initialized and it's enough to change only a few specific values.

For example:

.. code-block:: c

    /* In LV_EVENT_DRAW_TASK_ADDED */
    lv_draw_task_t * t = lv_event_get_draw_task(e);
    lv_draw_label_dsc_t * draw_dsc = lv_draw_task_get_label_dsc(t);

    /* Check a few things in `draw_dsc->base` */

    /* Make the color lighter for longer texts */
    draw_dsc->color = lv_color_lighten(draw_dsc->color,
                                       LV_MIN(lv_strlen(draw_dsc->text) * 5, 255));

    /* Create new Draw Tasks if needed by calling
     * `lv_draw_...(draw_dsc->base.layer, ...)` functions */



Rectangle Draw Descriptor
*************************

:cpp:type:`lv_draw_rect_dsc_t` is a helper descriptor that combines:

- Fill
- Border
- Outline (a border with its own styles)
- Shadow
- Background image (an image with its own styles)

into a single call.

``lv_obj_init_draw_rect_dsc(obj, part, &dsc);`` initializes a draw descriptor
from a Widget, and ``lv_draw_rect(layer, &dsc, area)`` draws the rectangle in a
specified area.

.. lv_example:: widgets/canvas/lv_example_canvas_3
  :language: c



Fill Draw Descriptor
********************

The main fields of :cpp:type:`lv_draw_fill_dsc_t` are straightforward.  It has a
radius, opacity, and color to draw a rectangle.  If opacity is 0, no draw task will
be created.

- :cpp:expr:`lv_draw_fill_dsc_init(&dsc)` initializes a fill Draw Task.
- :cpp:expr:`lv_draw_sw_fill(layer, &dsc, area)` creates a Draw Task to fill an area.
- :cpp:expr:`lv_draw_task_get_fill_dsc(draw_task)` retrieves the fill descriptor from
  a Draw Task.



Gradients
*********

The ``grad`` field of the fill descriptor (or :cpp:type:`lv_grad_dsc_t` in general)
supports:

- Horizontal
- Vertical
- Skew
- Radial
- Conical

gradient types.

The following show some example gradients.

.. lv_example:: styles/lv_example_style_2
  :language: c

.. lv_example:: styles/lv_example_style_16
  :language: c

.. lv_example:: styles/lv_example_style_17
  :language: c

.. lv_example:: styles/lv_example_style_18
  :language: c

For each gradient type, multiple color and opacity values can be assigned.  These are
called "stops".  The maximum number of stops is limited to
:c:macro:`LV_GRADIENT_MAX_STOPS`.

A gradient is basically a transition of colors and opacities between stops.

Besides just setting the color and opacity of each stop, it is also possible to set
where they start relative to the whole gradient area.

For example with 3 stops it can be set like this:

- 10% red: 0--10% fully red
- 60% green: 10--60% transition from red to green, 60% is fully green
- 65% blue: fast transition from green to blue between 60%--65%. After 65% fully blue.

The position of the stops are called fractions or offsets and are 8 bit values where
0 is 0% and 255 is 100% of the whole gradient area.

:cpp:expr:`lv_grad_init_stops(grad_dsc, colors, opas, fracs, cnt)` initializes
a gradient descriptor with stops containing the color, opacity and fraction of each
stop.

.. code-block:: c

    static const lv_color_t colors[2] = {
        LV_COLOR_MAKE(0xe8, 0xe8, 0xe8),
        LV_COLOR_MAKE(0x79, 0x79, 0x79),
    };

    static const lv_opa_t opas[2] = {
        170,
        255,
    };

    static const uint8_t fracs[2] = {
        170,
        255,
    };

    lv_grad_init_stops(&grad, colors, opas, fracs, sizeof(colors) / sizeof(lv_color_t));

If the opacity array is ``NULL`` 255 will be used for each stop.  If the fractions
array is ``NULL`` the colors will be distributed evenly.  For example with 3 colors:
0%, 50%, 100%


Padding
-------

Linear, radial, and conic gradients are defined between two points or angles.  You
can define how to pad the areas outside of the start and end points or angles:

- :cpp:enumerator:`LV_GRAD_EXTEND_PAD`: Repeat the same color
- :cpp:enumerator:`LV_GRAD_EXTEND_REPEAT`: Repeat the pattern
- :cpp:enumerator:`LV_GRAD_EXTEND_REFLECT`: Repeat the pattern normally and mirrored alternately



Horizontal and Vertical Gradients
---------------------------------

The simplest and usually fastest gradient types are horizontal and vertical gradients.

After initializing the stops with :cpp:expr:`lv_grad_init_stops` call either
:cpp:expr:`lv_grad_horizontal_init(&grad_dsc)` or
:cpp:expr:`lv_grad_vertical_init(&grad_dsc)` to get a horizontal or vertical gradient
descriptor.

.. lv_example:: grad/lv_example_grad_1
  :language: c


Linear Gradients
----------------

The linear (or skew) gradient are similar to horizontal or vertical gradient but the
angle of the gradient can be controlled.

The linear gradient will be rendered along a line defined by 2 points.

After initializing the stops with :cpp:func:`lv_grad_init_stops` call
:cpp:expr:`lv_grad_linear_init(&grad_dsc, from_x, from_y, to_x, to_y, LV_GRAD_EXTEND_...)`
with your point values and extend pattern strategy to get a linear gradient descriptor.

.. lv_example:: grad/lv_example_grad_2
  :language: c


Radial Gradients
----------------

The radial gradient is described by two circles: an outer circle and an inner circle
(also called the focal point).  The gradient will be calculated between the focal
point's circle and the edge of the outer circle.

If the center of the focal point and the center of the main circle are the same, the
gradient will spread evenly in all directions.  If the center points are not the
same, the gradient will have an egg shape.

The focal point's circle should be inside the main circle.

After initializing the stops with :cpp:func:`lv_grad_init_stops`, the outer
circle can be set by:
:cpp:expr:`lv_grad_radial_init(&grad_dsc, center_x, center_y, edge_x, edge_y, LV_GRAD_EXTEND_...)`

For both the center and edge coordinates, ``px`` or ``lv_pct()`` values can be used.

The inner circle (focal point) can be set with:
:cpp:expr:`lv_grad_radial_set_focal(&grad_dsc, center_x, center_y, radius)`

.. lv_example:: grad/lv_example_grad_3
  :language: c


Conic Gradients
---------------

The conic gradient is defined between the angles of a circle, and colors are mapped
to each angle.

After initializing the stops with :cpp:func:`lv_grad_init_stops`, the conic gradient
can be set up with:
:cpp:expr:`lv_grad_conical_init(&grad, center_x, center_y, angle_start, angle_end, LV_GRAD_EXTEND_...)`

For both the center and edge coordinates, ``px`` or ``lv_pct()`` values can be used.

The zero angle is on the right-hand side, and 90 degrees is at the bottom.

.. lv_example:: grad/lv_example_grad_4
  :language: c



Border Draw Descriptor
**********************

The :cpp:type:`lv_draw_border_dsc_t` border descriptor has radius, opacity,
width, color, and side fields.  If the opacity or width is 0, no Draw Task will
be created.

``side`` can contain ORed values of :cpp:type:`lv_border_side_t`, such as
:cpp:enumerator:`LV_BORDER_SIDE_BOTTOM`.  :cpp:enumerator:`LV_BORDER_SIDE_ALL`
applies to all sides, while :cpp:enumerator:`LV_BORDER_SIDE_INTERNAL` is used by
higher layers (e.g. a table Widget) to calculate border sides.  However, the drawing
routine receives only simpler values.

The following functions are used for border drawing:

- :cpp:expr:`lv_draw_border_dsc_init(&dsc)` initializes a border Draw Task.
- :cpp:expr:`lv_draw_sw_border(layer, &dsc, area)` creates a Draw Task to draw a border
  inward from its area.
- :cpp:expr:`lv_draw_task_get_border_dsc(draw_task)` retrieves the border descriptor
  from a Draw Task.

.. lv_example:: styles/lv_example_style_3
  :language: c


Outlines
********

The outline is similar to the border but is drawn outside the object's draw area.

In practice, there is no dedicated outline descriptor like
``lv_draw_outline_dsc_t``, because from the rendering perspective, the
outline is simply another border rendered outside the object's bounds.

The outline is used only in :cpp:type:`lv_draw_rect_dsc_t` for convenience. The two
differences compared to borders in :cpp:type:`lv_draw_rect_dsc_t` are:

- There is an ``outline_pad`` property to specify the gap between the target area and
  the inner side of the outline. It can be negative. For example, if ``outline_pad =
  -width``, the outline will resemble a border.

- There is no ``border_side`` property for the outline. It's always rendered as a
  full rectangle.

.. lv_example:: styles/lv_example_style_4
  :language: c



Box Shadow Draw Descriptor
**************************

The :cpp:type:`lv_draw_box_shadow_dsc_t` box shadow descriptor describes a **rounded
rectangle-shaped shadow**.  It cannot generate shadows for arbitrary shapes, text, or
images.  It includes the following fields:

:radius:    Radius, :cpp:expr:`LV_RADIUS_CIRCLE`.
:color:     Shadow color.
:width:     Shadow width (blur radius).
:spread:    Expands the rectangle in all directions; can be negative.
:ofs_x:     Horizontal offset.
:ofs_y:     Vertical offset.
:opa:       Opacity (0--255 range). Values like ``LV_OPA_TRANSP``, ``LV_OPA_10``,
            etc., can also be used.
:bg_cover:  Set to 1 if the background will cover the shadow (a hint for the
            renderer to skip masking).

Note: Rendering large shadows may be slow or memory-intensive.

The following functions are used for box shadow drawing:

- :cpp:expr:`lv_draw_box_shadow_dsc_init(&dsc)` initializes a box shadow Draw Task.
- :cpp:expr:`lv_draw_sw_box_shadow(layer, &dsc, area)` creates a Draw Task for a rectangle's
  shadow. The shadow's size and position depend on the width, spread, and offset.
- :cpp:expr:`lv_draw_task_get_box_shadow_dsc(draw_task)` retrieves the box shadow
  descriptor from a Draw Task.

.. lv_example:: styles/lv_example_style_5
  :language: c


.. |deg|    unicode:: U+000B0 .. DEGREE SIGN

Image Draw Descriptor
*********************

The :cpp:type:`lv_draw_image_dsc_t` image descriptor defines the parameters for
image drawing.  It is a complex descriptor with the following options:

:src:              The image source, either a pointer to `lv_image_dsc_t` or a file path.
:opa:              Opacity in the 0--255 range. Options like
                   ``LV_OPA_TRANSP``, ``LV_OPA_10``, etc., can also be used.
:clip_radius:      Clips the corners of the image with this radius.  Use
                   `LV_RADIUS_CIRCLE` for the maximum radius.
:rotation:         Image rotation in 0.1-degree units (e.g., 234 means 23.4\ |deg|\ ).
:scale_x:          Horizontal scaling (zoom) of the image.
                   256 (LV_SCALE_NONE) means no zoom, 512 doubles the size, and 128 halves it.
:scale_y:          Same as ``scale_x`` but for vertical scaling.
:skew_x:           Horizontal skew (parallelogram-like transformation) in 0.1-degree
                   units (e.g., 456 means 45.6\ |deg|\ ).
:skew_y:           Vertical skew, similar to ``skew_x``.
:pivot:            The pivot point for transformations (scaling and rotation).
                   (0,0) is the top-left corner of the image and can be set outside the image.
:bitmap_mask_src:  Pointer to an A8 or L8 image descriptor used to mask the
                   image.  The mask is always center-aligned.
:recolor:          Mixes this color with the image. For :cpp:enumerator:`LV_COLOR_FORMAT_A8`,
                   this will be the visible pixels' color.
:recolor_opa:      Intensity of recoloring (0 means no recoloring, 255 means full cover).
:blend_mode:       Defines how to blend image pixels with the background.
                   See :cpp:type:`lv_blend_mode_t` for more details.
:antialias:        Set to 1 to enable anti-aliasing for transformations.
:tile:             Tiles the image (repeats it both horizontally and vertically) if the
                   image is smaller than the `image_area` field in `lv_draw_image_dsc_t`.
:image_area:       Indicates the original, non-clipped area where the image
                   is drawn.  This is essential for:

                   1. Layer rendering, where only part of a layer may be rendered and
                      ``clip_radius`` needs the original image dimensions.
                   2. Tiling, where the draw area is larger than the image.

:sup:              Internal field to store information about the palette or color of A8 images.

Functions for image drawing:

- :cpp:expr:`lv_draw_image_dsc_init(&dsc)` initializes an image draw descriptor.
- :cpp:expr:`lv_draw_image(layer, &dsc, area)` creates a task to draw an image in a given area.
- :cpp:expr:`lv_draw_task_get_image_dsc(draw_task)` retrieves the image descriptor from a task.

.. lv_example:: widgets/canvas/lv_example_canvas_6
  :language: c

.. lv_example:: styles/lv_example_style_6
  :language: c


Layers - Special Images
-----------------------

Layers are treated as images, so an :cpp:type:`lv_draw_image_dsc_t` can describe
how layers are blended into their parent layers. All image features apply to
layers as well.

``lv_draw_layer(layer, &dsc, area)`` initializes the blending of a layer back to
its parent layer. Additionally, image-drawing-related functions can be used for
layers.

For more details, see :ref:`layers`.



Label Draw Descriptor
*********************

The :cpp:type:`lv_draw_label_dsc_t` label descriptor provides extensive options
for controlling text rendering:

:text:          The text to render.
:font:          Font to use, with support for fallback fonts.
:color:         Text color.
:opa:           Text opacity.
:line_space:    Additional space between lines.
:letter_space:  Additional space between characters.
:ofs_x:         Horizontal text offset.
:ofs_y:         Vertical text offset.
:sel_start:     Index of the first character for selection (not byte index).
                ``LV_DRAW_LABEL_NO_TXT_SEL`` means no selection.
:sel_end:       Index of the last character for selection.
:sel_color:     Color of selected characters.
:sel_bg_color:  Background color for selected characters.
:align:         Text alignment. See :cpp:type:`lv_text_align_t`.
:bidi_dir:      Base direction for right-to-left text rendering (e.g., Arabic).
                See :cpp:type:`lv_base_dir_t`.
:decor:         Text decoration, e.g., underline. See :cpp:type:`lv_text_decor_t`.
:flag:          Flags for text rendering. See :cpp:type:`lv_text_flag_t`.
:text_length:   Number of characters to render (0 means render until `\0`).
:text_local:    Set to 1 to allocate a buffer and copy the text.
:text_static:   Indicates ``text`` is constant and its pointer can be cached.
:hint:          Pointer to externally stored data to speed up rendering.
                See :cpp:type:`lv_draw_label_hint_t`.

Functions for text drawing:

- :cpp:expr:`lv_draw_label_dsc_init(&dsc)` initializes a label draw descriptor.
- :cpp:expr:`lv_draw_label(layer, &dsc, area)` creates a task to render text in an area.
- :cpp:expr:`lv_draw_character(layer, &dsc, point, unicode_letter)` creates a task to
  draw a character at a specific point.
- :cpp:expr:`lv_draw_task_get_label_dsc(draw_task)` retrieves the label descriptor from a task.

For character-specific drawing in draw units, use
:cpp:expr:`lv_draw_label_iterate_characters(draw_unit, draw_dsc, area, callback)`.
This iterates through all characters, calculates their positions, and calls the
callback for rendering each character.  For callback details, see
:cpp:type:`lv_draw_glyph_cb_t`.

.. lv_example:: widgets/canvas/lv_example_canvas_4
  :language: c

.. lv_example:: styles/lv_example_style_8
  :language: c



Arc Draw Descriptor
*******************

The :cpp:type:`lv_draw_arc_dsc_t` arc descriptor defines arc rendering with
these fields:

:color:        Arc color.
:img_src:      Image source for the arc, or `NULL` if unused.
:width:        Arc thickness.
:start_angle:  Starting angle in degrees (e.g., 0° is 3 o'clock, 90° is 6 o'clock).
:end_angle:    Ending angle.
:center:       Arc center point.
:radius:       Arc radius.
:opa:          Arc opacity (0--255).
:rounded:      Rounds the arc ends.

Functions for arc drawing:

- :cpp:expr:`lv_draw_arc_dsc_init(&dsc)` initializes an arc descriptor.
- :cpp:expr:`lv_draw_arc(layer, &dsc)` creates a task to render an arc.
- :cpp:expr:`lv_draw_task_get_arc_dsc(draw_task)` retrieves arc descriptor from task.

.. lv_example:: widgets/canvas/lv_example_canvas_5
  :language: c

.. lv_example:: styles/lv_example_style_7
  :language: c



Line Draw Descriptor
********************

The :cpp:type:`lv_draw_line_dsc_t` line descriptor defines line rendering with
these fields:

:p1:           First point of line (supports floating-point coordinates).
:p2:           Second point of line (supports floating-point coordinates).
:color:        Line color.
:width:        Line thickness.
:opa:          Line opacity (0--255).
:dash_width:   Length of dashes (0 means no dashes).
:dash_gap:     Length of gaps between dashes (0 means no dashes).
:round_start:  Rounds the line start.
:round_end:    Rounds the line end.
:raw_end:      Set to 1 to skip end calculations if they are unnecessary.

Functions for line drawing:

- :cpp:expr:`lv_draw_line_dsc_init(&dsc)` initializes a line descriptor.
- :cpp:expr:`lv_draw_line(layer, &dsc)` creates a task to draw a line.
- :cpp:expr:`lv_draw_task_get_line_dsc(draw_task)` retrieves line descriptor.

.. lv_example:: widgets/canvas/lv_example_canvas_7
  :language: c

.. lv_example:: styles/lv_example_style_9
  :language: c



Triangle Draw Descriptor
************************

Triangles are defined by :cpp:type:`lv_draw_triangle_dsc_t`, which includes:

:p[3]:   3 points for the triangle's vertices.
:color:  Triangle color.
:opa:    Triangle opacity.
:grad:   Gradient options. If ``grad.dir`` is not ``LV_GRAD_DIR_NONE``, the
         ``color`` field is ignored. The ``opa`` field adjusts overall opacity.

Functions for triangle drawing:
- :cpp:expr:`lv_draw_triangle_dsc_init(&dsc)` initializes a triangle descriptor.
- :cpp:expr:`lv_draw_triangle(layer, &dsc)` creates a task to draw a triangle.
- :cpp:expr:`lv_draw_task_get_triangle_dsc(draw_task)` retrieves triangle descriptor.

.. lv_example:: widgets/canvas/lv_example_canvas_9
  :language: c



Vector Draw Descriptor
**********************

TODO



Masking Operation
*****************

There are several options to mask parts of a layer, Widget, or drawing:

1. **Radius of Rectangles**:
   Set the `radius` style property or the ``radius`` in the draw descriptors.  This
   creates rounded rectangles, borders, outlines, etc..  However, the content of
   subsequent renderings will not be masked out in the corners.

2. **Clip Radius of Images**:
   Similar to rectangles, images can also be rendered with a ``radius``.  Since
   layer drawing and image drawing are handled the same way, this works for
   layers as well.

   You can draw various content on a layer and then render the layer with a
   ``clip_radius``, masking out all the content on the corners.

3. **Rectangle Mask Draw Task**:
   A special Draw Task can mask out a rectangle from a layer by setting the alpha
   channel of certain pixels to 0.  To achieve this:

   - Create an :cpp:type:`lv_draw_mask_rect_dsc_t` descriptor.
   - Set ``area``, ``radius``, and ``keep_outside`` parameters. If
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
   create a layer for the top and bottom corner areas of a Widget.  It renders the
   children there and blends it by setting ``clip_radius`` to the layer.

5. **Bitmap Masking for Images**:
   Using ``..._style_bitmap_mask`` or ``bitmap_mask`` in
   :cpp:type:`lv_draw_image_dsc_t` allows setting an A8 or L8 image as a mask
   for an image/layer during blending.

   - Limitation:  The mask always aligns to the center, and only one bitmap mask can
     be used for an image/layer.
   - When ``..._style_bitmap_mask`` is used, LVGL automatically creates a layer,
     renders the Widgets there, and applies the bitmap mask during blending.
   - Alternatively, the ``bitmap_mask`` property in the draw descriptor can be
     used directly for image drawing.

   By using the Canvas Widget with an :cpp:enumerator:`LV_COLOR_FORMAT_L8` buffer,
   bitmap masks can be rendered dynamically.

.. lv_example:: widgets/canvas/lv_example_label_4
  :language: c

.. lv_example:: widgets/canvas/lv_example_roller_3
  :language: c



API
***

.. API equals:
    lv_draw_arc
    lv_draw_dsc_base_t
    lv_draw_image
    lv_draw_label
    lv_draw_label_dsc_init
    lv_draw_line
    lv_draw_mask_rect
    lv_draw_rect_dsc_t
    lv_draw_task_get_label_dsc
    lv_draw_triangle
    LV_EVENT_DRAW_TASK_ADDED
    lv_grad_dsc_t
    lv_grad_horizontal_init
