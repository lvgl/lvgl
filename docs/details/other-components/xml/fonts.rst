.. _xml_fonts:

=====
Fonts
=====

Overview
********

A ``<fonts>`` section can be added in ``globals.xml`` files.
Later, it might be supported in components and widgets to define local fonts and keep the global space cleaner.

Usage
*****

The following section creates a mapping between font names and their paths with various attributes:

.. code-block:: xml

	<fonts>
		<bin as_file="false" name="medium" src="path/to/file.ttf" range="0x20-0x7f" symbols="°" size="24"/>
		<tiny_ttf as_file="true" name="big" src_path="path/to/file.ttf" range="0x20-0x7f" symbols="auto" size="48"/>
		<freetype name="chinese" src_path="file.ttf" size="48" custom_freetype_attribute="abc"/>
	</fonts>

In ``<styles>`` and ``<view>``, fonts can be referenced by their name, e.g.,

.. code-block:: xml

    <style name="style1" text_font="medium"/>

The tag name determines how the font is loaded. Currently, only ``tinyttf as_file="true"`` is supported.

- ``bin``:
  - If ``as_file="true"``: Converts the font file to ``bin`` (see `lv_font_conv`)
    which will be loaded by ``lv_binfont_create()``.
  - If ``as_file="false"`` (default): On export, the font file will be converted to a C array LVGL font
    that can be used directly by LVGL.
- ``tinyttf``:
  - If ``as_file="true"``: Can be loaded directly by ``lv_tiny_ttf_create_file()``.
  - If ``as_file="false"`` (default): The font file will be converted to a raw C array on export
    that will be loaded by ``lv_tiny_ttf_create_data()``.
- ``freetype``: The file can be loaded directly by ``lv_freetype_font_create()``.

For simplicity, if ``as_file="false"``, fonts will be loaded as files in the preview.
Setting ``as_file="false"`` affects only the C export.

If the UI is created from XML at runtime and a ``globals.xml`` is parsed, the ``as_file="false"`` tags are skipped
because it is assumed that the user manually creates the mapping. This is because the XML parser cannot
automatically map an LVGL font definition like:

.. code-block:: c

   lv_font_t my_font_24;

to

.. code-block:: xml

   <bin name="my_font_24"/>

Exported Code
-------------

When C code is exported, global ``const lv_font_t * <font_name>`` variables are created, and in the
initialization function of the component library (e.g., ``my_lib_init_gen()``), the actual font is assigned.

In :cpp:expr:`lv_style_set_text_font(&style1, <font_name>)`, the created font is referenced.

Constants
---------

Constants can also be used with fonts.

.. code-block:: xml

	<consts>
		<int name="font_size" value="32">
			<variant name="size" case="small" value="24"/>
		</int>
	</consts>

	<fonts>
		<bin name="medium" src_path="file.ttf" range="0x20-0x7f" symbols="°" size="#font_size"/>
	</fonts>

Default Font
------------

``"lv_font_default"`` can be used to access ``LV_FONT_DEFAULT``. Other built-in fonts are not exposed by default.
