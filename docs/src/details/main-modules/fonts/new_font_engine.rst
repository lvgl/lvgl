
.. _new_font_engine:

========================
Adding a New Font Engine
========================

Overview
********

Fonts have a **format** property. It describes how the glyph data is stored.
At the time of writing, there are several possible values that this field can take, and those
values fall into 2 categories:

1. **Bitmap based**: 1, 2, 4 or 8-bpp and image format, and
2. **Vector based** vector, SVG; for the latter, the user provides the rendering logic.

For simple formats:

- the font is stored as an array of bitmaps, one bitmap per glyph;
- the value stored for each pixel determines the pixel's opacity, enabling edges
  to be smoother --- higher bpp values result in smoother edges.

For advanced formats, the font information is stored in its respective format.

The **format** property also affects the amount of memory needed to store a
font. For example, ``format = LV_FONT_GLYPH_FORMAT_A4`` makes a font nearly four
times larger compared to ``format = LV_FONT_GLYPH_FORMAT_A1``.

Example
*******

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
       dsc_out->ofs_x = 0;         /* X offset of the bitmap in [px] */
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



