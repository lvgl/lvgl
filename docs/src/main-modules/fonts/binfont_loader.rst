
.. _binfont_loader:

==============
BinFont Loader
==============

Overview
********

:cpp:func:`lv_binfont_create` can be used to load a font from a file. The font needs
to have a special binary format. (Not TTF or WOFF). Use
`lv_font_conv <https://github.com/lvgl/lv_font_conv/>`__ with the
``--format bin`` option to generate an LVGL compatible font file.

Loading from File
*****************

.. note:: To load a font :ref:`LVGL's filesystem <file_system>`
          needs to be enabled and a driver must be added.

Example

.. code-block:: c

   lv_font_t *my_font = lv_binfont_create("X:/path/to/my_font.bin");
   if(my_font == NULL) return;

   /* Use the font */

   /* Free the font if not required anymore */
   lv_binfont_destroy(my_font);



Loading from Memory
*******************

:cpp:func:`lv_binfont_create_from_buffer` can be used to load a font from a memory buffer.
This function may be useful to load a font from an external file system, which is not
supported by LVGL. The font needs to be in the same format as if it were loaded from a file.

.. note:: To load a font from a buffer :ref:`LVGL's filesystem <file_system>`
          needs to be enabled and the MEMFS driver must be added.

Example

.. code-block:: c

   lv_font_t *my_font;
   uint8_t *buf;
   uint32_t bufsize;

   /* Read font file into the buffer from the external file system */
   ...

   /* Load font from the buffer */
   my_font = lv_binfont_create_from_buffer((void *)buf, bufsize);
   if(my_font == NULL) return;

   /* Use the font */

   /* Free the font if not required anymore */
   lv_binfont_destroy(my_font);



Dynamic Loading
***************

By default, the entire glyph bitmap data is loaded into memory when a font is created.
For fonts with many glyphs, this can consume significant RAM. The dynamic loading feature
allows glyph bitmaps to be loaded on-demand from the font file, reducing memory usage.

.. note:: To enable dynamic loading, set ``LV_BINFONT_DYNAMIC_LOAD`` to ``1`` in ``lv_conf.h``.

How It Works
------------

- **Static mode (default)**: All glyph bitmaps are loaded into memory at font creation time.
  The font file is closed after loading.

- **Dynamic mode**: Only glyph metadata is loaded initially. Glyph bitmaps are read from
  the font file on-demand when needed. The font file remains open for the lifetime of the font.

Memory Comparison
-----------------

+----------------+---------------------------+---------------------------+
| Mode           | Memory Usage              | File Handle               |
+================+===========================+===========================+
| Static         | All glyph bitmaps in RAM  | Closed after loading      |
+----------------+---------------------------+---------------------------+
| Dynamic        | Only largest glyph buffer | Kept open                 |
+----------------+---------------------------+---------------------------+

Thread Safety
-------------

Dynamic loading is thread-safe. A mutex protects access to the shared file handle
and glyph buffer. However, only one "raw bitmap" can be held at a time per font.
Always call :cpp:func:`lv_font_glyph_release_draw_data` after using raw bitmap data.

Example

.. code-block:: c

   /* Enable LV_BINFONT_DYNAMIC_LOAD in lv_conf.h */

   lv_font_t *my_font = lv_binfont_create("X:/path/to/my_font.bin");
   if(my_font == NULL) return;

   /* Use the font normally - dynamic loading is transparent */
   lv_obj_set_style_text_font(label, my_font, 0);

   /* The font file remains open until destroy is called */
   lv_binfont_destroy(my_font);

Limitations
-----------

- The font file must remain accessible for the lifetime of the font.
- Slightly slower rendering due to file I/O for each glyph.
- Not suitable for fonts stored on slow storage media.

