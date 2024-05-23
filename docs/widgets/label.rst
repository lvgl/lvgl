.. _lv_label:

================
Label (lv_label)
================

Overview
********

A label is the basic object type that is used to display text.

.. _lv_label_parts_and_styles:

Parts and Styles
****************

- :cpp:enumerator:`LV_PART_MAIN` Uses all the typical background properties and the
   text properties. The padding values can be used to add space between
   the text and the background.
- :cpp:enumerator:`LV_PART_SCROLLBAR` The scrollbar that is shown when the text is
   larger than the widget's size.
- :cpp:enumerator:`LV_PART_SELECTED` Tells the style of the 
  :ref:`selected text <lv_label_text_selection>`. Only ``text_color`` and ``bg_color`` style
   properties can be used.

.. _lv_label_usage:

Usage
*****

Set text
--------

You can set the text on a label at runtime with
:cpp:expr:`lv_label_set_text(label, "New text")`. This will allocate a buffer
dynamically, and the provided string will be copied into that buffer.
Therefore, you don't need to keep the text you pass to
:cpp:func:`lv_label_set_text` in scope after that function returns.

With :cpp:expr:`lv_label_set_text_fmt(label, "Value: %d", 15)` printf formatting
can be used to set the text.

Labels are able to show text from a static character buffer. To do so,
use :cpp:expr:`lv_label_set_text_static(label, "Text")`. In this case, the text
is not stored in the dynamic memory and the given buffer is used
directly instead. This means that the array can't be a local variable
which goes out of scope when the function exits. Constant strings are
safe to use with :cpp:func:`lv_label_set_text_static` (except when used with
:cpp:enumerator:`LV_LABEL_LONG_DOT`, as it modifies the buffer in-place), as they are
stored in ROM memory, which is always accessible.

.. _lv_label_newline:

Newline
-------

Newline characters are handled automatically by the label object. You
can use ``\n`` to make a line break. For example:
``"line1\nline2\n\nline4"``

.. _lv_label_long_modes:

Long modes
----------

By default, the width and height of the label is set to
:c:macro:`LV_SIZE_CONTENT`. Therefore, the size of the label is automatically
expanded to the text size. Otherwise, if the width or height are
explicitly set (using e.g.\ :cpp:func:`lv_obj_set_width` or a layout), the lines
wider than the label's width can be manipulated according to several
long mode policies. Similarly, the policies can be applied if the height
of the text is greater than the height of the label.

- :cpp:enumerator:`LV_LABEL_LONG_WRAP` Wrap too long lines. If the height is :c:macro:`LV_SIZE_CONTENT` the label's
  height will be expanded, otherwise the text will be clipped. (Default)
- :cpp:enumerator:`LV_LABEL_LONG_DOT` Replaces the last 3 characters from bottom right corner of the label with dots (``.``)
- :cpp:enumerator:`LV_LABEL_LONG_SCROLL` If the text is wider than the label scroll it horizontally back and forth. If it's
  higher, scroll vertically. Only one direction is scrolled and horizontal scrolling has higher precedence.
- :cpp:enumerator:`LV_LABEL_LONG_SCROLL_CIRCULAR` If the text is wider than the label scroll it horizontally continuously. If it's
  higher, scroll vertically. Only one direction is scrolled and horizontal scrolling has higher precedence.
- :cpp:enumerator:`LV_LABEL_LONG_CLIP` Simply clip the parts of the text outside the label.

You can specify the long mode with :cpp:expr:`lv_label_set_long_mode(label, LV_LABEL_LONG_...)`

Note that :cpp:enumerator:`LV_LABEL_LONG_DOT` manipulates the text buffer in-place in
order to add/remove the dots. When :cpp:func:`lv_label_set_text` or
:cpp:func:`lv_label_set_array_text` are used, a separate buffer is allocated and
this implementation detail is unnoticed. This is not the case with
:cpp:func:`lv_label_set_text_static`. The buffer you pass to
:cpp:func:`lv_label_set_text_static` must be writable if you plan to use
:cpp:enumerator:`LV_LABEL_LONG_DOT`.

.. _lv_label_text_selection:

Text selection
--------------

If enabled by :c:macro:`LV_LABEL_TEXT_SELECTION` part of the text can be
selected. It's similar to when you use your mouse on a PC to select a
text. The whole mechanism (click and select the text as you drag your
finger/mouse) is implemented in :ref:`Text area <lv_textarea>` and
the Label widget only allows manual text selection with
:cpp:expr:`lv_label_get_text_selection_start(label, start_char_index)` and
:cpp:expr:`lv_label_get_text_selection_start(label, end_char_index)`.

.. _lv_label_text_alignment:

Text alignment
--------------

To horizontally align the lines of a label the `text_align` style property can be used with
:cpp:func:`lv_obj_set_style_text_align` or :cpp:func:`lv_style_set_text_align`
Note that it has a visible effect only if

- the label widget's width is larger than the width of the longest line of the text
- the text has multiple lines with non equal line length

.. _lv_label_very_long_texts:

Very long texts
---------------

LVGL can efficiently handle very long (e.g. > 40k characters) labels by
saving some extra data (~12 bytes) to speed up drawing. To enable this
feature, set ``LV_LABEL_LONG_TXT_HINT   1`` in ``lv_conf.h``.

.. _lv_label_custom_scrolling_animations:

Custom scrolling animations
---------------------------

Some aspects of the scrolling animations in long modes
:cpp:enumerator:`LV_LABEL_LONG_SCROLL` and :cpp:enumerator:`LV_LABEL_LONG_SCROLL_CIRCULAR` can be
customized by setting the animation property of a style, using
:cpp:func:`lv_style_set_anim`.
It will be treated as a template which will be used to create the scroll animatins.

.. _lv_label_symbols:

Symbols
-------

The labels can display symbols alongside letters (or on their own). Read
the :ref:`fonts` section to learn more about the symbols.

.. _lv_label_events:

Events
******

No special events are sent by the Label.

See the events of the :ref:`Base object <lv_obj>` too.

Learn more about :ref:`events`.

.. _lv_label_keys:

Keys
****

No *Keys* are processed by the object type.

Learn more about :ref:`indev_keys`.

.. _lv_label_example:

Example
*******

.. include:: ../examples/widgets/label/index.rst

.. _lv_label_api:

API
***
