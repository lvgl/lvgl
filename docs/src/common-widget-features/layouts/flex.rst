.. _flex:

====
Flex
====



Overview
********

The Flexbox (or Flex for short) is a subset of `CSS Flexbox`_ behaviors.

It can arrange items (child Widgets) into rows or columns (tracks), handle wrapping,
adjust the spacing between items and tracks, handle *grow* to make
item(s) fill remaining space with respect to min/max width and
height.

To make a Widget a Flex container call
:cpp:expr:`lv_obj_set_layout(widget, LV_LAYOUT_FLEX)`.

Note that the Flex layout feature of LVGL needs to be globally enabled
with :c:macro:`LV_USE_FLEX` in ``lv_conf.h``.



Terms
*****

-  **tracks**: rows or columns
-  **main direction**: row or column, the direction in which multiple items are
   placed first
-  **cross direction**: the direction perpendicular to the **main direction**
-  **wrap**: if there is no more space in the track, a new track is started
-  **grow**: if set on an item it will "grow" to fill the remaining space in
   the track. The available space will be distributed among items
   respective to their grow value (larger value means more space)
-  **gap**: the space between rows and columns or the items on a track

See `CSS Flexbox`_ for illustrations showing the meanings of these terms.



Simple Interface
****************

Use the following functions to set and control the Flex layout on any parent Widget.

.. note::

    The parent Widget must be a Flex container for these styles to take effect.
    The functions below cause the parent Widget to become a Flex container if it is
    not already.


.. _flex_flow:

Flex flow
---------

:cpp:expr:`lv_obj_set_flex_flow(widget, flex_flow)`

The possible values for ``flex_flow`` are:

- :cpp:enumerator:`LV_FLEX_FLOW_ROW`: Place the children in a row without wrapping
- :cpp:enumerator:`LV_FLEX_FLOW_COLUMN`: Place the children in a column without wrapping
- :cpp:enumerator:`LV_FLEX_FLOW_ROW_WRAP`: Place the children in a row with wrapping
- :cpp:enumerator:`LV_FLEX_FLOW_COLUMN_WRAP`: Place the children in a column with wrapping
- :cpp:enumerator:`LV_FLEX_FLOW_ROW_REVERSE`: Place the children in a row without wrapping but in reversed order
- :cpp:enumerator:`LV_FLEX_FLOW_COLUMN_REVERSE`: Place the children in a column without wrapping but in reversed order
- :cpp:enumerator:`LV_FLEX_FLOW_ROW_WRAP_REVERSE`: Place the children in a row with wrapping but in reversed order
- :cpp:enumerator:`LV_FLEX_FLOW_COLUMN_WRAP_REVERSE`: Place the children in a column with wrapping but in reversed order

These values cause the Widget's layout behavior to model `CSS Flexbox`_ behavior
by combining flex-direction_ and flex-wrap_ as defined under flex-flow_.



.. _flex_align:

Flex align
----------

To manage placement of children use
:cpp:expr:`lv_obj_set_flex_align(widget,  main_place, cross_place, track_cross_place)`
which makes the parent Widget model the Flex-container behavior defined `here
<justify-content_>`_.

-  ``main_place`` determines how to distribute the items in their track
   on the main axis. E.g. flush the items to the right on
   :cpp:enumerator:`LV_FLEX_FLOW_ROW_WRAP`. (It's called
   justify-content_ in CSS.)
