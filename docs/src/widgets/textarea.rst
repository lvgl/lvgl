.. _lv_textarea:

=======================
Text Area (lv_textarea)
=======================


Overview
********

The Text Area is a :ref:`base_widget` with a :ref:`lv_label` and a cursor on
it.  Text or characters can be added to it.  Long lines are wrapped and when the
text becomes long enough, the Text Area can be scrolled.

One-line mode and password modes are supported.



.. _lv_textarea_parts_and_styles:

Parts and Styles
****************

-  :cpp:enumerator:`LV_PART_MAIN` The background of the Text Area; uses the
   :ref:`typical background style properties <typical bg props>` and the text related
   style properties including ``text_align`` to align the text to the left,
   right or center.
-  :cpp:enumerator:`LV_PART_SCROLLBAR` The scrollbar that is shown when the text is
   longer than its height.
-  :cpp:enumerator:`LV_PART_SELECTED` Determines the style of the :ref:`selected
   text <lv_label_text_selection>`. Only ``text_color`` and
   ``bg_color`` style properties can be used. ``bg_color`` should be set
   directly on the label of the Text Area.
-  :cpp:enumerator:`LV_PART_CURSOR` Marks the position where the characters are
   inserted. The cursor's area is always the bounding box of the current
   character. A block cursor can be created by adding a background color
   and background opacity to :cpp:enumerator:`LV_PART_CURSOR`\ 's style. To create
   a "bar" cursor leave the cursor transparent and set a left border. The
   ``anim_time`` style property sets the cursor's blink time.
-  :cpp:enumerator:`LV_PART_TEXTAREA_PLACEHOLDER` Unique to Text Area; allows styling
   the :ref:`placeholder text <textarea_placeholder_text>`.



.. _lv_textarea_usage:

Usage
*****

Adding text
-----------

You can insert text or characters to the current cursor's position with:

-  :cpp:expr:`lv_textarea_add_char(textarea, 'c')`
-  :cpp:expr:`lv_textarea_add_text(textarea, "insert this text")`

To add wide characters like ``'á'``, ``'ß'`` or CJK characters, use
:cpp:expr:`lv_textarea_add_text(textarea, "á")`.

:cpp:expr:`lv_textarea_set_text(textarea, "New text")` replaces all existing text
with "New text".

.. _textarea_placeholder_text:

Placeholder text
----------------

Placeholder text is text that is displayed when the Text Area is empty.  This can be
a handy way to provide the end user with a hint about what to type there.

Specify placeholder text using
:cpp:expr:`lv_textarea_set_placeholder_text(textarea, "Placeholder text")`.

Delete character
----------------

To delete the character to the left of the current cursor position, use
:cpp:expr:`lv_textarea_delete_char(textarea)`.

To delete to the right, use :cpp:expr:`lv_textarea_delete_char_forward(textarea)`

Moving the cursor
-----------------

The cursor position can be modified programmatically using
:cpp:expr:`lv_textarea_set_cursor_pos(textarea, cursor_pos)` where ``cursor_pos`` is
the zero-based index of the character the cursor should be placed in front of.
:cpp:enumerator:`LV_TEXTAREA_CURSOR_LAST` can be passed to mean "after the
last character"

You can move the cursor one character-position (or line) at a time with

- :cpp:expr:`lv_textarea_cursor_right(textarea)`
- :cpp:expr:`lv_textarea_cursor_left(textarea)`
- :cpp:expr:`lv_textarea_cursor_up(textarea)`
- :cpp:expr:`lv_textarea_cursor_down(textarea)`

If :cpp:expr:`lv_textarea_set_cursor_click_pos(textarea, true)` is applied, the
cursor will jump to the position where the Text Area was clicked.

Hiding the cursor
-----------------

The cursor is normally always visible. It can be a good idea to style it
to be visible only in :cpp:enumerator:`LV_STATE_FOCUSED` state. See :ref:`styles`
for more information about how to do this.

One-line mode
-------------

The Text Area can be configured to keep all text on a single line with
:cpp:expr:`lv_textarea_set_one_line(textarea, true)`. In this mode:

- the height is set automatically to show only one line,
- line break characters are ignored, and
- word wrap is disabled.

Password mode
-------------

The Text Area supports password mode which can be enabled with
:cpp:expr:`lv_textarea_set_password_mode(textarea, true)`.

By default, if the ``•`` (`Bullet,
U+2022 <http://www.fileformat.info/info/unicode/char/2022/index.htm>`__)
character exists in the font, the entered characters are converted to it after
a configurable delay after each new character is entered. If ``•`` does not
exist in the font, ``*`` will be used. You can override the default
"masking" character with :cpp:expr:`lv_textarea_set_password_bullet(textarea, str)`
where ``str`` is a NUL-terminated C string.  Example:

.. code-block:: c

    lv_textarea_set_password_bullet(textarea, "x");

In password mode :cpp:expr:`lv_textarea_get_text(textarea)` returns the actual
text entered, not the bullet characters.

The visibility time can be adjusted with :c:macro:`LV_TEXTAREA_DEF_PWD_SHOW_TIME` in ``lv_conf.h``.

