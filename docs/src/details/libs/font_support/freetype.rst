.. _freetype:

====================
FreeType Font Engine
====================

**FreeType** is a freely available software library to render fonts.

The LVGL FreeType extension is an interface to the FreeType library, enabling you to
generate font bitmaps at run time from most vector- and bitmap-font file formats.

For a detailed introduction, see:  https://freetype.org/ .



Adding FreeType to Your Project
*******************************

First, Download FreeType from the ``freetype2`` folder (and optionally ``freetype-docs``
and ``freetype-demos``) from its `official repository
<https://sourceforge.net/projects/freetype/files/>`__.  (The latest version is
recommended.)

If you haven't already done so, now is a good time to get familiar with setting up
and configuring this library.  The above website is a good place to start, as is the
``README`` file in the top directory of the version you downloaded.

There are two ways to use FreeType:

For UNIX
--------

For UNIX-like systems, the following is recommended to compile and install FreeType libraries.

- Enter the FreeType source code directory
- ``make``
- ``sudo make install``
- Add include path: ``/usr/include/freetype2`` (for GCC: ``-I/usr/include/freetype2 -L/usr/local/lib``)
- Link against library: ``freetype`` (for GCC: ``-L/usr/local/lib -lfreetype``)

For Embedded Devices
--------------------

For embedded devices, it is recommended to use the FreeType
configuration files provided by LVGL:

- lvgl/src/libs/freetype/ftmodule.h
- lvgl/src/libs/freetype/ftoption.h

which only include the most commonly used modules and options, which is important to
save limited FLASH space.

-  Copy the FreeType source code to your project directory.
-  Refer to the following ``Makefile`` for configuration:

.. code-block:: make

   # FreeType custom configuration header file
   CFLAGS += -DFT2_BUILD_LIBRARY
   CFLAGS += -DFT_CONFIG_MODULES_H=<lvgl/src/libs/freetype/ftmodule.h>
   CFLAGS += -DFT_CONFIG_OPTIONS_H=<lvgl/src/libs/freetype/ftoption.h>

   # FreeType include path
   CFLAGS += -Ifreetype/include

   # FreeType C source file
   FT_CSRCS += freetype/src/base/ftbase.c
   FT_CSRCS += freetype/src/base/ftbitmap.c
   FT_CSRCS += freetype/src/base/ftdebug.c
   FT_CSRCS += freetype/src/base/ftglyph.c
   FT_CSRCS += freetype/src/base/ftinit.c
   FT_CSRCS += freetype/src/base/ftstroke.c
   FT_CSRCS += freetype/src/cache/ftcache.c
   FT_CSRCS += freetype/src/gzip/ftgzip.c
   FT_CSRCS += freetype/src/sfnt/sfnt.c
   FT_CSRCS += freetype/src/smooth/smooth.c
   FT_CSRCS += freetype/src/truetype/truetype.c
   CSRCS += $(FT_CSRCS)



.. _freetype_usage:

Usage
*****

Set :c:macro:`LV_USE_FREETYPE` in ``lv_conf.h`` to ``1``.

Cache configuration:

- :c:macro:`LV_FREETYPE_CACHE_FT_GLYPH_CNT` Maximum number of cached glyphs., etc.

By default, the FreeType extension doesn't use LVGL's file system. You
can simply pass the path to the font as usual on your operating system
or platform.

If you want FreeType to use LVGL's memory allocation and file system
interface, you can enable :c:macro:`LV_FREETYPE_USE_LVGL_PORT` in
``lv_conf.h``, convenient for unified management.  If you do this, you will need
to exclude the configured FreeType library's ``ftsystem.c`` file from being compiled,
since LVGL's ``lv_ftsystem.c`` has custom versions of the functions defined therein.

The font style supports *Italic* and **Bold** fonts processed by
software, and can be set by using following values where style values are required:

- :cpp:enumerator:`LV_FREETYPE_FONT_STYLE_NORMAL`: Default style.
- :cpp:enumerator:`LV_FREETYPE_FONT_STYLE_ITALIC`: Italic style.
- :cpp:enumerator:`LV_FREETYPE_FONT_STYLE_BOLD`:  Bold style.

These values can be combined, e.g.
:cpp:expr:`LV_FREETYPE_FONT_STYLE_BOLD | LV_FREETYPE_FONT_STYLE_ITALIC`.

The FreeType extension also supports colored bitmap glyphs such as emojis. Note
that only bitmaps are supported at this time. Colored vector graphics cannot be
rendered. An example on how to draw a colored bitmap glyph is shown below.

Use the :cpp:func:`lv_freetype_font_create` function to create a font. To
delete a font, use :cpp:func:`lv_freetype_font_delete`. For more detailed usage,
please refer to the example code below.



.. admonition::  Further Reading

    - `FreeType tutorial <https://www.freetype.org/freetype2/docs/tutorial/step1.html>`__
    - LVGL's :ref:`add_font`

Rendering vector fonts is supported with VGLite or ThorVG,
when using vector fonts with ThorVG, it is possible to set a letter outline of a different color.

This is achieved by setting the style attributes with the
:cpp:func:`lv_style_set_text_outline_width` and :cpp:func:`lv_style_set_text_outline_color` functions

You will have to account for the increased width and height of letter due to the added letter outline,
to avoid letters overlapping space them out using :cpp:func:`lv_style_set_text_letter_space`

To use vector fonts with ThorVG, you will have to enable ``LV_USE_VECTOR_GRAPHICS`` in ``lv_conf.h``

.. note::

   This feature is currently experimental, there are clipping issues especially when using large font sizes.

See the :cpp:func:`lv_example_freetype_2_vector_font` function for a usage example


.. _freetype_example:

Examples
********

.. include:: ../../examples/libs/freetype/index.rst



.. _freetype_api:

API
***

.. API startswith:  lv_freetype_

