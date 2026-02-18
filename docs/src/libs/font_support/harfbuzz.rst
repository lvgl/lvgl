.. _harfbuzz:

===========================================
Multi-Language Text Rendering (HarfBuzz)
===========================================

Many of the world's writing systems --- including Hindi (Devanagari), Bengali,
Tamil, Telugu, Thai, and others --- cannot be rendered correctly by processing
one Unicode character at a time.  These scripts require **text shaping**:
consonants fuse into conjuncts, vowel marks reorder around their base
characters, and glyphs are vertically stacked or repositioned according to
context.

LVGL's HarfBuzz integration adds automatic text shaping for complex scripts.
It uses `HarfBuzz <https://harfbuzz.github.io/>`__, the industry-standard
open-source text shaping engine, together with FreeType for glyph rasterization.
Once enabled, labels and other text widgets shape complex-script text
transparently --- no application-level changes are needed.

.. note::

   HarfBuzz support requires :ref:`FreeType <freetype>`.  Only fonts loaded
   through :cpp:func:`lv_freetype_font_create` benefit from shaping.
   LVGL's built-in bitmap fonts are not affected.



Adding HarfBuzz to Your Project
*******************************

HarfBuzz requires FreeType as a dependency.  If you haven't already added
FreeType, see :ref:`freetype` first.

For UNIX
--------

On Debian / Ubuntu:

.. code-block:: bash

   sudo apt install libharfbuzz-dev

On macOS (Homebrew):

.. code-block:: bash

   brew install harfbuzz

Then add the include and linker flags to your build:

.. code-block:: make

   CFLAGS  += $(shell pkg-config --cflags harfbuzz)
   LDFLAGS += $(shell pkg-config --libs   harfbuzz)


Using CMake
-----------

If your project uses CMake, use ``find_package`` or ``FetchContent`` to
pull in both FreeType and HarfBuzz.

**Using system-installed libraries (find_package):**

.. code-block:: cmake

   find_package(Freetype REQUIRED)
   find_package(harfbuzz REQUIRED)

   target_include_directories(my_app PRIVATE
       ${FREETYPE_INCLUDE_DIRS}
       ${HARFBUZZ_INCLUDE_DIRS}
   )
   target_link_libraries(my_app PRIVATE
       lvgl
       ${FREETYPE_LIBRARIES}
       harfbuzz
   )

**Building from source (FetchContent):**

When cross-compiling or when system packages are not available, you can
build both libraries from source.  Because FreeType and HarfBuzz have a
circular dependency, the simplest approach is to build FreeType first
without HarfBuzz support, then build HarfBuzz against it:

.. code-block:: cmake

   include(FetchContent)

   # Step 1 - FreeType (without HarfBuzz)
   FetchContent_Declare(freetype
       GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
       GIT_TAG        VER-2-13-3
   )
   set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "" FORCE)
   set(FT_DISABLE_BZIP2    ON CACHE BOOL "" FORCE)
   set(FT_DISABLE_PNG      ON CACHE BOOL "" FORCE)
   set(FT_DISABLE_BROTLI   ON CACHE BOOL "" FORCE)
   FetchContent_MakeAvailable(freetype)

   # Step 2 - HarfBuzz (with FreeType, without optional deps)
   FetchContent_Declare(harfbuzz
       GIT_REPOSITORY https://github.com/harfbuzz/harfbuzz.git
       GIT_TAG        9.0.0
   )
   set(HB_HAVE_FREETYPE ON  CACHE BOOL "" FORCE)
   set(HB_HAVE_GLIB     OFF CACHE BOOL "" FORCE)
   set(HB_HAVE_ICU      OFF CACHE BOOL "" FORCE)
   set(HB_HAVE_GOBJECT  OFF CACHE BOOL "" FORCE)
   set(HB_BUILD_SUBSET  OFF CACHE BOOL "" FORCE)
   FetchContent_MakeAvailable(harfbuzz)

   target_link_libraries(my_app PRIVATE lvgl freetype harfbuzz)


Using Makefile (Amalgamated Source)
-----------------------------------

For projects that do not use CMake (e.g. IDE-managed builds for MCUs),
HarfBuzz provides an **amalgamated source file** that bundles the entire
library into a single compilation unit.  This eliminates the need for a
build system.

