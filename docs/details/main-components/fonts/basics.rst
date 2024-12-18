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


Create new fonts
***************

LVGL support several font engines but the most simple way to
get stared is to use the built-in engine and create a new font
using the `Online font converter <https://lvgl.io/tools/fontconverter>`__.

Copy the created file into your project and declare it in a file,
using :cpp:expr:`LV_FONT_DECLARE(my_font_name)`.

After that the font can be used like these


.. code-block:: c
   LV_FONT_DECLARE(my_font_name);

   lv_style_set_text_font(&my_style, &my_font_name);

   //Or
   lv_obj_set_style_text_font(label1, &my_font_name, 0);


Read more:

  - TODO


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


Symbols
*******

Overview
--------

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


Adding New Symbols
------------------

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


