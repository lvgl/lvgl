.. _lv_spangroup:

========================
Spangroup (lv_spangroup)
========================


Overview
********

The Spangroup Widget is used to display rich text.  Different
from the Label Widget, Spangroups can render text styled with
different fonts, colors, and sizes into the Spangroup Widget.
See example below.

A Spangroup contains 0 or more Span Descriptors ("Spans").  Each Span contains its
own text and style properties for that text.  You add 1 Span (as a child) to the
Spangroup for each "span" of uniquely-styled text needed.  Each Span so added is
appended to the end of the list.  The list sequence determines the order in which the
Spans are displayed.  Spans can be added to, and removed from, the Spangroup
throughout its life.  The number of Spans that can be added is limited only by
available RAM.



.. _lv_spangroup_parts_and_styles:

Parts and Styles
****************

-  :cpp:enumerator:`LV_PART_MAIN` Spangroup has only one part.



.. _lv_spangroup_usage:

Usage
*****

Set text and style
------------------

Add each needed Span to a Spangroup like this:

.. code-block:: c

    lv_span_t * span = lv_spangroup_new_span(spangroup);

After a Span is created, use the following functions to set its text
and style properties:

- :cpp:expr:`lv_span_set_text(span, "text")`
- :cpp:expr:`lv_style_set_<property_name>(&span->style, value)`

Example of the latter:  :cpp:expr:`lv_style_set_text_color(&span->style, lv_palette_main(LV_PALETTE_RED))`.

If the Spangroup Widget's ``mode != LV_SPAN_MODE_FIXED`` call
:cpp:expr:`lv_spangroup_refr_mode(spangroup)` after you have modifying any of its
Spans to ensure it is redrawn appropriately.


Retrieving a Span child
-----------------------

Spangroups store their children differently from normal Widgets, so
normal functions for getting children won't work.

:cpp:expr:`lv_spangroup_get_child(spangroup, id)` will return a pointer to the
child Span at index ``id``. In addition, ``id`` can be negative to index
from the end of the Spangroup where ``-1`` is the youngest child, ``-2``
is second youngest, etc.

E.g. ``lv_span_t * span = lv_spangroup_get_child(spangroup, 0)`` will
return the first child of the Spangroup.
``lv_span_t * span = lv_spangroup_get_child(spangroup, -1)`` will return
the last (or most recent) child.


Child count
-----------

Use :cpp:expr:`lv_spangroup_get_span_count(spangroup)` to get
the number of contained Spans.

E.g. ``uint32_t size = lv_spangroup_get_span_count(spangroup)``


Removing a Span
---------------
You can remove a Span at any time during the Spangroup's life using the function
:cpp:expr:`lv_spangroup_delete_span(spangroup, span)`.


Text align
----------

Like the Label Widget, a Spangroup can be set to one the following text-alignment modes:

- :cpp:enumerator:`LV_TEXT_ALIGN_LEFT` Align text to left.
- :cpp:enumerator:`LV_TEXT_ALIGN_CENTER` Center text.
- :cpp:enumerator:`LV_TEXT_ALIGN_RIGHT` Align text to right edge.
- :cpp:enumerator:`LV_TEXT_ALIGN_AUTO` Align auto.

Use function :cpp:expr:`lv_spangroup_set_align(spangroup, LV_TEXT_ALIGN_...)`
to set text alignment.


Modes
-----

**DEPRECATED**, set the width to ``LV_SIZE_CONTENT`` or fixed value to control expanding/wrapping.

A Spangroup can be set to one the following modes:

- :cpp:enumerator:`LV_SPAN_MODE_FIXED` Fixes its size.
- :cpp:enumerator:`LV_SPAN_MODE_EXPAND` Expand size to text size but stay on one line.
- :cpp:enumerator:`LV_SPAN_MODE_BREAK` Keep width; break lines that are too long and auto-expand height.

Use :cpp:expr:`lv_spangroup_set_mode(spangroup, LV_SPAN_MODE_BREAK)` to set its mode.


Overflow
--------

A Spangroup can be set to handle text overflow in one of the following ways:

- :cpp:enumerator:`LV_SPAN_OVERFLOW_CLIP` truncates text at the limit of the area.
- :cpp:enumerator:`LV_SPAN_OVERFLOW_ELLIPSIS` display an ellipsis (``...``) when text overflows the area.

Use :cpp:expr:`lv_spangroup_set_overflow(spangroup, LV_SPAN_OVERFLOW_CLIP)` to set
the Spangroup's overflow mode.


First line indent
-----------------

Use :cpp:expr:`lv_spangroup_set_indent(spangroup, 20)` to set the indent of the
first line. All modes support pixel units. In addition, :cpp:enumerator:`LV_SPAN_MODE_FIXED`
and :cpp:enumerator:`LV_SPAN_MODE_BREAK` modes support percentage units as well
(e.g. ``lv_pct(10)``).


Lines
-----

Use :cpp:expr:`lv_spangroup_set_max_lines(spangroup, 10)` to set the maximum number
of lines to be displayed in :cpp:enumerator:`LV_SPAN_MODE_BREAK` mode. A negative
value indicates no limit.


Data binding
------------

To get familiar with observers, subjects, and data bindings in general, visit the
:ref:`Observer <observer_how_to_use>` page.

Very similar to Label-text binding, a Span's text can be bound to a subject as well.

The only difference is that in the bind function both the Spangroup and the Span need to be specified:

:cpp:expr:`lv_spangroup_bind_span_text(spangroup, span1, &subject, format_string)`

Note that before calling :cpp:expr:`lv_spangroup_delete_span`, :cpp:expr:`lv_observer_remove`
needs to be called manually as LVGL can't remove the binding automatically.

.. _lv_spangroup_events:

Events
******

No special events are sent by Span Widgets.

.. admonition::  Further Reading

    Learn more about :ref:`lv_obj_events` emitted by all Widgets.

    Learn more about :ref:`events`.



.. _lv_spangroup_keys:

Keys
****

No *Keys* are processed by Span Widgets.

.. admonition::  Further Reading

    Learn more about :ref:`indev_keys`.



.. _lv_spangroup_example:

Example
*******

.. include:: ../../examples/widgets/span/index.rst



.. _lv_spangroup_api:

API
***
