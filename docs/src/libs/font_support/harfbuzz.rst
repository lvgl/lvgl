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



Installing HarfBuzz
********************

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

For Embedded Devices
--------------------

Cross-compile HarfBuzz from source (or use a package manager such as
``vcpkg`` or ``buildroot``).  The minimum required HarfBuzz build options are
the FreeType integration (``-DHB_HAVE_FREETYPE=ON``) and the built-in
Unicode functions (``-DHB_HAVE_GLIB=OFF`` if GLib is not available).

Add the HarfBuzz headers to your include path and link against
``libharfbuzz``.



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

