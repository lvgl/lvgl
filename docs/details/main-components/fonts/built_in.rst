====================
Built-in font engine
====================

Create a new font
*****************

Font converters
---------------

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


Declare fonts
-------------

To declare a font in a file, use :cpp:expr:`LV_FONT_DECLARE(my_font_name)`.

To make fonts globally available (like the built-in fonts), add them to
:c:macro:`LV_FONT_CUSTOM_DECLARE` in *lv_conf.h*.


Compressed fonts
****************

The bitmaps of fonts can be compressed by

- ticking the ``Compressed`` check box in the online converter
- not passing the ``--no-compress`` flag to the offline converter (compression is applied by default)

Compression is more effective with larger fonts and higher bpp. However,
it's about 30% slower to render compressed fonts. Therefore, it's
recommended to compress only the largest fonts of a user interface,
because

- they need the most memory
- they can be compressed better
- and probably they are used less frequently then the medium-sized fonts, so the performance cost is smaller.

Binary font loading
*******************

Load from file
--------------

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


Load from a Memory Buffer
-------------------------

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


