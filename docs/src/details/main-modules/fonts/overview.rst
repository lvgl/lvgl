.. |check|  unicode:: U+02713 .. CHECK MARK
.. |Aacute| unicode:: U+000C1 .. LATIN CAPITAL LETTER A WITH ACUTE
.. |eacute| unicode:: U+000E9 .. LATIN SMALL LETTER E WITH ACUTE
.. |otilde| unicode:: U+000F5 .. LATIN SMALL LETTER O WITH TILDE
.. |Utilde| unicode:: U+00168 .. LATIN CAPITAL LETTER U WITH TILDE
.. |uuml|   unicode:: U+000FC .. LATIN SMALL LETTER U WITH DIAERESIS
.. |uml|    unicode:: U+000A8 .. DIAERESIS

.. _font:

==============
Font (lv_font)
==============

What is a font?
***************

In LVGL fonts are collections of bitmaps and other information required
to render the images of the individual letters (glyphs). A font is stored in a
:cpp:type:`lv_font_t` variable and can be set in a style's *text_font* field.
For example:

.. code-block:: c

    lv_style_set_text_font(&my_style, &lv_font_montserrat_28);  /* Set a larger font */


Font Engines
************

Built in engine, usually good enough.

Easy to add a font, go to online converter, LV_FONT_DECLARE, and use. Set size, bpp, etc list, compbine multiple fonts.


List of available fonr engines + links

Unicode Support
***************

LVGL supports **UTF-8** encoded Unicode characters. Your editor needs to
be configured to save your code/text as UTF-8 (usually this the default)
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

Fonts may provide kerning information to adjust the spacing between specific
characters.

- The online converter generates kerning tables.
- The offline converter generates kerning tables unless ``--no-kerning`` is
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



Symbols
*******

The built-in symbols are created from the `FontAwesome <https://fontawesome.com/>`__ font.

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
6. Declare the font using ``extern lv_font_t my_font_name;`` or simply
   use :cpp:expr:`LV_FONT_DECLARE(my_font_name)`.

**Using the symbol**

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




.. _fonts_api:

API
***
