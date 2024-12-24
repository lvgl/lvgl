.. _font:

==============
Font (lv_font)
==============

In LVGL fonts are collections of bitmaps and other information required
to render images of individual letters (glyph). A font is stored in a
:cpp:type:`lv_font_t` variable and can be set in a style's *text_font* field.
For example:

.. code-block:: c

   lv_style_set_text_font(&my_style, &lv_font_montserrat_28);  /* Set a larger font */

Fonts have a **format** property. It describes how the glyph draw data is stored.
It has *2* categories: `Legacy simple format` and `Advanced format`.
In the most simple case, the font is stored in a simple array of bitmaps.
In the advanced format, the font can be stored in a different way like `Vector`, `SVG`, etc.

In case of the simple format, the value stored for a pixel determines the pixel's opacity.
This way, with higher *bpp (bit per pixel)*, the edges of the letter can be smoother.
The possible *bpp* values are 1, 2, 4 and 8 (higher values mean better quality).

The *format* property also affects the amount of memory needed to store a
font. For example, *format = LV_FONT_GLYPH_FORMAT_A4* makes a font nearly four times larger
compared to *format = LV_FONT_GLYPH_FORMAT_A1*.


Unicode support
***************

LVGL supports **UTF-8** encoded Unicode characters. Your editor needs to
be configured to save your code/text as UTF-8 (usually this the default)
and be sure that, :c:macro:`LV_TXT_ENC` is set to :c:macro:`LV_TXT_ENC_UTF8` in
*lv_conf.h*. (This is the default value)

To test it try

.. code-block:: c

   lv_obj_t * label1 = lv_label_create(lv_screen_active(), NULL);
   lv_label_set_text(label1, LV_SYMBOL_OK);

If all works well, a ✓ character should be displayed.


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


.. |Aacute| unicode:: U+000C1 .. LATIN CAPITAL LETTER A WITH ACUTE
.. |eacute| unicode:: U+000E9 .. LATIN SMALL LETTER E WITH ACUTE
.. |otilde| unicode:: U+000F5 .. LATIN SMALL LETTER O WITH TILDE
.. |Utilde| unicode:: U+00168 .. LATIN CAPITAL LETTER U WITH TILDE
.. |uuml|   unicode:: U+000FC .. LATIN SMALL LETTER U WITH DIAERESIS

For characters such as '|eacute|', '|uuml|', '|otilde|', '|Aacute|', and '|Utilde|', it is recommended
to use the single Unicode format (NFC) rather than decomposing them into
a base letter and diacritics (e.g. ``u + ¨``).

Complex languages where subsequent characters combine into a single glyph
and where the resulting glyph has no individual Unicode representation
(e.g., Devanagari), have limited support in LVGL.


Built-in fonts
**************

There are several built-in fonts in different sizes, which can be
enabled in ``lv_conf.h`` with *LV_FONT\_…* defines.

Normal fonts
------------

Containing all the ASCII characters, the degree symbol (U+00B0), the
bullet symbol (U+2022) and the built-in symbols (see below).

- :c:macro:`LV_FONT_MONTSERRAT_12`: 12 px font
- :c:macro:`LV_FONT_MONTSERRAT_14`: 14 px font
- :c:macro:`LV_FONT_MONTSERRAT_16`: 16 px font
- :c:macro:`LV_FONT_MONTSERRAT_18`: 18 px font
- :c:macro:`LV_FONT_MONTSERRAT_20`: 20 px font
- :c:macro:`LV_FONT_MONTSERRAT_22`: 22 px font
- :c:macro:`LV_FONT_MONTSERRAT_24`: 24 px font
- :c:macro:`LV_FONT_MONTSERRAT_26`: 26 px font
- :c:macro:`LV_FONT_MONTSERRAT_28`: 28 px font
- :c:macro:`LV_FONT_MONTSERRAT_30`: 30 px font
- :c:macro:`LV_FONT_MONTSERRAT_32`: 32 px font
- :c:macro:`LV_FONT_MONTSERRAT_34`: 34 px font
- :c:macro:`LV_FONT_MONTSERRAT_36`: 36 px font
- :c:macro:`LV_FONT_MONTSERRAT_38`: 38 px font
- :c:macro:`LV_FONT_MONTSERRAT_40`: 40 px font
- :c:macro:`LV_FONT_MONTSERRAT_42`: 42 px font
- :c:macro:`LV_FONT_MONTSERRAT_44`: 44 px font
- :c:macro:`LV_FONT_MONTSERRAT_46`: 46 px font
- :c:macro:`LV_FONT_MONTSERRAT_48`: 48 px font

