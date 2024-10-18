.. _lv_slider:

==================
Slider (lv_slider)
==================

Overview
********

The Slider object looks like a `Bar </widgets/bar>`__ supplemented with
a knob. The knob can be dragged to set a value. Just like Bar, Slider
can be vertical or horizontal.

.. _lv_slider_parts_and_styles:

Parts and Styles
****************

-  :cpp:enumerator:`LV_PART_MAIN` The background of the slider. Uses all the typical
   background style properties. ``padding`` makes the indicator smaller
   in the respective direction.
-  :cpp:enumerator:`LV_PART_INDICATOR` The indicator that shows the current state of
   the slider. Also uses all the typical background style properties.
-  :cpp:enumerator:`LV_PART_KNOB` A rectangle (or circle) drawn at the current value.
   Also uses all the typical background properties to describe the
   knob(s). By default, the knob is square (with an optional corner
   radius) with side length equal to the smaller side of the slider. The
   knob can be made larger with the ``padding`` values. Padding values
   can be asymmetric too.

.. _lv_slider_usage:

Usage
*****

Value and range
---------------

To set an initial value use :cpp:expr:`lv_slider_set_value(slider, new_value, LV_ANIM_ON/OFF)`. The
animation time is set by the styles' ``anim_time`` property.

To specify the range (min, max values), :cpp:expr:`lv_slider_set_range(slider, min , max)` can be used.
The default range is 0..100, and the default drawing direction is from left to right in horizontal mode and
bottom to top in vertical mode. If the minimum value is greater than the maximum value, like
100..0, the drawing direction changes to the opposite direction.

Modes
-----

The slider can be one of the following modes:

- :cpp:enumerator:`LV_SLIDER_MODE_NORMAL` A normal slider as described above
- :cpp:enumerator:`LV_SLIDER_SYMMETRICAL` Draw the indicator form the zero value to
  current value. Requires negative minimum range and positive maximum range.
- :cpp:enumerator:`LV_SLIDER_RANGE` Allows setting the start value too by
  :cpp:expr:`lv_bar_set_start_value(bar, new_value, LV_ANIM_ON/OFF)`. The start
  value has to be always smaller than the end value.

The mode can be changed with :cpp:expr:`lv_slider_set_mode(slider, LV_SLIDER_MODE_...)`

Knob-only mode
--------------

Normally, the slider can be adjusted either by dragging the knob, or by
clicking on the slider bar. In the latter case the knob moves to the
point clicked and slider value changes accordingly. In some cases it is
desirable to set the slider to react on dragging the knob only. This
feature is enabled by adding the :cpp:enumerator:`LV_OBJ_FLAG_ADV_HITTEST`:
:cpp:expr:`lv_obj_add_flag(slider, LV_OBJ_FLAG_ADV_HITTEST)`.

The extended click area (set by :cpp:expr:`lv_obj_set_ext_click_area(slider, value)`) increases to knob's click area.

.. _lv_slider_events:

Events
******

-  :cpp:enumerator:`LV_EVENT_VALUE_CHANGED` Sent while the slider is being dragged or
   changed with keys. The event is sent continuously while the slider is
   being dragged.
-  :cpp:enumerator:`LV_EVENT_RELEASED` Sent when the slider has just been released.

See the events of the `Bar </widgets/bar>`__ too.

Learn more about :ref:`events`.

.. _lv_slider_keys:

Keys
****

-  ``LV_KEY_UP/RIGHT`` Increment the slider's value by 1
-  ``LV_KEY_DOWN/LEFT`` Decrement the slider's value by 1

Learn more about :ref:`indev_keys`.

.. _lv_slider_example:

Example
*******

.. include:: ../examples/widgets/slider/index.rst

.. _lv_slider_api:

API
***
