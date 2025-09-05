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
   style properties and an arc using the Arc style properties. The Arc's
   size and position will respect the *padding* style properties.
-  :cpp:enumerator:`LV_PART_INDICATOR` Draws another Arc using the *Arc* style
   properties. Its padding values are interpreted relative to the
   background Arc.
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

The indicator Arc is drawn on the main part's Arc. Thus, if the value is
set to maximum, the indicator Arc will cover the entire "background" Arc.
To set the start and end angle of the background Arc use any of these functions:

- :cpp:expr:`lv_arc_set_bg_start_angle(arc, angle)`
- :cpp:expr:`lv_arc_set_bg_end_angle(arc, angle)`
- :cpp:expr:`lv_arc_set_bg_angles(arc, start_angle, end_angle)`

Zero degrees is at the middle right (3 o'clock) of the Widget and the
degrees increasing in the clockwise direction.  The angle values should be in
the range [0..360].

Rotation
--------

An offset to the 0-degree position can be added with
:cpp:expr:`lv_arc_set_rotation(arc, deg)`.

Mode
----

The Arc can be one of the following modes:

- :cpp:enumerator:`LV_ARC_MODE_NORMAL` Indicator Arc is drawn clockwise from minimum to current value.
- :cpp:enumerator:`LV_ARC_MODE_REVERSE` Indicator Arc is drawn counter-clockwise
  from maximum to current value.
- :cpp:enumerator:`LV_ARC_MODE_SYMMETRICAL` Indicator Arc is drawn from middle point to current value.

The mode can be set by :cpp:expr:`lv_arc_set_mode(arc, LV_ARC_MODE_...)` and
has no effect until angle is set by :cpp:func:`lv_arc_set_value` or value of the Arc
is changed by pointer input (finger, mouse, etc.).

Change rate
-----------

When the Arc's value is changed by pointer input (finger, mouse, etc.), the rate of
its change is limited according to its *change rate*.  Change rate is defined in
degrees/second units and can be set with
:cpp:expr:`lv_arc_set_change_rate(arc, rate)`

Knob offset
-----------

Changing the knob offset allows the location of the knob to be moved
relative to the end of the Arc.  The knob offset can be set by
:cpp:expr:`lv_arc_set_knob_offset(arc, offset_angle)`, and will only be visible if
:cpp:enumerator:`LV_PART_KNOB` is visible.

Setting indicator programmatically
----------------------------------

It is possible to set indicator angle directly with any of these functions:

- :cpp:expr:`lv_arc_set_start_angle(arc, start_angle)`
- :cpp:expr:`lv_arc_set_end_angle(arc, end_angle)`
- :cpp:expr:`lv_arc_set_angles(arc, start_angle, end_angle)`

When used, "value" and "mode" are ignored.

In other words, the angle and value settings are independent.  You should
exclusively use one or the other of the two methods.  Mixing the two could
result in unintended behavior.

To make the arc non-adjustable, remove the style of the knob and
make the Widget non-clickable:

.. code-block:: c

   lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
   lv_obj_remove_flag(arc, LV_OBJ_FLAG_CLICKABLE);

Interactive area
----------------

By default :cpp:enumerator:`LV_OBJ_FLAG_ADV_HITTEST` is disabled which
means the Arc's whole area is interactive.
As usual :cpp:func:`lv_obj_set_ext_click_area` can be used to increase
the area that will respond to pointer input (touch, mouse, etc.) outside the Arc by a
specified number of pixels.

If :cpp:enumerator:`LV_OBJ_FLAG_ADV_HITTEST` is enabled the Arc will be sensitive only
in the range between start and end background angles and on the Arc itself (not inside the Arc).
In this case ``ext_click_area`` makes the sensitive area ticker both inward and outward.
Additionally, a tolerance of :cpp:expr:`lv_dpx(50)` pixels is applied to each angle, extending the
hit-test range along the Arc's length.

Place another Widget on the knob
--------------------------------

Another Widget can be positioned according to the current position of
the Arc in order to follow the Arc's current value (angle). To do this
use :cpp:expr:`lv_arc_align_obj_to_angle(arc, widget_to_align, radius_offset)`.

Similarly
:cpp:expr:`lv_arc_rotate_obj_to_angle(arc, widget_to_rotate, radius_offset)` can be
used to rotate the Widget to the current value of the Arc.

A typical use case is to call these functions in the ``VALUE_CHANGED``
event of the Arc.

Data binding
------------

To get familiar with observers, subjects, and data bindings in general visit the
:ref:`Observer <observer_how_to_use>` page.

This method of subscribing to an integer Subject affects an Arc Widget's integer
value directly.  Note that this is a two-way binding (Subject <===> Widget), so an end
user's direct interaction with the Arc Widget updates the Subject's value and vice
versa.

It supports integer subjects and, when ``LV_USE_FLOAT`` is enabled, float subjects.


- :cpp:expr:`lv_arc_bind_value(arc, &subject)`


.. _lv_arc_events:

Events
******

-  :cpp:enumerator:`LV_EVENT_VALUE_CHANGED` sent when Arc is pressed/dragged to
   a new value.


.. admonition::  Further Reading

    Learn more about :ref:`lv_obj_events` emitted by all Widgets.

    Learn more about :ref:`events`.



.. _lv_arc_keys:

Keys
****

-  ``LV_KEY_RIGHT/UP`` Increases value by one.
-  ``LV_KEY_LEFT/DOWN`` Decreases value by one.

.. admonition::  Further Reading

    Learn more about :ref:`indev_keys`.



.. _lv_arc_example:

Example
*******

.. include:: ../../examples/widgets/arc/index.rst



.. _lv_arc_api:

API
***