-  Download the HarfBuzz source from its `official repository
   <https://github.com/harfbuzz/harfbuzz/releases>`__.
-  Copy the FreeType source code to your project directory (see
   :ref:`freetype` for details).
-  Refer to the following ``Makefile`` for configuration:

.. code-block:: make

   # FreeType (see FreeType documentation for full source list)
   CFLAGS  += -DFT2_BUILD_LIBRARY
   CFLAGS  += -DFT_CONFIG_MODULES_H=<lvgl/src/libs/freetype/ftmodule.h>
   CFLAGS  += -DFT_CONFIG_OPTIONS_H=<lvgl/src/libs/freetype/ftoption.h>
   CFLAGS  += -Ifreetype/include

   # HarfBuzz amalgamated source (single file, no build system needed)
   CXXFLAGS += -DHB_TINY
   CXXFLAGS += -Iharfbuzz/src
   CXXSRCS  += harfbuzz/src/harfbuzz.cc

``harfbuzz.cc`` includes all HarfBuzz source files internally.  No other
HarfBuzz ``.c`` or ``.cc`` files need to be added to your build.

.. note::

   HarfBuzz is written in C++ (C++11 or later).  The amalgamated source
   must be compiled with a C++ compiler.



.. _harfbuzz_usage:

Usage
*****

1. Enable FreeType and HarfBuzz in ``lv_conf.h``:

   .. code-block:: c

      #define LV_USE_FREETYPE  1
      #define LV_USE_HARFBUZZ  1

2. Create a FreeType font from a ``.ttf`` / ``.otf`` file that covers the
   target script (e.g. `Noto Sans Devanagari
   <https://fonts.google.com/noto/specimen/Noto+Sans+Devanagari>`__):

   .. code-block:: c

      lv_font_t * font_hindi = lv_freetype_font_create(
          "NotoSansDevanagari-Regular.ttf",
          LV_FREETYPE_FONT_RENDER_MODE_BITMAP, 32,
          LV_FREETYPE_FONT_STYLE_NORMAL);

3. Use the font with any text widget --- shaping happens automatically:

   .. code-block:: c

      lv_obj_t * label = lv_label_create(lv_screen_active());
      lv_obj_set_style_text_font(label, font_hindi, 0);
      lv_label_set_text(label, "नमस्ते दुनिया");   /* Hello World */

Font Fallback
-------------

To display characters not covered by the primary font (e.g. Latin digits
alongside Devanagari), set a fallback font:

.. code-block:: c

   font_hindi->fallback = &lv_font_montserrat_14;

When HarfBuzz encounters a glyph that the primary font cannot render, it
falls back to this font automatically.



Font Subsetting
***************

Full font files can be very large (500 KB+).  For production builds it is
recommended to **subset** fonts to include only the Unicode ranges your
application needs.  Use ``pyftsubset`` (from the ``fonttools`` Python
package) or HarfBuzz's own ``hb-subset`` tool.

.. code-block:: bash

   pip install fonttools

   # Subset to Latin + Devanagari only
   pyftsubset NotoSansDevanagari-Regular.ttf \
       --unicodes="U+0000-007F,U+0900-097F" \
       --layout-features='*' \
       --output-file=NotoSansDevanagari-Subset.ttf

.. warning::

   Always keep ``--layout-features='*'`` (or at minimum
   ``ccmp,rlig,liga,calt,mark,mkmk,kern,locl``) when subsetting fonts
   intended for HarfBuzz.  Removing OpenType layout features will break
   text shaping for complex scripts.

Typical size reductions:

.. list-table::
   :header-rows: 1
   :widths: 40 30 30

   * - Subset
     - Typical input
     - Typical output
   * - Full font (no subsetting)
     - 500--900 KB
     - ---
   * - Single script (e.g. Devanagari + Basic Latin)
     - 500--900 KB
     - 30--100 KB
   * - Latin only
     - 500--900 KB
     - 30--50 KB



Build Size Optimization
***********************

HarfBuzz provides compile-time defines to reduce binary size.  The most
relevant for LVGL projects:

