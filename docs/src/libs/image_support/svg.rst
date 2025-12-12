.. _svg:

===========
SVG Decoder
===========

The lv_svg extension provides makes it possible to use SVG images in your LVGL UI using the
`Scalable Vector Graphics (SVG) Tiny 1.2 Specification <https://www.w3.org/TR/SVGTiny12/>`__.

For a detailed introduction, see:  https://www.w3.org/TR/SVGTiny12/



Usage
*****

Enable :c:macro:`LV_USE_SVG` in ``lv_conf.h`` by setting its value to ``1``.

See the examples below.

If you need support for SVG animation attribute parsing,
you can set :c:macro:`LV_USE_SVG_ANIMATION` in ``lv_conf.h`` to ``1``.


As Image Source
***************

`lv_image` directly supports SVG images.  For example:

.. code-block:: c

    lv_image_set_src(widget, "S:path/to/example.svg");


Direct Rendering
****************

It is also possible to draw SVG vector graphics in draw events:

.. code:: c

    lv_svg_node_t * svg_doc;
    const char* svg_data = "<svg><rect x=\"0\" y=\"0\" width=\"100\" height=\"100\"/></svg>";

    /* Create an SVG DOM tree */
    svg_doc = lv_svg_load_data(svg_data, svg_len);
    ...

    /* Draw SVG image */
    lv_draw_svg(layer, svg_doc);
    ...

    /* Release the DOM tree */
    lv_svg_node_delete(svg_doc);

.. _svg_example:

Examples
********

.. include:: /examples/libs/svg/index.rst


.. _svg_api:

API
***

.. API equals:  lv_svg_load_data

.. API startswith:  lv_svg_
