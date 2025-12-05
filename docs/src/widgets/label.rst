.. include:: /include/substitutions.txt
.. _lv_label:

================
Label (lv_label)
================


Overview
********

A Label is the Widget used to display text.


.. _lv_label_parts_and_styles:

Parts and Styles
****************

- :cpp:enumerator:`LV_PART_MAIN` Uses the :ref:`typical background <typical bg props>` and
  text properties. Padding values can be used to add space between
  the text and the edges of the Label's background.
- :cpp:enumerator:`LV_PART_SCROLLBAR` The scrollbar that is shown when the text is
  larger than the Widget's size.
- :cpp:enumerator:`LV_PART_SELECTED` Tells the style of the
  :ref:`selected text <lv_label_text_selection>`. Only ``text_color`` and ``bg_color`` style
  properties can be used.


.. _lv_label_usage:

Usage
*****

Set text
--------

You can set the text on a Label at runtime with
:cpp:expr:`lv_label_set_text(label, "New text")`. This will allocate a buffer
dynamically, and the provided string will be copied into that buffer.
Therefore, you don't need to keep the text you pass to
:cpp:func:`lv_label_set_text` in scope after that function returns.

With :cpp:expr:`lv_label_set_text_fmt(label, fmt, ...)` printf formatting
can be used to set the text.  Example:  :cpp:expr:`lv_label_set_text_fmt(label, "Value: %d", 15)`.

Labels are able to show text from a static character buffer as well.  To do so, use
:cpp:expr:`lv_label_set_text_static(label, "Text")`.  In this case, the text is not
stored in dynamic memory and the given buffer is used directly instead.  This means
that the contents of the character buffer *must* remain valid for the life of the
label or until another buffer is set via one of the above functions.

``const`` strings are safe to use with :cpp:func:`lv_label_set_text_static` since
they are stored in ROM memory, which is always accessible.

.. warning::

    Do not use ``const`` strings with :cpp:func:`lv_label_set_text_static` when the
    Label is being used in :cpp:enumerator:`LV_LABEL_LONG_MODE_DOTS` mode since the Label
    will attempt to do an in-place edit of the string.  This will cause an MCU
    exception by attempting to modify program memory (ROM).

.. _label_rapidly_updating_text:

.. caution::

    If your Label is updated with new strings rapidly (e.g. > 30X / second, such as
    RPM in a dashboard, or an ADC value), and the length of those strings changes
    frequently, it is advisable to:

    - allocate a static string buffer large enough contain the largest possible string,
    - update that buffer with the new strings only when they will make a visible
      difference for the end user, and
    - update the Label with :cpp:expr:`lv_label_set_text_static(label, buffer)` using that buffer.

    Reason:  if you use :cpp:expr:`lv_label_set_text(label, new_text)`, a memory
    realloc() will be forced every time the length of the string changes.  That
    MCU overhead can be avoided by doing the above.

Set translation tag
-------------------

When using LVGL's translation module, you can bind a translation tag to a label directly with :cpp:expr:`lv_label_set_translation_tag(label, tag)`.
After this function is called, future changes to the language will automatically update the label's text to display the corresponding translation
for that tag in the new language.

.. _lv_label_newline:

Newline
-------

Newline characters are handled automatically by the Label Widget. You
can use ``\n`` to make a line break. For example:
``"line1\nline2\n\nline4"``

.. _lv_label_long_modes:

Long modes
----------

By default, the width and height of the Label are set to
:c:macro:`LV_SIZE_CONTENT`. Thus, the size of the Label is automatically expanded
to the text size + padding + border width. Otherwise, if the width or height are
explicitly set (using e.g.\ :cpp:func:`lv_obj_set_width` or a layout), the lines
wider than the Label's width can be manipulated according to several
long mode policies. Similarly, the policies can be applied if the height
of the text is greater than the height of the Label.

- :cpp:enumerator:`LV_LABEL_LONG_MODE_WRAP` Wrap lines that are too long. If the height is :c:macro:`LV_SIZE_CONTENT` the Label's
  height will be expanded, otherwise the text will be clipped. (Default)
- :cpp:enumerator:`LV_LABEL_LONG_MODE_DOTS` Replaces the last 3 characters from bottom right corner of the Label with dots (``.``)
- :cpp:enumerator:`LV_LABEL_LONG_MODE_SCROLL` If the text is wider than the label, scroll it horizontally back and forth. If it's
  higher, scroll vertically. Only one direction is scrolled and horizontal scrolling has higher precedence.
- :cpp:enumerator:`LV_LABEL_LONG_MODE_SCROLL_CIRCULAR` If the text is wider than the Label, scroll it horizontally continuously. If it's
  higher, scroll vertically. Only one direction is scrolled and horizontal scrolling has higher precedence.
- :cpp:enumerator:`LV_LABEL_LONG_MODE_CLIP` Simply clip the parts of the text outside the Label.

You can specify the long mode with :cpp:expr:`lv_label_set_long_mode(label, LV_LABEL_LONG_...)`

Note that :cpp:enumerator:`LV_LABEL_LONG_MODE_DOTS` manipulates the text buffer in-place in
order to add/remove the dots. When :cpp:func:`lv_label_set_text` or
:cpp:func:`lv_label_set_array_text` are used, a separate buffer is allocated and
this implementation detail is unnoticed. This is not the case with
:cpp:func:`lv_label_set_text_static`. The buffer you pass to
:cpp:func:`lv_label_set_text_static` must be writable if you plan to use
:cpp:enumerator:`LV_LABEL_LONG_MODE_DOTS`.