-  ``cross_place`` determines how to distribute the items in their track
   on the cross axis. E.g. if the items have different height, align them
   against the bottom of the track. (It's called align-items_ in CSS.)
-  ``track_cross_place`` determines how to distribute the tracks (It's
   called align-content_ in CSS.)

The possible values are:

- :cpp:enumerator:`LV_FLEX_ALIGN_START`: means left when direction is horizontal, top when vertical (default)
- :cpp:enumerator:`LV_FLEX_ALIGN_END`: means right when direction is horizontal, bottom when vertical
- :cpp:enumerator:`LV_FLEX_ALIGN_CENTER`: simply center with respect to direction
- :cpp:enumerator:`LV_FLEX_ALIGN_SPACE_EVENLY`: items are distributed so
  that the spacing between any two items (and the space to the edges) is
  equal. Does not apply to ``track_cross_place``.
- :cpp:enumerator:`LV_FLEX_ALIGN_SPACE_AROUND`: items are evenly
  distributed in the track with equal space around them. Note that
  visually the spaces are not equal since all the items have equal space
  on both sides.  This makes the space between items double the space
  between edge items and the container's edge.  Does not apply to
  ``track_cross_place``.
- :cpp:enumerator:`LV_FLEX_ALIGN_SPACE_BETWEEN`: items are evenly distributed in
  the track with no space before and after first and last items.  Does not apply
  to ``track_cross_place``.

See justify-content_, align-items_ and align-content_ for illustrations of these values.


.. _flex_grow:

Flex grow
---------

Flex grow can be used to make one or more children fill available space in the track.
When more than one child Widget have non-zero grow values, all available space will
be distributed in proportion to their respective grow values.  For example, if there
is 400 px space remaining and 3 child Widgets with non-zero grow values:

- ``A`` with grow = 1
- ``B`` with grow = 1
- ``C`` with grow = 2

``A`` and ``B`` will occupy 100 px, and ``C`` will occupy 200 px.

Flex grow can be set on a child Widget with
:cpp:expr:`lv_obj_set_flex_grow(child, value)`. ``value`` needs to be >=
1 or 0 to disable grow on the child.

See flex-grow_ for an illustration of this behavior.



.. _flex_style:

Style Interface
***************

All Flex-related values are style properties under the hood so you
can use them as you would any other style property.

The following flex related style properties exist:

-  :cpp:enumerator:`FLEX_FLOW`
-  :cpp:enumerator:`FLEX_MAIN_PLACE`
-  :cpp:enumerator:`FLEX_CROSS_PLACE`
-  :cpp:enumerator:`FLEX_TRACK_PLACE`
-  :cpp:enumerator:`FLEX_GROW`

.. _flex_padding:

Internal padding
----------------

To modify the minimum space flexbox inserts between Widgets, the
following functions can be used to set the flex container padding style:

-  :cpp:func:`lv_style_set_pad_row` sets padding between rows.

-  :cpp:func:`lv_style_set_pad_column` sets padding between columns.

These can, for example, be used if you do not want any padding between
Widgets: :cpp:expr:`lv_style_set_pad_column(&row_container_style, 0)`



.. _flex_other:

Other Features
**************

RTL
---

If the base direction of the container is set the
:cpp:enumerator:`LV_BASE_DIR_RTL` the meaning of
:cpp:enumerator:`LV_FLEX_ALIGN_START` and
:cpp:enumerator:`LV_FLEX_ALIGN_END` is swapped on ``ROW`` layouts. I.e.
``START`` will mean right.

The items on ``ROW`` layouts, and tracks of ``COLUMN`` layouts will be
placed from right to left.

New track
---------

You can force Flex to put an item into a new line with
:cpp:expr:`lv_obj_add_flag(child, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK)`.



.. admonition::  Further Reading

    Learn more about `CSS Flexbox`_.



.. _flex_examples:

Examples
********

.. include:: /examples/layouts/flex/index.rst


..  Hyperlinks

.. _css flexbox:     https://css-tricks.com/snippets/css/a-guide-to-flexbox/
.. _flex-direction:  https://css-tricks.com/snippets/css/a-guide-to-flexbox/#aa-flex-direction
.. _flex-wrap:       https://css-tricks.com/snippets/css/a-guide-to-flexbox/#aa-flex-wrap
.. _flex-flow:       https://css-tricks.com/snippets/css/a-guide-to-flexbox/#aa-flex-flow
.. _justify-content: https://css-tricks.com/snippets/css/a-guide-to-flexbox/#aa-justify-content
.. _align-items:     https://css-tricks.com/snippets/css/a-guide-to-flexbox/#aa-align-items
.. _align-content:   https://css-tricks.com/snippets/css/a-guide-to-flexbox/#aa-align-content
.. _flex-grow:       https://css-tricks.com/snippets/css/a-guide-to-flexbox/#aa-flex-grow


.. _flex_api:

API
***
