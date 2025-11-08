.. |check|  unicode:: U+02713 .. CHECK MARK
.. |Aacute| unicode:: U+000C1 .. LATIN CAPITAL LETTER A WITH ACUTE
.. |eacute| unicode:: U+000E9 .. LATIN SMALL LETTER E WITH ACUTE
.. |otilde| unicode:: U+000F5 .. LATIN SMALL LETTER O WITH TILDE
.. |Utilde| unicode:: U+00168 .. LATIN CAPITAL LETTER U WITH TILDE
.. |uuml|   unicode:: U+000FC .. LATIN SMALL LETTER U WITH DIAERESIS
.. |uml|    unicode:: U+000A8 .. DIAERESIS

.. _font:

========
Overview
========

What is a Font?
***************

In LVGL fonts are collections of bitmaps and other information required
to render the images of the individual letters (glyphs). A font is stored in a
:cpp:type:`lv_font_t` variable and can be set in a style's *text_font* field.
For example:

.. code-block:: c

    lv_style_set_text_font(&my_style, &lv_font_montserrat_28);  /* Set a larger font */

Font Engines
************

A font engine is some C code that allows LVGL to extract various information
from the fonts, such as character (glyph) information or bitmap.

LVGL's built-in font engine is suitable for most typical cases.
It can handle various bit-per-pixel settings (1, 2, 3, 4, 8) in bitmaps,
kerning, having only selected character ranges from multiple fonts,
compressing bitmaps, and several others.

The built-in font engine is also the easiest to use:

1. Go to https://lvgl.io/tools/fontconverter
2. Upload font(s) and set the ranges and/or specify a list of characters to include and other parameters
3. Click the "Submit" button and copy the generated file to your project
4. In a C file add :cpp:expr:`LV_FONT_DECLARE(font_name)` to declare the font
5. Use the font like ``lv_style_set_text_font(&my_style, &font_name);``
   or ``lv_obj_set_style_text_font(label1, &font_name, 0);``

LVGL also supports several other font engines:

- ``fmt_txt``: This is the built-in font engine that stores the fonts as a C array
- ``binfont``: Similar to the built-in format, but the font is stored as a file, so it can be loaded at runtime too
- ``tiny_ttf``: Small vector graphics engine to load TTF files at runtime at any size
- ``freetype``: Well known font rendering library load and render TTF fonts at runtime. Also supports letter strokes.

Unicode Support
***************

LVGL supports **UTF-8** encoded Unicode characters. Your editor needs to
be configured to save your code/text as UTF-8 (usually this is the default)
and be sure that :c:macro:`LV_TXT_ENC` is set to :c:macro:`LV_TXT_ENC_UTF8` in
``lv_conf.h``. (This is the default value.)

To test it try

.. code-block:: c

   lv_obj_t * label1 = lv_label_create(lv_screen_active(), NULL);
   lv_label_set_text(label1, LV_SYMBOL_OK);

If all works well, a '\ |check|\ ' character should be displayed.


Typesetting
***********

Although LVGL can decode and display any Unicode characters
(assuming the font supports them), LVGL cannot correctly render
all complex languages.

The standard Latin-based languages (e.g., English, Spanish, German)
and East Asian languages such as Chinese, Japanese, and Korean (CJK)
are relatively straightforward, as their characters are simply
written from left to right.

Languages like Arabic, Persian, and Hebrew, which use Right-to-Left
(RTL) or mixed writing directions, are also supported in LVGL.
Learn more :ref:`here <bidi>`.

For characters such as '|eacute|', '|uuml|', '|otilde|', '|Aacute|', and '|Utilde|',
it is recommended to use the single Unicode format (NFC) rather than decomposing them
into a base letter and diacritics (e.g. ``u`` + |uml|).

Complex languages where subsequent characters combine into a single glyph
and where the resulting glyph has no individual Unicode representation
(e.g., Devanagari), have limited support in LVGL.


Kerning
*******

Fonts usually provide kerning information to adjust the spacing between specific
characters.

