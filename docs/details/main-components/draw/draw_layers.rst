.. _draw_layers:

================
Draw Layers
================

A layer is a buffer with a given area on which rendering happens. Each display has a "main" layer, but
during rendering additional layers might be created internally to handle for example widget transformations.

In order to create a new layer use :cpp:func:`lv_draw_layer_create`:

.. code-block:: c
   lv_area_t layer_area = {10, 10, 80, 50}; /*Area of the the new layer. */
   lv_layer_t * new_layer = lv_draw_layer_create(parent_layer, LV_COLOR_FORMAT_RGB565, &layer_area);

Once the layer is created draw tasks can be created on it.
(See the details of :refr:`draw_descriptors`)

.. code-block:: c
    lv_draw_rect_dsc_t rect_draw_dsc;
    lv_draw_rect_dsc_init(&rect_draw_dsc);
    rect_draw_dsc.bg_color = lv_palette_main(LV_PALETTE_RED);
    rect_draw_dsc.radius = 5;

    lv_area_t rect_area = {15, 15, 70, 40};
    lv_draw_rect(new_layer, &rect_draw_dsc, &rect_area);

Finally the layer needs to be also rendered to the parent layer.
As a layer is very similar to an image it can be rendered the same way as the images.

.. code-block:: c
    lv_draw_image_dsc_t image_draw_dsc;
    lv_draw_image_dsc_init(&image_draw_dsc);
	lv_draw_layer(new_layer, &image_draw_dsc, &layer_area);

The buffer of the layer (where the rendering will happen) is not allocated when the layer is created,
but it's allocated by the draw units when the first draw task is dispatched to them.

Note that buffer of the layers can be really large, so make sure that there is enough heap
(or ``LV_MEM_SIZE`` is large enough) for the layers.

To save some memory LVGL can render some type of layers in in smaller chunk:

- **Simple layers**:  Simple layer means that the layer can be rendered in chunks.
For example with ``opa_layered = 140`` it's possible to render only 10 lines
from the layer. When it's ready go the the next 10 lines.
It avoids large memory allocations for the layer buffer.
The  buffer size for a chunk can be set by `LV_DRAW_LAYER_SIMPLE_BUF_SIZE` in lv_conf.h.
- **Transformed layers**: The widget is transformed and cannot be rendered in chunks.
It's because - due to the transformations -  pixel outside of
a given area will also contribute to the final image.
In this case there is no limitation on the buffer size.
LVGL will allocate as large buffer as needed to render the transformed area.*/

In both cases ``LV_DRAW_LAYER_MAX_MEMORY`` controls the total amount of memory can be allocated at once for layers.
If it's ``0`` there is no limit.

