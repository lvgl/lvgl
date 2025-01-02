.. _draw_layers:

================
Draw Layers
================

A layer is a buffer with a specified area where rendering occurs. Each display
has a "main" layer, but additional layers may be created internally during
rendering to handle tasks such as widget transformations.

Creating a New Layer
--------------------

To create a new layer, use :cpp:func:`lv_draw_layer_create`:

.. code-block:: c
   lv_area_t layer_area = {10, 10, 80, 50}; /* Area of the new layer */
   lv_layer_t * new_layer = lv_draw_layer_create(parent_layer, LV_COLOR_FORMAT_RGB565, &layer_area);

Once the layer is created, draw tasks can be added to it.
(See details in :ref:`draw_descriptors`.)

.. code-block:: c
    lv_draw_rect_dsc_t rect_draw_dsc;
    lv_draw_rect_dsc_init(&rect_draw_dsc);
    rect_draw_dsc.bg_color = lv_palette_main(LV_PALETTE_RED);
    rect_draw_dsc.radius = 5;

    lv_area_t rect_area = {15, 15, 70, 40};
    lv_draw_rect(new_layer, &rect_draw_dsc, &rect_area);

Finally, the layer must be rendered to its parent layer. Since a layer behaves
similarly to an image, it can be rendered the same way as images:

.. code-block:: c
    lv_draw_image_dsc_t image_draw_dsc;
    lv_draw_image_dsc_init(&image_draw_dsc);
    lv_draw_layer(new_layer, &image_draw_dsc, &layer_area);

Layer Buffers
-------------

The buffer for a layer (where rendering occurs) is not allocated at creation.
Instead, it is allocated by draw units when the first draw task is dispatched.

**Memory Considerations**:
Layer buffers can be large, so ensure sufficient heap memory or increase
``LV_MEM_SIZE`` in the configuration.

Layer Types
-----------

To save memory, LVGL can render certain types of layers in smaller chunks:

1. **Simple Layers**:
   Simple layers can be rendered in chunks. For example, with
   ``opa_layered = 140``, only 10 lines of the layer can be rendered at a time,
   then the next 10 lines, and so on.
   This avoids allocating a large buffer for the entire layer. The buffer size
   for a chunk is set using ``LV_DRAW_LAYER_SIMPLE_BUF_SIZE`` in `lv_conf.h`.

2. **Transformed Layers**:
   Transformed widgets cannot be rendered in chunks because transformations
   often affect pixels outside the given area. For such layers, LVGL allocates
   a buffer large enough to render the entire transformed area without limits.

Memory Limit for Layers
-----------------------

The total memory available for layers at once is controlled by
``LV_DRAW_LAYER_MAX_MEMORY``. If set to `0`, there is no limit.

