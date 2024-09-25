.. _nanosvg:

===============
NanoSvg decoder
===============

Allow the use of SVG images in LVGL.

Detailed introduction: https://github.com/memononen/nanosvg

NanoSVG is a simple stupid single-header-file SVG parse. 
The output of the parser is a list of cubic bezier shapes.

The library suits well for anything from rendering scalable 
icons in your editor application to prototyping a game.

NanoSVG supports a wide range of SVG features, but something 
may be missing, feel free to create a pull request!

The shapes in the SVG images are transformed by the viewBox 
and converted to specified units. That is, you should get 
the same looking data as your designed in your favorite app.

NanoSVG can return the paths in few different units. For 
example if you want to render an image, you may choose to 
get the paths in pixels, or if you are feeding the data 
into a CNC-cutter, you may want to use millimeters.

If enabled in ``lv_conf.h`` by :c:macro:`LV_USE_NANOSVG` LVGL will register a new
image decoder automatically so SVG files can be directly used as any
other image sources.

:Note: a file system driver needs to be registered to open images from 
files. Read more about it :ref:`overview_file_system` or just
enable one in ``lv_conf.h`` with ``LV_USE_FS_...``

.. _nanosvg_example:

Example
-------

.. include:: ../examples/libs/nanosvg/index.rst

.. _nanosvg_api:

API
---

:ref:`nanosvg`

