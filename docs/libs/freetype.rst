.. _freetype:

================
FreeType support
================

Interface to FreeType library to generate font bitmaps run time.

Detailed introduction: https://www.freetype.org

Add FreeType to your project
----------------------------

First, Download FreeType from `here <https://sourceforge.net/projects/freetype/files/>`__.

There are two ways to use FreeType:

For UNIX
~~~~~~~~

For UNIX systems, it is recommended to use the way of compiling and installing libraries.

- Enter the FreeType source code directory
- ``make``
- ``sudo make install``
- Add include path: ``/usr/include/freetype2`` (for GCC: ``-I/usr/include/freetype2 -L/usr/local/lib``)
- Link library: ``freetype`` (for GCC: ``-L/usr/local/lib -lfreetype``)

For Embedded Devices
~~~~~~~~~~~~~~~~~~~~

For embedded devices, it is more recommended to use the FreeType
configuration file provided by LVGL, which only includes the most
commonly used functions, which is very meaningful for saving limited
FLASH space.

-  Copy the FreeType source code to your project directory.
-  Refer to the following ``Makefile`` for configuration:

.. code:: make

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
   FT_CSRCS += freetype/src/cache/ftcache.c
   FT_CSRCS += freetype/src/gzip/ftgzip.c
   FT_CSRCS += freetype/src/sfnt/sfnt.c
   FT_CSRCS += freetype/src/smooth/smooth.c
   FT_CSRCS += freetype/src/truetype/truetype.c
   CSRCS += $(FT_CSRCS)

.. _freetype_usage:

Usage
-----

Enable :c:macro:`LV_USE_FREETYPE` in ``lv_conf.h``.

Cache configuration:

- :c:macro:`LV_FREETYPE_CACHE_FT_GLYPH_CNT` Maximum number of cached glyphs., etc.

By default, the FreeType extension doesn't use LVGL's file system. You
can simply pass the path to the font as usual on your operating system
or platform.

If you want FreeType to use lvgl's memory allocation and file system
interface, you can enable :c:macro:`LV_FREETYPE_USE_LVGL_PORT` in
``lv_conf.h``, convenient for unified management.

The font style supports *Italic* and **Bold** fonts processed by
software, and can be set with reference to the following values:

- :cpp:enumerator:`LV_FREETYPE_FONT_STYLE_NORMAL`: Default style.
- :cpp:enumerator:`LV_FREETYPE_FONT_STYLE_ITALIC`: Italic style.
- :cpp:enumerator:`LV_FREETYPE_FONT_STYLE_BOLD`:  Bold style.

They can be combined.eg:
:cpp:expr:`LV_FREETYPE_FONT_STYLE_BOLD | LV_FREETYPE_FONT_STYLE_ITALIC`.

Use the :cpp:func:`lv_freetype_font_create` function to create a font. To
delete a font, use :cpp:func:`lv_freetype_font_delete`. For more detailed usage,
please refer to example code.

.. _freetype_example:

Example
-------

.. include:: ../examples/libs/freetype/index.rst

Learn more
----------

-  FreeType`tutorial <https://www.freetype.org/freetype2/docs/tutorial/step1.html>`__
-  LVGL's :ref:`add_font`

.. _freetype_api:

API
---

:ref:`ftoption`

:ref:`ftmodule`

