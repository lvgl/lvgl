Span (lv_span)
==============

Overview
********

A spangroup is the object that is used to display rich text. Different
from the label object, ``spangroup`` can render text styled with
different fonts, colors, and sizes into the spangroup object.

Parts and Styles
****************

-  :cpp:enumerator:`LV_PART_MAIN` The spangroup has only one part.

Usage
*****

Set text and style
------------------

The spangroup object uses span to describe text and text style. so,
first we need to create ``span`` descriptor using ``lv_span_t * span = lv_spangroup_new_span(spangroup)``.
Then use :cpp:expr:`lv_span_set_text(span, "text")` to set text. The style of the span is
configured as with a normal style object by using its ``style`` member,
eg::cpp:expr:`lv_style_set_text_color(&span->style, lv_palette_main(LV_PALETTE_RED))`.

If spangroup object ``mode != LV_SPAN_MODE_FIXED`` you must call
:cpp:func:`lv_spangroup_refr_mode` after you have modified ``span``
style(eg:set text, changed the font size, del span).

Retrieving a span child
-----------------------

Spangroups store their children differently from normal objects, so
normal functions for getting children won't work.

:cpp:expr:`lv_spangroup_get_child(spangroup, id)` will return a pointer to the
child span at index ``id``. In addition, ``id`` can be negative to index
from the end of the spangroup where ``-1`` is the youngest child, ``-2``
is second youngest, etc.

e.g. ``lv_span_t* span = lv_spangroup_get_child(spangroup, 0)`` will
return the first child of the spangroup.
``lv_span_t* span = lv_spangroup_get_child(spangroup, -1)`` will return
the last (or most recent) child.

Child Count
-----------

Use the function :cpp:expr:`lv_spangroup_get_span_count(spangroup)` to get back
the number of spans the group is maintaining.

e.g. ``uint32_t size = lv_spangroup_get_span_count(spangroup)``

Text align
----------

like label object, the spangroup can be set to one the following modes:

- :cpp:enumerator:`LV_TEXT_ALIGN_LEFT` Align text to left.
- :cpp:enumerator:`LV_TEXT_ALIGN_CENTER` Align text to center.
- :cpp:enumerator:`LV_TEXT_ALIGN_RIGHT` Align text to right.
- :cpp:enumerator:`LV_TEXT_ALIGN_AUTO` Align text auto.

use function :cpp:expr:`lv_spangroup_set_align(spangroup, LV_TEXT_ALIGN_CENTER)`
to set text align.

Modes
-----

The spangroup can be set to one the following modes:

- :cpp:enumerator:`LV_SPAN_MODE_FIXED` fixes the object size.
- :cpp:enumerator:`LV_SPAN_MODE_EXPAND` Expand the object size to the text size but stay on a single line.
- :cpp:enumerator:`LV_SPAN_MODE_BREAK` Keep width, break the too long lines and auto expand height.

Use :cpp:expr:`lv_spangroup_set_mode(spangroup, LV_SPAN_MODE_BREAK)` to set
object mode.

Overflow
--------

The spangroup can be set to one the following modes:

- :cpp:enumerator:`LV_SPAN_OVERFLOW_CLIP` truncates the text at the limit of the area.
- :cpp:enumerator:`LV_SPAN_OVERFLOW_ELLIPSIS` will display an ellipsis(``...``) when text overflows the area.

Use :cpp:expr:`lv_spangroup_set_overflow(spangroup, LV_SPAN_OVERFLOW_CLIP)` to set object overflow mode.

First line indent
-----------------

Use :cpp:expr:`lv_spangroup_set_indent(spangroup, 20)` to set the indent of the
first line. all modes support pixel units, in addition to :cpp:enumerator:`LV_SPAN_MODE_FIXED`
and :cpp:enumerator:`LV_SPAN_MODE_BREAK` mode supports percentage units
too.

Lines
-----

Use :cpp:expr:`lv_spangroup_set_max_lines(spangroup, 10)` to set the maximum number
of lines to be displayed in LV_SPAN_MODE_BREAK mode, negative values
indicate no limit.

Events
******

No special events are sent by this widget.

Learn more about :ref:`events`.

Keys
****

No *Keys* are processed by the object type.

Learn more about :ref:`indev_keys`.

Example
*******


.. include:: ../examples/widgets/span/index.rst

API
***