Special fonts
-------------

-  :c:macro:`LV_FONT_MONTSERRAT_28_COMPRESSED`: Same as normal 28 px font but stored as a :ref:`fonts_compressed` with 3 bpp
-  :c:macro:`LV_FONT_DEJAVU_16_PERSIAN_HEBREW`: 16 px font with normal range + Hebrew, Arabic, Persian letters and all their forms
-  :c:macro:`LV_FONT_SIMSUN_16_CJK`: 16 px font with normal range plus 1000 of the most common CJK radicals
-  :c:macro:`LV_FONT_UNSCII_8`: 8 px pixel perfect font with only ASCII characters
-  :c:macro:`LV_FONT_UNSCII_16`: 16 px pixel perfect font with only ASCII characters

The built-in fonts are **global variables** with names like
:cpp:var:`lv_font_montserrat_16` for a 16 px height font. To use them in a
style, just add a pointer to a font variable like shown above.

The built-in fonts with *bpp = 4* contain the ASCII characters and use
the `Montserrat <https://fonts.google.com/specimen/Montserrat>`__ font.

In addition to the ASCII range, the following symbols are also added to
the built-in fonts from the `FontAwesome <https://fontawesome.com/>`__
font.

.. _fonts_symbols:

.. image:: /misc/symbols.png

The symbols can be used singly as:

.. code-block:: c

   lv_label_set_text(my_label, LV_SYMBOL_OK);

Or together with strings (compile time string concatenation):

.. code-block:: c

   lv_label_set_text(my_label, LV_SYMBOL_OK "Apply");

Or more symbols together:

.. code-block:: c

   lv_label_set_text(my_label, LV_SYMBOL_OK LV_SYMBOL_WIFI LV_SYMBOL_PLAY);


Special Features
****************

.. _bidi:

Bidirectional support
---------------------

Most languages use a Left-to-Right (LTR for short) writing direction,
however some languages (such as Hebrew, Persian or Arabic) use
Right-to-Left (RTL for short) direction.

LVGL not only supports RTL text but supports mixed (a.k.a.
bidirectional, BiDi) text rendering as well. Some examples:

.. image:: /misc/bidi.png

BiDi support is enabled by :c:macro:`LV_USE_BIDI` in *lv_conf.h*

All text has a base direction (LTR or RTL) which determines some
rendering rules and the default alignment of the text (Left or Right).
However, in LVGL, the base direction is not only applied to labels. It's
a general property which can be set for every Widget. If not set then it
will be inherited from the parent. This means it's enough to set the
base direction of a screen and every Widget will inherit it.

The default base direction for screens can be set by
:c:macro:`LV_BIDI_BASE_DIR_DEF` in *lv_conf.h* and other Widgets inherit the
base direction from their parent.

To set a Widget's base direction use :cpp:expr:`lv_obj_set_style_base_dir(widget, base_dir, selector)`.
The possible base directions are:

- :cpp:enumerator:`LV_BASE_DIR_LTR`: Left to Right base direction
- :cpp:enumerator:`LV_BASE_DIR_RTL`: Right to Left base direction
- :cpp:enumerator:`LV_BASE_DIR_AUTO`: Auto detect base direction

This list summarizes the effect of RTL base direction on Widgets:

- Create Widgets by default on the right
- ``lv_tabview``: Displays tabs from right to left
- ``lv_checkbox``: Shows the box on the right
- ``lv_buttonmatrix``: Shows buttons from right to left
- ``lv_list``: Shows icons on the right
- ``lv_dropdown``: Aligns options to the right
- The text strings in ``lv_table``, ``lv_buttonmatrix``, ``lv_keyboard``, ``lv_tabview``, ``lv_dropdown``, ``lv_roller`` are "BiDi processed" to be displayed correctly

