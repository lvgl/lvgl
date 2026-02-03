.. _font_manager:

============
Font Manager
============

The font manager is a secondary encapsulation of the LVGL font engine,
which facilitates the use and management of fonts for applications.
The font management functions currently provided include:

- Font resource reference counting (reduces repeated creation of font resources).
- Font resource concatenation (font fallback).
- Font resource recycling mechanism (buffers recently deleted font resources to
  reduce the time overhead of repeated creation).



.. _font_manager_usage:

Usage
*****

Enable FreeType and Font Manager in ``lv_conf.h`` by setting the
:c:macro:`LV_USE_FONT_MANAGER` macros to non-zero
values, and configure :c:macro:`LV_FONT_MANAGER_NAME_MAX_LEN` to set the maximum
length of the font name.

Initialize Font Manager
-----------------------

Use :cpp:func:`lv_font_manager_create` to create a font manager, where the
:cpp:func:`recycle_cache_size` parameter is used to set the number of font recycling
caches, which can improve font creation efficiency.

Use :cpp:func:`lv_font_manager_add_src_static` to add a mapping between font names
and font resources to tell the font manager how to access the font resources.
Note that if the font resource description structure is not statically allocated
(for example, allocated from a local variable), use :cpp:func:`lv_font_manager_add_src` to add the resource.
This function will copy the contents of the structure itself.

Use :cpp:func:`lv_font_manager_remove_src` to remove the font resource mapping.

It should be noted that the ``src`` parameter must strictly correspond to ``class_p``.
``class_p`` will affect the way the font manager interprets src. If an incompatible parameter is passed, the program may fail.
For currently supported font classes, please refer to the example code.

.. code-block:: c

   static lv_font_manager_t * g_font_manager = NULL;

   void font_manager_init(void)
   {
      /* Create font manager, with 8 fonts recycling buffers */
      g_font_manager = lv_font_manager_create(8);

      /* Add font path mapping to font manager */
      lv_font_manager_add_src_static(g_font_manager, "Lato-Regular", "./lvgl/examples/libs/freetype/Lato-Regular.ttf", &lv_freetype_font_class);

      char path[] = "/path/to/myfont.ttf";
      lv_font_manager_add_src(g_font_manager, "MyFont", path, &lv_freetype_font_class);
   }

Create Font from Font Manager
-----------------------------

The parameters will be passed to the font creation function of the font backend,
such as :cpp:func:`lv_freetype_font_create` and :cpp:func:`lv_tiny_ttf_create_file`.
The font backend will select the supported parameters by itself and ignore the unsupported parameters.

The ``font_family`` parameter can be filled with the names of multiple fonts
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
                                                    LV_FREETYPE_FONT_STYLE_NORMAL,
                                                    LV_FONT_KERNING_NONE);

   /* Handle error case */
   if(g_font == NULL) {
      g_font = (lv_font_t *)LV_FONT_DEFAULT;
   }

   /* Create label with the font */
   lv_obj_t * label = lv_label_create(lv_screen_active());
   lv_obj_set_style_text_font(label, g_font, 0);
   lv_label_set_text(label, "Hello World!");

Delete Font
-----------

Use :cpp:func:`lv_font_manager_delete_font` to delete the font when it is no longer needed.
The font manager will mark the font resource as a recyclable font so that it has the
possibility of being more quickly created next time.

Note that you need to delete any Widgets that used the font first, and then
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
being referenced, the font manager will fail to be destroyed and the function will return false.



.. _font_manager_example:

Example
*******

.. include:: /examples/others/font_manager/index.rst



.. _font_manager_api:

API
***
