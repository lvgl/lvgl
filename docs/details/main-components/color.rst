.. _color:

================
Color (lv_color)
================

The color module handles all color-related functions like changing color
depth, creating colors from hex code, converting between color depths,
mixing colors, etc.

The type :cpp:type:`lv_color_t` is used to store a color in RGB888 format.
This type and format is used in almost all APIs regardless of :cpp:expr:`LV_COLOR_DEPTH`.



.. _color_create:

Creating Colors
***************

RGB
---

Create colors from Red, Green and Blue channel values:

.. code-block:: c

   /* All channels are 0-255 */
   lv_color_t c = lv_color_make(red, green, blue);


   /* Same but can be used for const initialization as well */
   lv_color_t c = LV_COLOR_MAKE(red, green, blue);

   /* From hex code 0x000000..0xFFFFFF interpreted as RED + GREEN + BLUE */
   lv_color_t c = lv_color_hex(0x123456);

   /* From 3 digits. Same as lv_color_hex(0x112233) */
   lv_color_t c = lv_color_hex3(0x123);

HSV
---

Create colors from Hue, Saturation and Value values:

.. code-block:: c

   //h = 0..359, s = 0..100, v = 0..100
   lv_color_t c = lv_color_hsv_to_rgb(h, s, v);

   //All channels are 0-255
   lv_color_hsv_t c_hsv = lv_color_rgb_to_hsv(r, g, b);


   //From lv_color_t variable
   lv_color_hsv_t c_hsv = lv_color_to_hsv(color);

.. _color_palette:

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
:cpp:expr:`lv_color_t` ``c =`` :cpp:expr:`lv_palette_main(LV_PALETTE_...)`.

For the lighter variants of a palette color use
:cpp:expr:`lv_color_t` ``c =`` :cpp:expr:`lv_palette_lighten(LV_PALETTE_..., v)`. ``v`` can be
1..5. For the darker variants of a palette color use
:cpp:expr:`lv_color_t` ``c =`` :cpp:expr:`lv_palette_darken(LV_PALETTE_..., v)`. ``v`` can be
1..4.

.. _color_modify_and_mix:

Modify and mix colors
---------------------

The following functions can modify a color:

.. code-block:: c

   // Lighten a color. 0: no change, 255: white
   lv_color_t c = lv_color_lighten(c, lvl);

   // Darken a color. 0: no change, 255: black
   lv_color_t c = lv_color_darken(lv_color_t c, lv_opa_t lvl);

   // Lighten or darken a color. 0: black, 128: no change 255: white
   lv_color_t c = lv_color_change_lightness(lv_color_t c, lv_opa_t lvl);


   // Mix two colors with a given ratio 0: full c2, 255: full c1, 128: half c1 and half c2
   lv_color_t c = lv_color_mix(c1, c2, ratio);

.. _color_builtin:

Built-in colors
---------------

:cpp:func:`lv_color_white` and :cpp:func:`lv_color_black` return ``0xFFFFFF`` and
``0x000000`` respectively.



.. _color_opacity:

Opacity
*******

To describe opacity the :cpp:type:`lv_opa_t` type is created from ``uint8_t``.
Some special purpose defines are also introduced:

-  :cpp:enumerator:`LV_OPA_TRANSP` Value: 0, means no opacity making the color
   completely transparent
-  :cpp:enumerator:`LV_OPA_10` Value: 25, means the color covers only a little
-  ``LV_OPA_20 ... OPA_80`` follow logically
-  :cpp:enumerator:`LV_OPA_90` Value: 229, means the color nearly completely covers
-  :cpp:enumerator:`LV_OPA_COVER` Value: 255, means the color completely covers (full
   opacity)

You can also use the ``LV_OPA_*`` defines in :cpp:func:`lv_color_mix` as a
mixing *ratio*.



.. _color_api:

API
***