Arabic and Persian support
--------------------------

There are some special rules to display Arabic and Persian characters:
the *form* of a character depends on its position in the text. A
different form of the same letter needs to be used when it is isolated,
at start, middle or end positions. Besides these, some conjunction rules
should also be taken into account.

LVGL supports these rules if :c:macro:`LV_USE_ARABIC_PERSIAN_CHARS` is enabled.

However, there are some limitations:

- Only displaying text is supported (e.g. on labels), text inputs (e.g. text area) don't support this feature.
- Static text (i.e. const) is not processed. E.g. text set by :cpp:func:`lv_label_set_text` will be "Arabic processed" but :cpp:func:`lv_label_set_text_static` won't.
- Text get functions (e.g. :cpp:func:`lv_label_get_text`) will return the processed text.

.. _fonts_compressed:

Compressed fonts
----------------

The built-in font engine supports compressed bitmaps.
Compressed fonts can be generated by

- ticking the ``Compressed`` check box in the online converter
- not passing the ``--no-compress`` flag to the offline converter (compression is applied by default)

Compression is more effective with larger fonts and higher bpp. However,
it's about 30% slower to render compressed fonts. Therefore, it's
recommended to compress only the largest fonts of a user interface,
because

- they need the most memory
- they can be compressed better
- and probably they are used less frequently then the medium-sized fonts, so the performance cost is smaller.

Compressed fonts also support ``bpp=3``.

Kerning
-------

Fonts may provide kerning information to adjust the spacing between specific
characters.

- The online converter generates kerning tables.
- The offline converter generates kerning tables unless ``--no-kerning`` is
  specified.
- FreeType integration does not currently support kerning.
- The Tiny TTF font engine supports GPOS and Kern tables.

To configure kerning at runtime, use :cpp:func:`lv_font_set_kerning`.

.. _add_font:


Adding a New Font
*****************

There are several ways to add a new font to your project:

1. The simplest method is to use the `Online font converter <https://lvgl.io/tools/fontconverter>`__.
   Just set the parameters, click the *Convert* button, copy the font to your project
   and use it. **Be sure to carefully read the steps provided on that site
   or you will get an error while converting.**
2. Use the `Offline font converter <https://github.com/lvgl/lv_font_conv>`__.
   (Requires Node.js to be installed)
3. If you want to create something like the built-in
   fonts (Montserrat font and symbols) but in a different size and/or
   ranges, you can use the ``built_in_font_gen.py`` script in
   ``lvgl/scripts/built_in_font`` folder. (This requires Python and
   ``lv_font_conv`` to be installed)

To declare a font in a file, use :cpp:expr:`LV_FONT_DECLARE(my_font_name)`.

To make fonts globally available (like the built-in fonts), add them to
:c:macro:`LV_FONT_CUSTOM_DECLARE` in *lv_conf.h*.


Adding New Symbols
******************

The built-in symbols are created from the `FontAwesome <https://fontawesome.com/>`__ font.

1. Search for a symbol on https://fontawesome.com. For example the
   `USB symbol <https://fontawesome.com/icons/usb?style=brands>`__. Copy its
   Unicode ID which is ``0xf287`` in this case.
2. Open the `Online font converter <https://lvgl.io/tools/fontconverter>`__.
   Add `FontAwesome.woff <https://lvgl.io/assets/others/FontAwesome5-Solid+Brands+Regular.woff>`__.
3. Set the parameters such as Name, Size, BPP. You'll use this name to
   declare and use the font in your code.
4. Add the Unicode ID of the symbol to the range field. E.g.\ ``0xf287``
   for the USB symbol. More symbols can be enumerated with ``,``.
5. Convert the font and copy the generated source code to your project.
   Make sure to compile the .c file of your font.
6. Declare the font using ``extern lv_font_t my_font_name;`` or simply
   use :cpp:expr:`LV_FONT_DECLARE(my_font_name)`.