.. _lv_label_text_recolor:

Text recolor
------------

In the text, you can use commands to recolor parts of the text.
For example: ``Write a #ff0000 red# word``. This feature can be enabled
individually for each label by :cpp:expr:`lv_label_set_recolor(label, en)`
function. In the context of word-wrapped text, any Recoloring started on a
line will be terminated at the end of the line where the line is wrapped if it
was not already terminated by an ending ``#`` in the text.

.. _lv_label_text_selection:

Text selection
--------------

If enabled by :c:macro:`LV_LABEL_TEXT_SELECTION` part of the text can be
selected. It's similar to when you use your mouse on a PC to select
text. The whole mechanism (click and select the text as you drag your
finger/mouse) is implemented in :ref:`lv_textarea` and
the Label Widget only allows programmatic text selection with
:cpp:expr:`lv_label_get_text_selection_start(label, start_char_index)` and
:cpp:expr:`lv_label_get_text_selection_end(label, end_char_index)`.

.. _lv_label_text_alignment:

Text alignment
--------------

To horizontally align the lines of a Label the `text_align` style property can be used with
:cpp:func:`lv_obj_set_style_text_align` or :cpp:func:`lv_style_set_text_align`,
passing one of the ``LV_TEXT_ALIGN_...`` enumeration values.
Note that this has a visible effect only if:

- the Label Widget's width is larger than the width of the longest line of text, and
- the text has multiple lines with different line lengths.


.. _lv_label_very_long_texts:

Very long text
--------------

LVGL can efficiently handle very long (e.g. > 40k characters) Labels by
saving some extra data (~12 bytes) to speed up drawing. To enable this
feature, set ``LV_LABEL_LONG_TXT_HINT`` to ``1`` in ``lv_conf.h``.

.. _lv_label_custom_scrolling_animations:

Custom scrolling animations
---------------------------

Some aspects of the scrolling animations in long modes
:cpp:enumerator:`LV_LABEL_LONG_SCROLL` and :cpp:enumerator:`LV_LABEL_LONG_SCROLL_CIRCULAR` can be
customized by setting the Label's animation style property, using
:cpp:func:`lv_style_set_anim`.
It will be treated as a template which will be used to create the scroll animations.

.. _lv_label_symbols:

Symbols
-------

The Labels can display symbols alongside letters (or on their own). Read
the :ref:`font` section to learn more about symbols.



.. _lv_label_data_binding:

Data binding
------------

To get familiar with observers, subjects, and data bindings in general visit the
:ref:`Observer <observer_how_to_use>` page.

This method of subscribing to a Subject affects a Label Widget's
``text``.  The Subject can be a STRING, POINTER, INTEGER, or FLOAT type.

When the subscription occurs, and each time the Subject's value is changed thereafter,
the Subject's value is used to update the Label's text as follows:

:string Subject:    Subject's string is used to directly update the Label's text.

:pointer Subject:   If NULL is passed as the ``format_string`` argument when
                    subscribing, the Subject's pointer value is assumed to point to a
                    NUL-terminated string and is used to directly update the Label's
                    text.  See :ref:`observer_format_string` for other options.

:integer Subject:   Subject's integer value is used with the ``format_string`` argument.
                    See :ref:`observer_format_string` for details.


:float Subject:     Subject's float value is used with the ``format_string`` argument.
                    Requires ``LV_USE_FLOAT``.
                    See :ref:`observer_format_string` for details.

Note that this is a one-way binding (Subject ===> Widget).

- :cpp:expr:`lv_label_bind_text(label, &subject, format_string)`

.. _observer_format_string:

The ``format_string`` Argument
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``format_string`` argument is optional and if provided, must contain exactly 1
printf-like format specifier and be one of the following:

:string or pointer Subject:  "%s" to format the new pointer value as a string or "%p"
                             to format the pointer as a pointer (typically the
                             pointer's address value is spelled out with 4, 8 or 16
                             hexadecimal characters depending on the platform).

:integer Subject:            "%d" format specifier (``"%" PRIdxx`` --- a
                             cross-platform equivalent where ``xx`` can be ``8``,
                             ``16``, ``32`` or ``64``, depending on the platform).

:float Subject:              "%f" format specifier, e.g. "%0.2f", to display two digits
                             after the decimal point.

If ``NULL`` is passed for the ``format_string`` argument:

:string or pointer Subject:  Updates expect the pointer to point to a NUL-terminated string.
:integer Subject:            The Label will simply display the number. Equivalent to "%d".
:float Subject:            The Label will display the value with "%0.1f" format string.

**Example:**  "%d |deg|\ C"

As usual with format strings, ``%%`` is used to get ``%``. For example ``%d%%``




.. _lv_label_events:

Events
******

No special events are sent by Label Widgets.  By default, Label Widgets are created
without the LV_OBJ_FLAG_CLICKABLE flag, but you can add it to make a Label Widget
emit LV_EVENT_CLICKED events if desired.

.. admonition::  Further Reading

    Learn more about :ref:`lv_obj_events` emitted by all Widgets.

    Learn more about :ref:`events`.



.. _lv_label_keys:

Keys
****

No *Keys* are processed by Label Widgets.

.. admonition::  Further Reading

    Learn more about :ref:`indev_keys`.



.. _lv_label_example:

Examples
********

.. include:: /examples/widgets/label/index.rst



.. _lv_label_api:

API
***