- The `Online converter <https://lvgl.io/tools/fontconverter>`__ generates kerning tables.
- The `Offline converter <https://github.com/lvgl/lv_font_conv/>`__ generates kerning tables unless ``--no-kerning`` is
  specified.
- FreeType integration does not currently support kerning.
- The Tiny TTF font engine supports GPOS (Glyph Positioning) and Kern tables.

To configure kerning at runtime, use :cpp:func:`lv_font_set_kerning`.


Using Font Fallback
*******************

If the font in use does not have a glyph needed in a text-rendering task, you can
specify a ``fallback`` font to be used in :cpp:type:`lv_font_t`.

``fallback`` can be chained, so it will try to solve until there is no ``fallback`` set.

.. code-block:: c

   /* Roboto font doesn't have support for CJK glyphs */
   lv_font_t *roboto = my_font_load_function();
   /* Droid Sans Fallback has more glyphs but its typeface doesn't look good as Roboto */
   lv_font_t *droid_sans_fallback = my_font_load_function();
   /* So now we can display Roboto for supported characters while having wider characters set support */
   roboto->fallback = droid_sans_fallback;


.. _font_symbols:

Symbols
*******

LVGL supports some predefined "symbols". A symbol is a specific Unicode character
in a font with an icon-like image. The symbols have names like ``LV_SYMBOL_OK``,
``LV_SYMBOL_HOME``, etc. See the full list of predefined symbols below:

.. image:: /_static/images/symbols.png

The symbols in the :ref:`built-in fonts <built_in_fonts>` are created from
the `FontAwesome <https://fontawesome.com/>`__ font.

Using these symbols is very simple:


.. code-block:: c

    lv_label_set_text(label, LV_SYMBOL_OK); /*Just a symbol*/
    lv_label_set_text(label, LV_SYMBOL_OK "Apply"); /*Concatenate with a string*/


.. _font_adding_a_custom_symbol:

Adding a Custom Symbol
----------------------

1. Search for a symbol on https://fontawesome.com. For example the
   `USB symbol <https://fontawesome.com/icons/usb?style=brands>`__. Copy its
   Unicode ID which is ``0xf287``.
2. Open the `Online font converter <https://lvgl.io/tools/fontconverter>`__.
   Add `FontAwesome.woff <https://lvgl.io/assets/others/FontAwesome5-Solid+Brands+Regular.woff>`__.
3. Set the parameters such as Name, Size, BPP. You'll use this name to
   declare and use the font in your code.
4. Add the Unicode ID of the symbol to the range field. E.g. ``0xf287``
   for the USB symbol. More symbols can be enumerated with ``,``.
5. Convert the font and copy the generated source code to your project.
   Make sure to compile the ``.c`` file of your font.
6. Declare the font using :cpp:expr:`LV_FONT_DECLARE(my_font_name)`.


Using Your Custom Symbol
------------------------

1. Convert the Unicode value to UTF8, for example on
   `this site <http://www.ltg.ed.ac.uk/~richard/utf-8.cgi?input=f287&mode=hex>`__.
   For ``0xf287`` the *Hex UTF-8 bytes* are ``EF 8A 87``.
2. Create a ``#define`` string from the UTF8 values: ``#define MY_USB_SYMBOL "\xEF\x8A\x87"``
3. Create a label and set the text. Eg. :cpp:expr:`lv_label_set_text(label, MY_USB_SYMBOL)`

:note: :cpp:expr:`lv_label_set_text(label, MY_USB_SYMBOL)` searches for this symbol
       in the font defined in the style's ``text.font`` property. To use the symbol
       you will need to set the style's text font to use the generated font, e.g.
       :cpp:expr:`lv_style_set_text_font(&my_style, &my_font_name)` or
       :cpp:expr:`lv_obj_set_style_text_font(label, &my_font_name, 0)`.

Of course any other fonts can be used, just make sure that they define the
symbols you need.

.. _fonts_api:

API
***
