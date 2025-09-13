.. include:: custom_tools.txt

.. _fonts:

=====
Fonts
=====

Normally fonts would be defined in ``globals.xml`` in order to make them available
to all components in your UI.

If a TTF font file is more than you need in your application, or taking too much
memory, you can limit the range of characters included in your application by
importing it as binary, and specify the range using the same syntax as the
LVGL font converter.  Here are 2 examples using at TTF file and converting it to
a C array with limited character ranges.

The following example demonstrates including fonts with a limited range of glyphs
to reduce the application's program-size footprint:

.. code-block:: xml

    <fonts>
        <bin name="font_hour_32" size="32" bpp="4" as_file="false" range="0x20-0x7F" symbols="" src_path="fonts/Inter_28_Bold.ttf"/>
        <bin name="inter_28" size="28" bpp="4" as_file="false" range="0x20-0x7F" symbols="" src_path="fonts/Inter_28_Bold.ttf"/>
        <bin name="font_hour_25" size="25" bpp="4" as_file="false" range="0x20-0x7F" symbols="" src_path="fonts/Inter_28_Bold.ttf"/>
    </fonts>

You can also use fonts directly from a ``.ttf`` file by using a ``<tiny_ttf>`` element
instead of a ``<bin>`` element.

.. caution::  This requires a RAM footprint the size of the ``.ttf`` file, but if
              the RAM is available, it provides an extremely versatile way to make
              a font available to UI components!