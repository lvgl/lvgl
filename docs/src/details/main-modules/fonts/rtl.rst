

.. _bidi:


=====================
Bidirectional Support
=====================


Overview
********

Most languages use a Left-to-Right (LTR for short) writing direction,
however some languages (such as Hebrew, Persian or Arabic) use
Right-to-Left (RTL for short) direction.

LVGL not only supports RTL text but supports mixed (a.k.a.
bidirectional, BiDi) text rendering as well. Some examples:

.. image:: /_static/images/bidi.png

BiDi support is enabled by setting :c:macro:`LV_USE_BIDI` to a non-zero value in ``lv_conf.h``.

All text has a base direction (LTR or RTL) which determines some
rendering rules and the default alignment of the text (left or right).
However, in LVGL, the base direction is not only applied to labels. It's
a general property which can be set for every Widget. If not set then it
will be inherited from the parent. This means it's enough to set the
base direction of a screen and its child Widgets will inherit it.

The default base direction for screens can be set by
:c:macro:`LV_BIDI_BASE_DIR_DEF` in ``lv_conf.h`` and other Widgets inherit the
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
- ``lv_buttonmatrix``: Orders buttons from right to left
- ``lv_list``: Shows icons on the right
- ``lv_dropdown``: Aligns options to the right
- The text strings in ``lv_table``, ``lv_buttonmatrix``, ``lv_keyboard``, ``lv_tabview``,
  ``lv_dropdown``, ``lv_roller`` are "BiDi processed" to be displayed correctly

Arabic and Persian support
**************************

There are some special rules to display Arabic and Persian characters:
the *form* of a character depends on its position in the text. A
different form of the same letter needs to be used when it is isolated,
at start, middle or end positions. Besides these, some conjunction rules
should also be taken into account.

LVGL supports these rules if :c:macro:`LV_USE_ARABIC_PERSIAN_CHARS` is enabled
in ``lv_conf.h``.

However, there are some limitations:

- Only displaying text is supported (e.g. on labels), i.e. text inputs (e.g. Text
  Area) do not support this feature.
- Static text (i.e. const) is not processed. E.g. text set by :cpp:func:`lv_label_set_text`
  will be "Arabic processed" but :cpp:func:`lv_label_set_text_static` will not.
- Text get functions (e.g. :cpp:func:`lv_label_get_text`) will return the processed text.