.. list-table::
   :header-rows: 1
   :widths: 25 75

   * - Define
     - Effect
   * - ``HB_TINY``
     - Maximum size reduction.  Disables legacy shapers (AAT, Graphite),
       thread safety, debug code, and fallback shaper.  OpenType shaping
       (used by LVGL) is fully retained.
   * - ``HB_LEAN``
     - Disables non-shaping APIs and CFF glyph drawing.
   * - ``HB_NO_MT``
     - Disables thread safety (mutexes, atomics).  Already included in
       ``HB_TINY``.

For all builds, the following compiler and linker flags are recommended:

.. code-block:: make

   CXXFLAGS += -Os                           # optimize for size
   CFLAGS   += -ffunction-sections -fdata-sections
   CXXFLAGS += -ffunction-sections -fdata-sections
   LDFLAGS  += -Wl,--gc-sections             # strip unused code

Enabling Link-Time Optimization (``-flto``) can provide an additional
~20% size reduction.



Supported Scripts
*****************

HarfBuzz supports all scripts that have OpenType shaping rules.  The table
below lists the most widely used complex scripts and highlights what makes
each one difficult to render without a shaping engine.

.. list-table::
   :header-rows: 1
   :widths: 18 15 67

   * - Script
     - Speakers
     - Key shaping features
   * - Devanagari (Hindi, Marathi, Sanskrit, Nepali)
     - 615 M+
     - Conjuncts (consonant clusters), half-forms, Reph reordering,
       matra (vowel sign) reordering
   * - Bengali (Bangla)
     - 265 M+
     - Conjuncts with shared strokes, left-matra reordering (vowel sign
       visually precedes the consonant it logically follows), Reph
   * - Telugu
     - 93 M+
     - Below-base vertical consonant stacking, pre-base matra reordering,
       one of the largest conjunct inventories of any script
   * - Thai
     - 61 M+
     - Above-base and below-base mark stacking, contextual vertical
       repositioning of tone marks and vowel signs (no conjuncts)
   * - Tamil
     - 81 M+
     - Mandatory ligature substitution --- combined forms have no Unicode
       codepoint and can only be produced via the font's GSUB table
   * - Kannada
     - 55 M+
     - Below-base forms, Reph, matra reordering (South Indic model)
   * - Malayalam
     - 38 M+
     - Extremely large conjunct set, Chillu (dead-consonant) forms
   * - Gujarati
     - 57 M+
     - Conjuncts, half-forms, Reph (North Indic model, similar to Devanagari)
   * - Myanmar (Burmese)
     - 43 M+
     - Kinzi reordering, consonant stacking, medial forms; uses a dedicated
       HarfBuzz shaper separate from the Indic shaper
   * - Khmer
     - 18 M+
     - Subscript consonant (Coeng) stacking, pre-base Ro reordering

Any script with proper OpenType tables in the font file will be shaped
correctly.  The scripts listed above are only the most common examples.



Known Limitations
*****************

- **Mixed-script text in a single label**: When a label contains text from
  multiple scripts (e.g. ``"Hello नमस्ते World"``), the shaped and
  unshaped portions may not align perfectly.  For best results, use
  separate labels for text segments in different scripts, or use a single
  font that covers all required scripts.

- **FreeType fonts only**: Only fonts created with
  :cpp:func:`lv_freetype_font_create` are shaped by HarfBuzz.  LVGL's
  built-in bitmap fonts and Tiny TTF fonts are not affected.

- **No automatic script/language detection per-run**: HarfBuzz uses
  ``hb_buffer_guess_segment_properties()`` to detect the script.  For
  text that mixes multiple complex scripts in a single string, detection
  may not always choose the correct script for every segment.



.. admonition::  Further Reading

   - `HarfBuzz documentation <https://harfbuzz.github.io/>`__
   - `HarfBuzz build configuration (CONFIG.md) <https://github.com/harfbuzz/harfbuzz/blob/main/CONFIG.md>`__
   - :ref:`FreeType Font Engine <freetype>`
   - LVGL's :ref:`add_font`



.. _harfbuzz_example:

Examples
********

.. include:: ../../examples/libs/harfbuzz/index.rst



.. _harfbuzz_api:

API
***

.. API startswith:  lv_hb_
