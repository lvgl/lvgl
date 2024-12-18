===========
Typesetting
===========

Although LVGL can decode and display any Unicode characters
(assuming the font supports them), LVGL cannot correctly render
all complex languages.

The standard Latin-based languages (e.g., English, Spanish, German)
and East Asian languages such as Chinese, Japanese, and Korean (CJK)
are relatively straightforward, as their characters are simply
written from left to right.

Languages like Arabic, Persian, and Hebrew, which use Right-to-Left
(RTL) or mixed writing directions, are also supported in LVGL.
Learn more :ref:`here <bidi>`.


.. |Aacute| unicode:: U+000C1 .. LATIN CAPITAL LETTER A WITH ACUTE
.. |eacute| unicode:: U+000E9 .. LATIN SMALL LETTER E WITH ACUTE
.. |otilde| unicode:: U+000F5 .. LATIN SMALL LETTER O WITH TILDE
.. |Utilde| unicode:: U+00168 .. LATIN CAPITAL LETTER U WITH TILDE
.. |uuml|   unicode:: U+000FC .. LATIN SMALL LETTER U WITH DIAERESIS

For characters such as '|eacute|', '|uuml|', '|otilde|', '|Aacute|', and '|Utilde|', it is recommended
to use the single Unicode format (NFC) rather than decomposing them into
a base letter and diacritics (e.g. ``u + ¨``).

Complex languages where subsequent characters combine into a single glyph
and where the resulting glyph has no individual Unicode representation
(e.g., Devanagari), have limited support in LVGL.

Kerning
-------

Fonts may provide kerning information to adjust the spacing between specific
characters.

- The online converter generates kerning tables.
- The offline converter generates kerning tables unless ``--no-kerning`` is
  specified.
- FreeType integration does not currently support kerning.
- The Tiny TTF font engine supports GPOS and Kern tables.

To configure kerning at runtime, use :cpp:func:`lv_font_set_kerning`.

.. _bidi:

Bidirectional support
---------------------

Most languages use a Left-to-Right (LTR for short) writing direction,
however some languages (such as Hebrew, Persian or Arabic) use
Right-to-Left (RTL for short) direction.

LVGL not only supports RTL text but supports mixed (a.k.a.
bidirectional, BiDi) text rendering as well. Some examples:

.. image:: /misc/bidi.png

BiDi support is enabled by :c:macro:`LV_USE_BIDI` in *lv_conf.h*

All text has a base direction (LTR or RTL) which determines some
rendering rules and the default alignment of the text (Left or Right).
However, in LVGL, the base direction is not only applied to labels. It's
a general property which can be set for every Widget. If not set then it
will be inherited from the parent. This means it's enough to set the
base direction of a screen and every Widget will inherit it.

The default base direction for screens can be set by
:c:macro:`LV_BIDI_BASE_DIR_DEF` in *lv_conf.h* and other Widgets inherit the
base direction from their parent.

To set a Widget's base direction use :cpp:expr:`lv_obj_set_style_base_dir(widget, base_dir, selector)`.
The possible base directions are:

- :cpp:enumerator:`LV_BASE_DIR_LTR`: Left to Right base direction
- :cpp:enumerator:`LV_BASE_DIR_RTL`: Right to Left base direction
- :cpp:enumerator:`LV_BASE_DIR_AUTO`: Auto detect base direction

This list summarizes the effect of RTL base direction on Widgets:

- Create Widgets by default on the right
- ``lv_tabview``: Displays tabs from right to left
- ``lv_checkbox``: Shows the box on the right
- ``lv_buttonmatrix``: Shows buttons from right to left
- ``lv_list``: Shows icons on the right
- ``lv_dropdown``: Aligns options to the right
- The text strings in ``lv_table``, ``lv_buttonmatrix``, ``lv_keyboard``, ``lv_tabview``, ``lv_dropdown``, ``lv_roller`` are "BiDi processed" to be displayed correctly

Arabic and Persian support
--------------------------

There are some special rules to display Arabic and Persian characters:
the *form* of a character depends on its position in the text. A
different form of the same letter needs to be used when it is isolated,
at start, middle or end positions. Besides these, some conjunction rules
should also be taken into account.

LVGL supports these rules if :c:macro:`LV_USE_ARABIC_PERSIAN_CHARS` is enabled.

However, there are some limitations:

- Only displaying text is supported (e.g. on labels), text inputs (e.g. text area) don't support this feature.
- Static text (i.e. const) is not processed. E.g. text set by :cpp:func:`lv_label_set_text` will be "Arabic processed" but :cpp:func:`lv_label_set_text_static` won't.
- Text get functions (e.g. :cpp:func:`lv_label_get_text`) will return the processed text.