Accepted characters
-------------------

You can set a list of accepted characters with
:cpp:expr:`lv_textarea_set_accepted_chars(textarea, list)` where ``list`` is a
pointer to a NUL-terminated string, or NULL to accept all characters.  Characters
entered not in this list will be ignored.

.. code-block:: c

    lv_textarea_set_accepted_chars(textarea, "0123456789.+-");

Max text length
---------------

The maximum number of characters can be limited using
:cpp:expr:`lv_textarea_set_max_length(textarea, max_char_num)`.

Very long text
--------------

If the text in the Text Area is very long (e.g. > 20k characters), scrolling and
drawing might be slow.  However, by setting :c:macro:`LV_LABEL_LONG_TXT_HINT` in
``lv_conf.h`` to a non-zero value, the performance with long text is significantly
improved.  It does this by saving some additional information about the current
vertical position of the text shown.  With this mode configured, scrolling and drawing
is as fast as with "normal" short text.  The cost is 12 extra bytes per label in RAM.

This value is set to ``1`` by default.  If you do not use long text, you can save
12 bytes per label by setting it to ``0``.

Selecting text
--------------

If :c:macro:`LV_LABEL_TEXT_SELECTION` is set to a non-zero value in ``lv_conf.h``,
some additional functionality (and 8 bytes per label) are added to Label Widgets
and Text Area Widgets, and text-selection functionality is automated in Text Area
Widgets.  (If you do not use selected text in your application, you can save 8 bytes
per label in RAM by setting that macro to equate to ``0``.)

Any part of the text can be selected if enabled with
:cpp:expr:`lv_textarea_set_text_selection(textarea, true)`.  This works much like
when you select text on your PC by clicking and dragging with your mouse or other
pointer input device.  If you pass ``false`` to this function to disable text
selection, any text selected time of the call will be de-selected.

Shift+click and keyboard-only text selection with Shift+Arrow keys is expected to
be added in the future.

If you need to programmatically deal with selected text, in addition to the
:cpp:expr:`lv_textarea_set_text_selection(textarea, enable)` function, the following
is your tool set for doing so.  (``ta_label`` is a pointer to the Text Area's
Label retrieved with ``ta_label = lv_textarea_get_label(textarea);``.)

- :cpp:expr:`lv_textarea_get_text_selection(textarea)` tells whether text selection is enabled.
- :cpp:expr:`lv_textarea_text_is_selected(textarea)` tells whether any text is currently selected.
- :cpp:expr:`lv_textarea_clear_selection(textarea)` clears current text selection.
- :cpp:expr:`lv_label_set_text_selection_start(ta_label, index)` where ``index`` is
  the zero-based index of the first character of the selected text.
  Pass :c:macro:`LV_DRAW_LABEL_NO_TXT_SEL` to specify no text selected.
- :cpp:expr:`lv_label_set_text_selection_end(ta_label, index)` where ``index`` is
  the zero-based index of the character just after the selected text.
  Pass :c:macro:`LV_DRAW_LABEL_NO_TXT_SEL` to specify no text selected.
- :cpp:expr:`lv_label_get_text_selection_start(ta_label)` zero-based index of the
  first character of the selected text.
  :c:macro:`LV_DRAW_LABEL_NO_TXT_SEL` indicates no text selected.
- :cpp:expr:`lv_label_get_text_selection_end(ta_label)` zero-based index of the
  character just after the selected text.
  :c:macro:`LV_DRAW_LABEL_NO_TXT_SEL` indicates no text selected.

Normally you won't need these since Text Area automates the text selection process,
but if you do need to change the selection programmatically, the above are your
tools to do so.


.. _lv_textarea_events:

Events
******

-  :cpp:enumerator:`LV_EVENT_INSERT` Sent right before a character or text is
   inserted. The event parameter is the text about to be inserted.
   :cpp:expr:`lv_textarea_set_insert_replace(textarea, "New text")` can be called
   from within that event to replace the text to be inserted.  The contents of the
   buffer passed must be survive long enough for the call to `lv_timer_handler()`
   that is driving the event to return (after which the Text Area's label will have
   copied the text).  So it should not be a local buffer (created on the stack)
   where its contents will be destroyed before that happens.  Passing ``""`` means "do
   not insert anything".
-  :cpp:enumerator:`LV_EVENT_VALUE_CHANGED` Sent when the content of the Text Area
   has changed.
-  :cpp:enumerator:`LV_EVENT_READY` Sent when :cpp:enumerator:`LV_KEY_ENTER` is
   pressed (or sent) to a one-line Text Area.

.. admonition::  Further Reading

    Learn more about :ref:`lv_obj_events` emitted by all Widgets.

    Learn more about :ref:`events`.



.. _lv_textarea_keys:

Keys
****

-  ``LV_KEY_UP/DOWN/LEFT/RIGHT`` Move the cursor
-  ``Any character`` Add the character to the current cursor position

.. admonition::  Further Reading

    Learn more about :ref:`indev_keys`.



.. _lv_textarea_example:

Examples
********

.. include:: /examples/widgets/textarea/index.rst



.. _lv_textarea_api:

API
***