**Using the symbol**

1. Convert the Unicode value to UTF8, for example on
   `this site <http://www.ltg.ed.ac.uk/~richard/utf-8.cgi?input=f287&mode=hex>`__.
   For ``0xf287`` the *Hex UTF-8 bytes* are ``EF 8A 87``.
2. Create a ``define`` string from the UTF8 values: ``#define MY_USB_SYMBOL "\xEF\x8A\x87"``
3. Create a label and set the text. Eg. :cpp:expr:`lv_label_set_text(label, MY_USB_SYMBOL)`

:note: :cpp:expr:`lv_label_set_text(label, MY_USB_SYMBOL)` searches for this
       symbol in the font defined in ``style.text.font`` properties. To use the
       symbol you may need to change it. Eg ``style.text.font = my_font_name``


Loading a Font at Run-Time
**************************

:cpp:func:`lv_binfont_create` can be used to load a font from a file. The font needs
to have a special binary format. (Not TTF or WOFF). Use
`lv_font_conv <https://github.com/lvgl/lv_font_conv/>`__ with the
``--format bin`` option to generate an LVGL compatible font file.

:note: To load a font :ref:`LVGL's filesystem <overview_file_system>`
       needs to be enabled and a driver must be added.

Example

.. code-block:: c

   lv_font_t *my_font = lv_binfont_create("X:/path/to/my_font.bin");
   if(my_font == NULL) return;

   /* Use the font */

   /* Free the font if not required anymore */
   lv_binfont_destroy(my_font);


Loading a Font from a Memory Buffer at Run-Time
***********************************************

:cpp:func:`lv_binfont_create_from_buffer` can be used to load a font from a memory buffer.
This function may be useful to load a font from an external file system, which is not
supported by LVGL. The font needs to be in the same format as if it were loaded from a file.

:note: To load a font from a buffer :ref:`LVGL's filesystem <overview_file_system>`
       needs to be enabled and the MEMFS driver must be added.

Example

.. code-block:: c

   lv_font_t *my_font;
   uint8_t *buf;
   uint32_t bufsize;

   /* Read font file into the buffer from the external file system */
   ...

   /* Load font from the buffer */
   my_font = lv_binfont_create_from_buffer((void *)buf, buf));
   if(my_font == NULL) return;
   /* Use the font */

   /* Free the font if not required anymore */
   lv_binfont_destroy(my_font);


Using a BDF Font
****************

Small displays with low resolution don't look pretty with automatically rendered fonts. A bitmap font provides
the solution, but it's necessary to convert the bitmap font (BDF) to a TTF.

Convert BDF to TTF
------------------

BDF are bitmap fonts where fonts are not described in outlines but in pixels. BDF files can be used but
they must be converted into the TTF format via mkttf. This tool uses potrace to generate outlines from
the bitmap information. The bitmap itself will be embedded into the TTF as well. `lv_font_conv <https://github.com/lvgl/lv_font_conv/>`__ uses
the embedded bitmap but it also needs the outlines. One could think you can use a fake MS Bitmap
only sfnt (ttf) (TTF without outlines) created by fontforge but this will not work.

Install imagemagick, python3, python3-fontforge and potrace

On Ubuntu Systems, just type

.. code:: bash

    sudo apt install imagemagick python3-fontforge potrace

Clone mkttf

.. code:: bash

    git clone https://github.com/Tblue/mkttf

Read the mkttf docs.

Former versions of imagemagick needs the imagemagick call in front of convert, identify and so on.
But newer versions don't. So you might probably change 2 lines in potrace-wrapper.sh.
Open potrace-wrapper.sh and remove imagemagick from line 55 and line 64.

line 55

.. code:: bash

    wh=($(identify -format '%[width]pt %[height]pt' "${input?}"))

line 64

.. code:: bash

    convert "${input?}" -sample '1000%' - \

It might be necessary to change the mkttf.py script.

line 1

.. code:: bash

    #!/usr/bin/env python3

Example for a 12px font
-----------------------

