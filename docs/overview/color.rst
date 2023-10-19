======
Colors
======

The color module handles all color-related functions like changing color
depth, creating colors from hex code, converting between color depths,
mixing colors, etc.

The type :cpp:type:`lv_color_t` is used to store a color. Its fields are set
according to :c:macro:`LV_COLOR_DEPTH` in ``lv_conf.h``. (See below)

Creating colors
***************

RGB
---

Create colors from Red, Green and Blue channel values:

.. code:: c

   //All channels are 0-255
   lv_color_t c = lv_color_make(red, green, blue);

   //From hex code 0x000000..0xFFFFFF interpreted as RED + GREEN + BLUE
   lv_color_t c = lv_color_hex(0x123456);

   //From 3 digits. Same as lv_color_hex(0x112233)
   lv_color_t c = lv_color_hex3(0x123);

HSV
---

Create colors from Hue, Saturation and Value values:

.. code:: c

   //h = 0..359, s = 0..100, v = 0..100
   lv_color_t c = lv_color_hsv_to_rgb(h, s, v);

   //All channels are 0-255
   lv_color_hsv_t c_hsv = lv_color_rgb_to_hsv(r, g, b);


   //From lv_color_t variable
   lv_color_hsv_t c_hsv = lv_color_to_hsv(color);

Palette
-------

LVGL includes `Material Design's palette <https://vuetifyjs.com/en/styles/colors/#material-colors>`__ of
colors. In this system all named colors have a nominal main color as
well as four darker and five lighter variants.

The names of the colors are as follows: 

- :c:macro:`LV_PALETTE_RED`
- :c:macro:`LV_PALETTE_PINK`
- :c:macro:`LV_PALETTE_PURPLE`
- :c:macro:`LV_PALETTE_DEEP_PURPLE`
- :c:macro:`LV_PALETTE_INDIGO`
- :c:macro:`LV_PALETTE_BLUE`
- :c:macro:`LV_PALETTE_LIGHT_BLUE`
- :c:macro:`LV_PALETTE_CYAN`
- :c:macro:`LV_PALETTE_TEAL`
- :c:macro:`LV_PALETTE_GREEN`
- :c:macro:`LV_PALETTE_LIGHT_GREEN`
- :c:macro:`LV_PALETTE_LIME`
- :c:macro:`LV_PALETTE_YELLOW`
- :c:macro:`LV_PALETTE_AMBER`
- :c:macro:`LV_PALETTE_ORANGE`
- :c:macro:`LV_PALETTE_DEEP_ORANGE`
- :c:macro:`LV_PALETTE_BROWN`
- :c:macro:`LV_PALETTE_BLUE_GREY`
- :c:macro:`LV_PALETTE_GREY`

To get the main color use
``lv_color_t c = lv_palette_main(LV_PALETTE_...)``.

For the lighter variants of a palette color use
``lv_color_t c = lv_palette_lighten(LV_PALETTE_..., v)``. ``v`` can be
1..5. For the darker variants of a palette color use
``lv_color_t c = lv_palette_darken(LV_PALETTE_..., v)``. ``v`` can be
1..4.

Modify and mix colors
---------------------

The following functions can modify a color:

.. code:: c

   // Lighten a color. 0: no change, 255: white
   lv_color_t c = lv_color_lighten(c, lvl);

   // Darken a color. 0: no change, 255: black
   lv_color_t c = lv_color_darken(lv_color_t c, lv_opa_t lvl);

   // Lighten or darken a color. 0: black, 128: no change 255: white
   lv_color_t c = lv_color_change_lightness(lv_color_t c, lv_opa_t lvl);


   // Mix two colors with a given ratio 0: full c2, 255: full c1, 128: half c1 and half c2
   lv_color_t c = lv_color_mix(c1, c2, ratio);

Built-in colors
---------------

:cpp:func:`lv_color_white` and :cpp:func:`lv_color_black` return ``0xFFFFFF`` and
``0x000000`` respectively.

Opacity
*******

To describe opacity the :cpp:type:`lv_opa_t` type is created from ``uint8_t``.
Some special purpose defines are also introduced:

-  :cpp:enumerator:`LV_OPA_TRANSP` Value: 0, means no opacity making the color
   completely transparent
-  :cpp:enumerator:`LV_OPA_10` Value: 25, means the color covers only a little
-  ``LV_OPA_20 ... OPA_80`` follow logically
-  :cpp:enumerator:`LV_OPA_90` Value: 229, means the color near completely covers
-  :cpp:enumerator:`LV_OPA_COVER` Value: 255, means the color completely covers (full
   opacity)

You can also use the ``LV_OPA_*`` defines in :cpp:func:`lv_color_mix` as a
mixing *ratio*.

Color types
***********

The following variable types are defined by the color module:

-  :cpp:union:`lv_color1_t` Monochrome color. Also has R, G, B fields for
   compatibility but they are always the same value (1 byte)
-  :cpp:union:`lv_color8_t` A structure to store R (3 bit),G (3 bit),B (2 bit)
   components for 8-bit colors (1 byte)
-  :cpp:class:`lv_color16_t` A structure to store R (5 bit),G (6 bit),B (5 bit)
   components for 16-bit colors (2 byte)
-  :cpp:class:`lv_color32_t` A structure to store R (8 bit),G (8 bit), B (8 bit)
   components for 24-bit colors (4 byte)
-  :cpp:type:`lv_color_t` Equal to ``lv_color1/8/16/24_t`` depending on the
   configured color depth setting
-  :cpp:type:`lv_color_int_t` ``uint8_t``, ``uint16_t`` or ``uint32_t``
   depending on the color depth setting. Used to build color arrays from
   plain numbers.
-  :cpp:type:`lv_opa_t` A simple ``uint8_t`` type to describe opacity.

The :cpp:type:`lv_color_t`, :cpp:union:`lv_color1_t`, :cpp:union:`lv_color8_t`, :cpp:class:`lv_color16_t`
and :cpp:class:`lv_color32_t` types have four fields:

-  :cpp:member:`red` red channel
-  :cpp:member:`green` green channel
-  :cpp:member:`blue` blue channel
-  :cpp:member:`full` red + green + blue as one number

You can set the current color depth in *lv_conf.h*, by setting the
:c:macro:`LV_COLOR_DEPTH` define to 1 (monochrome), 8, 16 or 32.

Convert color
-------------

You can convert a color from the current color depth to another. The
converter functions return with a number, so you have to use the
:cpp:member:`full` field to map a converted color back into a structure:

.. code:: c

   lv_color_t c;
   c.red   = 0x38;
   c.green = 0x70;
   c.blue  = 0xCC;

   lv_color1_t c1;
   c1.full = lv_color_to1(c);  /*Return 1 for light colors, 0 for dark colors*/

   lv_color8_t c8;
   c8.full = lv_color_to8(c);  /*Give a 8 bit number with the converted color*/

   lv_color16_t c16;
   c16.full = lv_color_to16(c); /*Give a 16 bit number with the converted color*/

   lv_color32_t c24;
   c32.full = lv_color_to32(c);    /*Give a 32 bit number with the converted color*/

API
***
