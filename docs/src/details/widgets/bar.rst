.. _lv_bar:

============
Bar (lv_bar)
============


Overview
********

The Bar Widget has a background and an indicator. The length of the
indicator against the background indicates the Bar's current value.

Both the start and end values of the Bar can be set. Changing the start value to a
value other than the minimum value in its range adjusts the start position of the indicator.



.. _lv_bar_parts_and_styles:

Parts and Styles
****************

-  :cpp:enumerator:`LV_PART_MAIN` The Bar's background.  It uses the :ref:`typical
   background style properties <typical bg props>`. Adding padding makes the indicator
   smaller or larger. The ``anim_time`` style property sets the
   animation time if the values set with :cpp:enumerator:`LV_ANIM_ON`.
-  :cpp:enumerator:`LV_PART_INDICATOR` The Bar's indicator; also uses the :ref:`typical
   background style properties <typical bg props>`.

.. _lv_bar_usage:

Usage
*****


Orientation and size
--------------------

- for orientation, width and height, simply set width and height style properties;
- :cpp:expr:`lv_bar_set_orientation(bar, orientation)` to override orientation
  caused by ``width`` and ``height``.  Valid values for ``orientation`` are:

  - :cpp:enumerator:`LV_BAR_ORIENTATION_AUTO`
  - :cpp:enumerator:`LV_BAR_ORIENTATION_HORIZONTAL`
  - :cpp:enumerator:`LV_BAR_ORIENTATION_VERTICAL`


Value and range
---------------

A new value can be set with
:cpp:expr:`lv_bar_set_value(bar, new_value, LV_ANIM_ON/OFF)`. The value is
interpreted in a range (minimum and maximum values) which can be
modified with :cpp:expr:`lv_bar_set_range(bar, min, max)`. The default range is
0..100, and the default drawing direction is from left to right in horizontal mode and
bottom to top in vertical mode. If the minimum value is greater than the maximum value, like
100..0, the drawing direction is reversed.

The new value in :cpp:func:`lv_bar_set_value` can be set with or without an
animation depending on the last parameter (``LV_ANIM_ON/OFF``).


Modes
-----

The Bar can be one of the following modes:

- :cpp:enumerator:`LV_BAR_MODE_NORMAL` A normal Bar as described above
- :cpp:enumerator:`LV_BAR_MODE_SYMMETRICAL` Draws indicator from zero value to current value. Requires a negative
  minimum value and positive maximum value, e.g. [-100..100].
- :cpp:enumerator:`LV_BAR_MODE_RANGE` Allows setting the start value as well with
  :cpp:expr:`lv_bar_set_start_value(bar, new_value, LV_ANIM_ON/OFF)`. The start
  value must be smaller than the end value.



.. _lv_bar_events:

Events
******

No special events are sent by Bar Widgets.

.. admonition::  Further Reading

    Learn more about :ref:`lv_obj_events` emitted by all Widgets.

    Learn more about :ref:`events`.



.. _lv_bar_keys:

Keys
****

No *Keys* are processed by Bar Widgets.

.. admonition::  Further Reading

    Learn more about :ref:`indev_keys`.



.. _lv_bar_example:

Example
*******

.. include:: ../../examples/widgets/bar/index.rst



.. _lv_bar_api:

API
***