.. code-block:: console

    cd mkttf
    ./mkttf.py ./TerminusMedium-12-12.bdf
    Importing bitmaps from 0 additional fonts...
    Importing font `./TerminusMedium-12-12.bdf' into glyph background...
    Processing glyphs...
    Saving TTF file...
    Saving SFD file...
    Done!

The TTF TerminusMedium-001.000.ttf has been created from ./TerminusMedium-12-12.bdf.

Create font for lvgl

.. code:: bash

    lv_font_conv --bpp 1 --size 12 --no-compress --font TerminusMedium-001.000.ttf --range 0x20-0x7e,0xa1-0xff --format lvgl -o terminus_1bpp_12px.c

:note: use 1bpp because we don't use anti-aliasing. It doesn't look sharp on displays with a low resolution.


Adding a New Font Engine
************************

LVGL's font interface is designed to be very flexible but, even so, you
can add your own font engine in place of LVGL's internal one. For
example, you can use `FreeType <https://www.freetype.org/>`__ to
real-time render glyphs from TTF fonts or use an external flash to store
the font's bitmap and read them when the library needs them. FreeType can be used in LVGL as described in :ref:`Freetype <freetype>`.

To add a new font engine, a custom :cpp:type:`lv_font_t` variable needs to be created:

.. code-block:: c

   /* Describe the properties of a font */
   lv_font_t my_font;
   my_font.get_glyph_dsc = my_get_glyph_dsc_cb;        /* Set a callback to get info about glyphs */
   my_font.get_glyph_bitmap = my_get_glyph_bitmap_cb;  /* Set a callback to get bitmap of a glyph */
   my_font.line_height = height;                       /* The real line height where any text fits */
   my_font.base_line = base_line;                      /* Base line measured from the top of line_height */
   my_font.dsc = something_required;                   /* Store any implementation specific data here */
   my_font.user_data = user_data;                      /* Optionally some extra user data */

   ...

   /* Get info about glyph of `unicode_letter` in `font` font.
    * Store the result in `dsc_out`.
    * The next letter (`unicode_letter_next`) might be used to calculate the width required by this glyph (kerning)
    */
   bool my_get_glyph_dsc_cb(const lv_font_t * font, lv_font_glyph_dsc_t * dsc_out, uint32_t unicode_letter, uint32_t unicode_letter_next)
   {
       /* Your code here */

       /* Store the result.
        * For example ...
        */
       dsc_out->adv_w = 12;        /* Horizontal space required by the glyph in [px] */
       dsc_out->box_h = 8;         /* Height of the bitmap in [px] */
       dsc_out->box_w = 6;         /* Width of the bitmap in [px] */
       dsc_out->ofs_x = 0;         /* X offset of the bitmap in [pf] */
       dsc_out->ofs_y = 3;         /* Y offset of the bitmap measured from the as line */
       dsc_out->format= LV_FONT_GLYPH_FORMAT_A2;

       return true;                /* true: glyph found; false: glyph was not found */
   }


   /* Get the bitmap of `unicode_letter` from `font`. */
   const uint8_t * my_get_glyph_bitmap_cb(const lv_font_t * font, uint32_t unicode_letter)
   {
       /* Your code here */

       /* The bitmap should be a continuous bitstream where
        * each pixel is represented by `bpp` bits */

       return bitmap;    /* Or NULL if not found */
   }


Using Font Fallback
*******************

You can specify ``fallback`` in :cpp:type:`lv_font_t` to provide fallback to the
font. When the font fails to find glyph to a letter, it will try to let
font from ``fallback`` to handle.

``fallback`` can be chained, so it will try to solve until there is no ``fallback`` set.

.. code-block:: c

   /* Roboto font doesn't have support for CJK glyphs */
   lv_font_t *roboto = my_font_load_function();
   /* Droid Sans Fallback has more glyphs but its typeface doesn't look good as Roboto */
   lv_font_t *droid_sans_fallback = my_font_load_function();
   /* So now we can display Roboto for supported characters while having wider characters set support */
   roboto->fallback = droid_sans_fallback;


.. _fonts_api:

API
***
