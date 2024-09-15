.. _lv_arc:

============
Arc (lv_arc)
============

Overview
********

The Arc consists of a background and a foreground arc. The foreground
(indicator) can be touch-adjusted.

.. _lv_arc_parts_and_styles:

Parts and Styles
****************

-  :cpp:enumerator:`LV_PART_MAIN` Draws a background using the typical background
   style properties and an arc using the arc style properties. The arc's
   size and position will respect the *padding* style properties.
-  :cpp:enumerator:`LV_PART_INDICATOR` Draws another arc using the *arc* style
   properties. Its padding values are interpreted relative to the
   background arc.
-  :cpp:enumerator:`LV_PART_KNOB` Draws a handle on the end of the indicator using all
   background properties and padding values. With zero padding the knob
   size is the same as the indicator's width. Larger padding makes it
   larger, smaller padding makes it smaller.

.. _lv_arc_usage:

Usage
*****

Value and range
---------------

A new value can be set using :cpp:expr:`lv_arc_set_value(arc, new_value)`. The
value is interpreted in a range (minimum and maximum values) which can
be modified with :cpp:expr:`lv_arc_set_range(arc, min, max)`. The default range
is 0..100.

The indicator arc is drawn on the main part's arc. This if the value is
set to maximum the indicator arc will cover the entire "background" arc.
To set the start and end angle of the background arc use the
:cpp:expr:`lv_arc_set_bg_angles(arc, start_angle, end_angle)` functions or
``lv_arc_set_bg_start/end_angle(arc, angle)``.

Zero degrees is at the middle right (3 o'clock) of the object and the
degrees are increasing in clockwise direction. The angles should be in
the [0;360] range.

Rotation
--------

An offset to the 0 degree position can be added with
:cpp:expr:`lv_arc_set_rotation(arc, deg)`.

Mode
----

The arc can be one of the following modes:

- :cpp:enumerator:`LV_ARC_MODE_NORMAL` The indicator arc is drawn from the minimum value to the current.
- :cpp:enumerator:`LV_ARC_MODE_REVERSE` The indicator arc is drawn counter-clockwise
  from the maximum value to the current.
- :cpp:enumerator:`LV_ARC_MODE_SYMMETRICAL` The indicator arc is drawn from the middle point to the current value.

The mode can be set by :cpp:expr:`lv_arc_set_mode(arc, LV_ARC_MODE_...)` and
used only if the angle is set by :cpp:func:`lv_arc_set_value` or the arc is
adjusted by finger.

Change rate
-----------

If the arc is pressed the current value will set with a limited speed
according to the set *change rate*. The change rate is defined in
degree/second unit and can be set with
:cpp:expr:`lv_arc_set_change_rage(arc, rate)`

Knob offset
-----------

Changing the knob offset allows the location of the knob to be moved
relative to the end of the arc The knob offset can be set by
:cpp:expr:`lv_arc_set_knob_offset(arc, offset_angle)`, will only be visible if
:cpp:enumerator:`LV_PART_KNOB` is visible

Setting the indicator manually
------------------------------

It's also possible to set the angles of the indicator arc directly with
:cpp:expr:`lv_arc_set_angles(arc, start_angle, end_angle)` function or
``lv_arc_set_start/end_angle(arc, start_angle)``. In this case the set
"value" and "mode" are ignored.

In other words, the angle and value settings are independent. You should
exclusively use one or the other. Mixing the two might result in
unintended behavior.

To make the arc non-adjustable, remove the style of the knob and make
the object non-clickable:

.. code:: c

   lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
   lv_obj_remove_flag(arc, LV_OBJ_FLAG_CLICKABLE);

Interactive area
----------------

By default :cpp:enumerator:`LV_OBJ_FLAG_ADV_HITTEST` is disabled which 
means the arc's whole area is interactive. 
As usual :cpp:func:`lv_obj_set_ext_click_size` can be used to increase
the sensitive area outside the arc by a specified number of pixels.

If :cpp:enumerator:`LV_OBJ_FLAG_ADV_HITTEST` is enabled the arc will be sensitive only 
in the range of start and end background angles and on the arc itself (not inside the arc). 
In this case ``ext_click_size`` makes the sensitive area ticker both inward and outward.
Additionally, a tolerance of :cpp:expr:`lv_dpx(50)` pixels is applied to each angle, extending the 
hit-test range along the arc's length.

Place another object to the knob
--------------------------------

Another object can be positioned according to the current position of
the arc in order to follow the arc's current value (angle). To do this
use :cpp:expr:`lv_arc_align_obj_to_angle(arc, obj_to_align, radius_offset)`.

Similarly
:cpp:expr:`lv_arc_rotate_obj_to_angle(arc, obj_to_rotate, radius_offset)` can be
used to rotate the object to the current value of the arc.

It's a typical use case to call these functions in the ``VALUE_CHANGED``
event of the arc.

.. _lv_arc_events:

Events
******

-  :cpp:enumerator:`LV_EVENT_VALUE_CHANGED` sent when the arc is pressed/dragged to
   set a new value.
-  :cpp:enumerator:`LV_EVENT_DRAW_PART_BEGIN` and :cpp:enumerator:`LV_EVENT_DRAW_PART_END` are sent
   with the following types:

   -  :cpp:enumerator:`LV_ARC_DRAW_PART_BACKGROUND` The background arc.

      -  ``part``: :cpp:enumerator:`LV_PART_MAIN`
      -  ``p1``: center of the arc
      -  ``radius``: radius of the arc
      -  ``arc_dsc``

   -  :cpp:enumerator:`LV_ARC_DRAW_PART_FOREGROUND` The foreground arc.

      -  ``part``: :cpp:enumerator:`LV_PART_INDICATOR`
      -  ``p1``: center of the arc
      -  ``radius``: radius of the arc
      -  ``arc_dsc``

   -  LV_ARC_DRAW_PART_KNOB The knob

      -  ``part``: :cpp:enumerator:`LV_PART_KNOB`
      -  ``draw_area``: the area of the knob
      -  ``rect_dsc``:

See the events of the :ref:`Base object <lv_obj>` too.

Learn more about :ref:`events`.

.. _lv_arc_keys:

Keys
****

-  ``LV_KEY_RIGHT/UP`` Increases the value by one.
-  ``LV_KEY_LEFT/DOWN`` Decreases the value by one.

Learn more about :ref:`indev_keys`.

.. _lv_arc_example:

Example
*******

.. include:: ../examples/widgets/arc/index.rst

.. _lv_arc_api:

API
***
