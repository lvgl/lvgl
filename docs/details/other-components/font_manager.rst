.. _font_manager:

============
Font Manager
============

Font Manager is a secondary encapsulation based on :ref:`freetype`, which
facilitates upper-level applications to manage and use vector fonts. Currently
supported functions include:

- Font resource reference counting (reduces repeated creation of font resources).
- Font resource concatenation (font fallback).
- Font resource recycling mechanism (buffers recently deleted font resources to reduce the time overhead of repeated creation).


.. _font_manager_usage:

Usage
*****

Enable :c:macro:`LIB_FREETYPE` and `LV_USE_FONT_MANAGER` in ``lv_conf.h``.

Configure :c:macro:`LV_FONT_MANAGER_NAME_MAX_LEN` to set the maximum length of the font name.

Initialize Font Manager
-----------------------

Use :cpp:func:`lv_font_manager_create` to create a font manager, where the
:cpp:func:`recycle_cache_size` parameter is used to set the number of font recycling
caches,which can improve font creation efficiency.

Use :cpp:func:`lv_font_manager_add_path_static` to add a mapping between the font
file path and the custom font name, so that the application can access the font
resources more conveniently.  It should be noted that if the file path is not static
(assigned from a local variable), please use :cpp:func:`lv_font_manager_add_path` to
add the path.  This API will copy the path content to the internal management.

Use :cpp:func:`lv_font_manager_remove_path` to remove the font path mapping.

.. code-block:: c

   static lv_font_manager_t * g_font_manager = NULL;

   void font_manager_init(void)
   {
      /* Create font manager, with 8 fonts recycling buffers */
      g_font_manager = lv_font_manager_create(8);

      /* Add font path mapping to font manager */
      lv_font_manager_add_path_static(g_font_manager, "Lato-Regular", "./lvgl/examples/libs/freetype/Lato-Regular.ttf");
      lv_font_manager_add_path_static(g_font_manager, "MyFont", "./path/to/myfont.ttf");
   }

Create Font from Font Manager
-----------------------------

Use :cpp:func:`lv_font_manager_create_font` to create a font.  The parameters are
basically the same as :cpp:func:`lv_freetype_font_create`.

The :cpp:func:`font_family` parameter can be filled with the names of multiple fonts
(separated by ``,``) to achieve font concatenation (when the corresponding glyph is
not found in a font file, it will automatically search from the next concatenated
font).

.. code-block:: c

   static lv_font_t * g_font = NULL;

   /* Create font from font manager */
   lv_font_t * g_font = lv_font_manager_create_font(g_font_manager,
                                                    "Lato-Regular,MyFont",
                                                    LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                                    24,
                                                    LV_FREETYPE_FONT_STYLE_NORMAL);

   /* Create label with the font */
   lv_obj_t * label = lv_label_create(lv_screen_active());
   lv_obj_set_style_text_font(label, g_font, 0);
   lv_label_set_text(label, "Hello World!");

Delete Font
-----------

Use :cpp:func:`lv_font_manager_delete_font` to delete the font.
The font manager will mark the font resource as a recyclable font so that it can be
quickly created next time.

Note that you need to delete the widget that references the font first, and then
delete the font to avoid accessing wild pointers.

.. code-block:: c

   /* Delete label and font */
   lv_obj_delete(label);
   lv_font_manager_delete_font(g_font_manager, g_font);
   g_font = NULL;

Delete Font Manager
-------------------

Use :cpp:func:`lv_font_manager_delete` to destroy the entire font manager. It should
be noted that before destruction, it is necessary to ensure that the application has
deleted all fonts using :cpp:func:`lv_font_manager_delete_font`.  The font manager
will check the reference status of all allocated fonts.  If there are still fonts
being referenced, the font manager will fail to destroy and return false.


.. _font_manager_example:

Example
*******

.. include:: ../../examples/others/font_manager/index.rst

.. _font_manager_api:

API
***
