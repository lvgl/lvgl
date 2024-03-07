.. _gridnav:

===============
Grid navigation
===============

Grid navigation (gridnav for short) is a feature that changes the
currently focused child object as arrow keys are pressed.

If the children are arranged into a grid-like layout then the up, down,
left and right arrows move focus to the nearest sibling in the
respective direction.

It doesn't matter how the children are positioned, as only the current x
and y coordinates are considered. This means that gridnav works with
manually positioned children, as well as :ref:`flex` and
:ref:`grid` layouts.

Gridnav also works if the children are arranged into a single row or
column. That makes it useful, for example, to simplify navigation on a
:ref:`List widget <lv_list>`.

Gridnav assumes that the object to which gridnav is added is part of a
:ref:`group <indev_groups>`. This way, if the object with
gridnav is focused, the arrow key presses are automatically forwarded to
the object so that gridnav can process the arrow keys.

To move the focus to the next widget of the group use
:cpp:enumerator:`LV_KEY_NEXT` or :cpp:enumerator:`LV_KEY_PREV`.
Optionally you can also use :cpp:func:`lv_group_focus_next` 
or :cpp:func:`lv_group_focus_prev` or the ``TAB``
key on keyboard as usual.

If the container is scrollable and the focused child is out of the view,
gridnav will automatically scroll the child into view.

.. _gridnav_usage:

Usage
-----

To add the gridnav feature to an object use
:cpp:expr:`lv_gridnav_add(cont, flags)`.

``flags`` control the behavior of gridnav:

- :cpp:enumerator:`LV_GRIDNAV_CTRL_NONE`: Default settings
- :cpp:enumerator:`LV_GRIDNAV_CTRL_ROLLOVER`: If there is no next/previous object in a
  direction, the focus goes to the object in the next/previous row (on
  left/right keys) or first/last row (on up/down keys)
- :cpp:enumerator:`LV_GRIDNAV_CTRL_SCROLL_FIRST`: If an arrow is pressed and the focused
  object can be scrolled in that direction then it will be scrolled instead of
  going to the next/previous object. If there is no more room for scrolling the
  next/previous object will be focused normally

:cpp:expr:`lv_gridnav_remove(cont)` Removes gridnav from an object.

Focusable objects
-----------------

An object needs to be clickable or click focusable
(:cpp:enumerator:`LV_OBJ_FLAG_CLICKABLE` or :cpp:enumerator:`LV_OBJ_FLAG_CLICK_FOCUSABLE`) and not
hidden (:cpp:enumerator:`LV_OBJ_FLAG_HIDDEN`) to be focusable by gridnav.

.. _gridnav_example:

Example
-------

.. include:: ../examples/others/gridnav/index.rst

.. _gridnav_api:

API
---

