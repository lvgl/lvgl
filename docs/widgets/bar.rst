.. _lv_bar:

============
Bar (lv_bar)
============

Overview
********

The bar object has a background and an indicator on it. The width of the
indicator is set according to the current value of the bar.

Vertical bars can be created if the width of the object is smaller than
its height.

Not only the end, but also the start value of the bar can be set, which
changes the start position of the indicator.

.. _lv_bar_parts_and_styles:

Parts and Styles
****************

-  :cpp:enumerator:`LV_PART_MAIN` The background of the bar and it uses the typical
   background style properties. Adding padding makes the indicator
   smaller or larger. The ``anim_time`` style property sets the
   animation time if the values set with :cpp:enumerator:`LV_ANIM_ON`.
-  :cpp:enumerator:`LV_PART_INDICATOR` The indicator itself; also uses all the typical
   background properties.

.. _lv_bar_usage:

Usage
*****

Value and range
---------------

A new value can be set by
``lv_bar_set_value(bar, new_value, LV_ANIM_ON/OFF)``. The value is
interpreted in a range (minimum and maximum values) which can be
modified with :cpp:expr:`lv_bar_set_range(bar, min, max)`. The default range is
0..100, and the default drawing direction is from left to right in horizontal mode and
bottom to top in vertical mode. If the minimum value is greater than the maximum value, like
100..0, the drawing direction changes to the opposite direction.

The new value in :cpp:func:`lv_bar_set_value` can be set with or without an
animation depending on the last parameter (``LV_ANIM_ON/OFF``).

Modes
-----

The bar can be one of the following modes:

- :cpp:enumerator:`LV_BAR_MODE_NORMAL` A normal bar as described above
- :cpp:enumerator:`LV_BAR_MODE_SYMMETRICAL` Draw the indicator from the zero value to current value. Requires a negative
  minimum range and positive maximum range.
- :cpp:enumerator:`LV_BAR_MODE_RANGE` Allows setting the start value too by
  ``lv_bar_set_start_value(bar, new_value, LV_ANIM_ON/OFF)``. The start
  value always has to be smaller than the end value.

.. _lv_bar_events:

Events
******
See the events of the :ref:`Base object <lv_obj>` too.

Learn more about :ref:`events`.

.. _lv_bar_keys:

Keys
****

No *Keys* are processed by the object type.

Learn more about :ref:`indev_keys`.

.. _lv_bar_example:

Example
*******

.. include:: ../examples/widgets/bar/index.rst

.. _lv_bar_api:

API
***
