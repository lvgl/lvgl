.. _gridnav:

===============
Grid navigation
===============

Grid navigation (gridnav for short) is a feature that moves focus among a set
of child Widgets via arrow-key presses.

If the child Widgets are arranged into a grid-like layout then the up, down,
left and right arrows move focus to the nearest sibling in the
respective direction.

It doesn't matter how the children are positioned, as only the current x
and y coordinates are considered. This means that gridnav works with
manually positioned children, as well as :ref:`flex` and
:ref:`grid` layouts.

Gridnav also works if the children are arranged into a single row or
column. This is useful, for example, to simplify navigation among items in a
:ref:`List Widget <lv_list>`.

Gridnav assumes that the Widget to which gridnav is added is part of a
:ref:`group <indev_groups>`. This way, if the Widget with
gridnav has focus, the arrow key presses are automatically forwarded to
the Widget so that gridnav can process the arrow keys.

To move the focus to the next widget of the group use
:cpp:enumerator:`LV_KEY_NEXT` or :cpp:enumerator:`LV_KEY_PREV`.
Optionally you can also use :cpp:func:`lv_group_focus_next`
or :cpp:func:`lv_group_focus_prev` or the ``TAB``
key on keyboard as usual.

If the container is scrollable and the focused child is out of the view,
gridnav will automatically scroll the child into view.



.. _gridnav_usage:

Usage
*****

To add gridnav behavior to any Widget (e.g. one serving as a container for
child Widgets that the end user will navigate among using arrow keys) use
:cpp:expr:`lv_gridnav_add(container, flags)`.

The ``flags`` argument controls the navigation behavior:

- :cpp:enumerator:`LV_GRIDNAV_CTRL_NONE`: Default settings
- :cpp:enumerator:`LV_GRIDNAV_CTRL_ROLLOVER`: If there is no next/previous Widget in a
  direction, the focus goes to the Widget in the next/previous row (on
  left/right keys) or first/last row (on up/down keys)
- :cpp:enumerator:`LV_GRIDNAV_CTRL_SCROLL_FIRST`: If an arrow is pressed and the focused
  Widget can be scrolled in that direction then it will be scrolled instead of
  going to the next/previous Widget. If there is no more room for scrolling the
  next/previous Widget will receive focus normally.
- :cpp:enumerator:`LV_GRIDNAV_CTRL_HORIZONTAL_MOVE_ONLY`: Only use the left/right keys
  for grid navigation. Up/down key events will be sent to the Widget that has focus.
- :cpp:enumerator:`LV_GRIDNAV_CTRL_VERTICAL_MOVE_ONLY`: Only use the up/down keys
  for grid navigation. Left/right key events will be sent to the Widget that has focus.

While the above behaviors can be combined by bit-wise OR-ing the above values together,
:cpp:enumerator:`LV_GRIDNAV_CTRL_HORIZONTAL_MOVE_ONLY` and :cpp:enumerator:`LV_GRIDNAV_CTRL_VERTICAL_MOVE_ONLY`
should not be used together.

:cpp:expr:`lv_gridnav_remove(container)` Removes gridnav behavior from a Widget.



Focusable Widgets
*****************

A Widget needs to be clickable or click focusable
(:cpp:enumerator:`LV_OBJ_FLAG_CLICKABLE` or :cpp:enumerator:`LV_OBJ_FLAG_CLICK_FOCUSABLE`) and not
hidden (:cpp:enumerator:`LV_OBJ_FLAG_HIDDEN`) to receive focus via gridnav.



.. _gridnav_example:

Examples
********

.. include:: ../../examples/others/gridnav/index.rst



.. _gridnav_api:

API
***

