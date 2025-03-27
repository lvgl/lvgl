.. _draw:

=======
Drawing
=======

What is Drawing?
****************

Drawing (also known as :dfn:`rendering`) is writing pixel colors into a buffer where
they will be delivered to a display panel as pixels.  Sometimes this is done by
copying colors from places like a "background color" property.  Other times it
involves computing those colors before they are written (e.g. combining them with
other colors when an object has partial opacity).

The following sections cover the LVGL drawing logic and how to use it and optionally
tune it to fit your particular project (e.g. if you have a GPU or other resources
that you would like to get involved).

.. toctree::
    :maxdepth: 2

    draw_pipeline
    draw_api
    draw_layers
    draw_descriptors
