
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

:note: To load a font :ref:`LVGL's filesystem <file_system>`
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

:note: To load a font from a buffer :ref:`LVGL's filesystem <file_system>`
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


