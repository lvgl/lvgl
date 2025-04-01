.. _draw_layers:

===========
Draw Layers
===========

Not to be confused with a :ref:`Display's main 4 layers <display_screen_layers>`, a
:dfn:`Draw Layer` is a buffer created during rendering, necessitated by certain style
properties, so different sets of pixels are correctly combined.  Factors requiring
such layers are:

- partial opacity
- bit-mask being applied
- blend mode
- clipped corners (a bit-mask application)
- transformations

    - scale
    - skew
    - rotation

Later that layer will be merged to the screen or its parent layer at the correct
point in the rendering sequence.



Layer Types
***********

Simple Layer
------------

The following style properties trigger the creation of a "Simple Layer":

- ``opa_layered``
- ``bitmap_mask_src``
- ``blend_mode``

In this case the Widget will be sliced into ``LV_DRAW_SW_LAYER_SIMPLE_BUF_SIZE``
sized chunks.

If there is no memory for a new chunk, LVGL will try allocating the layer after
another chunk is rendered and freed.


Transform Layer
---------------

The following style properties trigger the creation of a "Transform Layer":

- ``transform_scale_x``
- ``transform_scale_y``
- ``transform_skew_x``
- ``transform_skew_y``
- ``transform_rotate``

Due to the nature of transformations, the Widget being transformed (and its children)
must be rendered first, followed by the transformation step.  This necessitates a
temporary drawing area (layer), often larger than the Widget proper, to provide an
area of adequate size for the transformation.  LVGL tries to render as small area of
the widget as possible, but due to the nature of transformations no slicing is
possible in this case.


Clip Corner
-----------

The ``clip_corner`` style property also causes LVGL to create a 2 layers with radius
height for the top and bottom parts of the Widget.



Getting the Current Layer
*************************

The first parameter of the ``lv_draw_rect/label/etc`` functions is a layer.

In most cases a layer is not created, but an existing layer is used to draw there.

The draw API can be used in these cases and the current layer can be used differently
in each case:

1.  **In draw events**:
    In ``LV_EVENT_DRAW_MAIN/POST_BEGIN/...`` events the Widget is being rendered to a
    layer of the display or another temporary layer created earlier during rendering.
    The current target layer can be retrieved using :cpp:expr:`lv_event_get_layer(e)`.

    It also possible to create new layers in these events, but the previous layer is
    also required since it will be the parent layer in :cpp:func:`lv_draw_layer`.

2.  **Modifying the created Draw Tasks**:
    In :cpp:enumerator:`LV_EVENT_DRAW_TASK_ADDED` the draw tasks created by
    ``lv_draw_rect/label/etc`` can be modified.  It's not required to know the current
    layer to modify a draw task.  However, if something new also needs to be drawn with
    ``lv_draw_rect/label/etc`` the current layer is also required.

    The current layer can be read from the ``base`` draw descriptor.  For example:

    .. code-block:: c

        /* In LV_EVENT_DRAW_TASK_ADDED */
        lv_draw_task_t * t = lv_event_get_draw_task(e);
        lv_draw_base_dsc_t * draw_dsc = lv_draw_task_get_draw_dsc(t);

        lv_layer_t * current_layer = draw_dsc.layer;

3.  **Draw to the Canvas Widget**:
    The canvas itself doesn't store a layer, but one can be easily created and used
    like this:

    .. code-block:: c

        /* Initialize a layer */
        lv_layer_t layer;
        lv_canvas_init_layer(canvas, &layer);

        /* Draw something here */

        /* Wait until the rendering is ready */
        lv_canvas_finish_layer(canvas, &layer);



Creating a New Layer
********************

To create a new layer, use :cpp:func:`lv_draw_layer_create`:

.. code-block:: c

   lv_area_t layer_area = {10, 10, 80, 50}; /* Area of the new layer */
   lv_layer_t * new_layer = lv_draw_layer_create(parent_layer, LV_COLOR_FORMAT_RGB565, &layer_area);

Once the layer is created, draw tasks can be added to it
by using the :ref:`Draw API <draw_api>` and :ref:`Draw descriptors <draw_descriptors>`.
In most cases this means calling the ``lv_draw_rect/label/etc`` functions.

Finally, the layer must be rendered to its parent layer.  Since a layer behaves
similarly to an image, it can be rendered the same way as images:

.. code-block:: c

    lv_draw_image_dsc_t image_draw_dsc;
    lv_draw_image_dsc_init(&image_draw_dsc);
    image_draw_dsc.src = new_layer; /* Source image is the new layer. */
    /* Draw new layer to parent layer. */
    lv_draw_layer(parent_layer, &image_draw_dsc, &layer_area);



Memory Considerations
*********************


Layer Buffers
-------------

The buffer for a layer (where rendering occurs) is not allocated at creation.
Instead, it is allocated by :ref:`Draw Units` when the first :ref:`Draw Task <draw
tasks>` is dispatched.

Layer buffers can be large, so ensure there is sufficient heap memory or increase
:c:macro:`LV_MEM_SIZE` in ``lv_conf.h``.


Layer Type Memory Requirements
------------------------------

To save memory, LVGL can render certain types of layers in smaller chunks:

1.  **Simple Layers**:
    Simple layers can be rendered in chunks. For example, with
    ``opa_layered = 140``, only 10 lines of the layer can be rendered at a time,
    then the next 10 lines, and so on.
    This avoids allocating a large buffer for the entire layer. The buffer size for a
    chunk is set using :c:macro:`LV_DRAW_LAYER_SIMPLE_BUF_SIZE` in ``lv_conf.h``.

2.  **Transform Layers**:
    Transform Widgets cannot be rendered in chunks because transformations
    often affect pixels outside the given area. For such layers, LVGL allocates
    a buffer large enough to render the entire transformed area without limits.


Memory Limit for Layers
-----------------------

The total memory available for layers at once is controlled by
:c:macro:`LV_DRAW_LAYER_MAX_MEMORY` in ``lv_conf.h``.  If set to ``0``, there is no
limit.



API
***

.. API equals:
    lv_draw_layer_create
    LV_EVENT_DRAW_TASK_ADDED
    lv_event_get_layer
