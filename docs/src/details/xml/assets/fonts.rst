.. _xml_fonts:

=====
Fonts
=====

.. |nbsp|   unicode:: U+000A0 .. NO-BREAK SPACE
    :trim:

Overview
********

In XML, fonts are considered external resources. A target font engine needs to be selected
and named in order to be referenced in XML files later.

Registering Fonts
*****************

From File
---------

Fonts used directly from a TTF file can be listed in the ``<fonts>`` section
of ``globals.xml``.

Each child of ``<fonts>`` starts with the desired font engine's name, a
``name`` property, and other properties depending on the selected engine.

``as_file`` must be set to ``true`` to indicate that the font is treated as a file.

For example:

.. code-block:: xml

    <globals>
        <fonts>
            <bin as_file="false" name="medium" src="path/to/file.ttf" range="0x20-0x7f" symbols="°" size="24"/>
            <tiny_ttf as_file="true" name="big" src_path="path/to/file.ttf" range="0x20-0x7f" size="48"/>
        </fonts>
    </globals>

When registering ``globals.xml`` with
:cpp:expr:`lv_xml_register_component_from_file("A:path/to/globals.xml")`,
fonts are automatically created with the selected font engine and mapped to the given names.

The fonts can have relative paths in ``globals.xml``. Before registering ``globals.xml``,
:cpp:expr:`lv_xml_set_default_asset_path("path/prefix/")` can be called to set the parent folder.
The font paths will then be appended to this base path.

From Data
---------

If a font is used from C data (arrays) compiled into the firmware, the font's pointer can be registered
to XML like this:

.. code-block:: c

   lv_xml_register_font(scope, "font_name", &my_font);

Fonts must be registered before any XML files referencing them are registered, so that the font data
is already known.

In LVGL's UI Editor, when using fonts as data, it's also necessary to add the fonts to
``globals.xml`` with ``as_file=false``, so the Editor can generate the required
C files and also perform validation when referencing data fonts.

Usage in XML
************

Once a font is registered, it can be referenced by its name in styles or even ``<api>`` properties:

.. code-block:: xml

    <component>
        <api>
            <prop name="title_font" type="font" default="my_font_32"/>
        </api>

        <styles>
            <style name="subtitle" text_font="my_font_24"/>
        </styles>

        <view flex_flow="column">
            <lv_label style_text_font="$title_font" text="Title"/>
            <lv_label text="I'm the subtitle">
                <style name="subtitle"/>
            </lv_label>
        </view>

Font Engines
************

``<bin>``
---------

Binary fonts can either reference:

1. A ".bin" font file created by LVGL's Font converter from a TTF file (when ``as_file="true"``).
   The font will be loaded by :cpp:expr:`lv_binfont_create`.
2. An :cpp:expr:`lv_font_t` pointer compiled into the firmware (``as_file="false"``).
   No loading is needed as :cpp:expr:`lv_font_t` can be used directly.

``<bin>`` fonts require these properties:

:``name``:        The name to reference the font later
:``src_path``:    Path to the font file
:``size``:        Font size in px (e.g., "12")
:``bpp``:         Bits-per-pixel: 1, 2, 4, or 8
:``as_file``:     ``true`` if the font is a ".bin" file, ``false`` if it is an :cpp:expr:`lv_font_t` in C

LVGL's UI Editor always generates a call to :cpp:expr:`lv_xml_register_font` using the set ``name``.
If ``as_file`` is:

- ``false``: It generates a C file with the :cpp:expr:`lv_font_t` structs.
- ``true``: It generates a ".bin" file.

Binary fonts also support selecting a subset of characters:

:``range``:       For example, ``"0x30-0x39 0x100-0x200"`` to specify Unicode ranges to include.
                  The default is ``"0x20-0x7F"`` to cover the ASCII range.
:``symbols``:     List of extra characters to add, e.g., ``"°ÁŐÚ"``. Can be used together with ``range``.
                  Default is empty (no extras).

``<tiny_ttf>``
--------------

TinyTTF fonts use the :ref:`tiny_ttf` engine to load TTF files directly or from data.

Required properties:

:``name``:        The name to reference the font later
:``src_path``:    Path to the font file
:``size``:        Font size in px (e.g., "12")
:``as_file``:     ``true`` or ``false``

LVGL's UI Editor generates a call to :cpp:expr:`lv_xml_register_font` using the set ``name``.
If ``as_file`` is:

- ``false``: A C file with raw file data is also generated.
- ``true``: Nothing else is generated, as the TTF file will be used directly.

``<freetype>``
--------------

FreeType fonts use the :ref:`freetype` engine to load TTF files directly. Loading from data is not supported,
so ``as_file`` is always considered ``true``.

Required properties:

:``name``:        The name to reference the font later
:``src_path``:    Path to the font file
:``size``:        Font size in px (e.g., "12")

LVGL's UI Editor generates a call to :cpp:expr:`lv_xml_register_font` using the set ``name``.
