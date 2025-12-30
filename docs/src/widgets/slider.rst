.. _lv_slider:

==================
Slider (lv_slider)
==================



Overview
********

The Slider Widget looks like a :ref:`lv_bar` supplemented with
a knob. The knob can be dragged to set the Slider's value. Like Bar, a Slider
can be vertical or horizontal.



.. _lv_slider_parts_and_styles:

Parts and Styles
****************

-  :cpp:enumerator:`LV_PART_MAIN` The background of the Slider. Uses the
   :ref:`typical background style properties <typical bg props>`. ``padding`` makes
   the indicator smaller in the respective direction.
-  :cpp:enumerator:`LV_PART_INDICATOR` The indicator that shows the current state of
   the Slider; also uses the :ref:`typical background style properties <typical bg props>`.
-  :cpp:enumerator:`LV_PART_KNOB` A rectangle (or circle) drawn at the current value;
   also uses the :ref:`typical background style properties <typical bg props>` to
   describe the knob(s).  By default, the knob is round (radius-style can modify this)
   with side length equal to the smaller dimension of the Slider.  The knob can be
   made larger with the ``padding`` values.  Padding values can be asymmetric as well.



.. _lv_slider_usage:

Usage
*****

Value, range and orientation
----------------------------

Once a Slider is created, it has:

- value == 0
- default range of [0..100],
- horizontal orientation, with
- default width of approximately 2 inches (according to configured value of :c:macro:`LV_DPI_DEF`),
- default height of approximately 1/10 inch (according to configured value of :c:macro:`LV_DPI_DEF`).

To set different values use:

- :cpp:expr:`lv_slider_set_value(slider, new_value, LV_ANIM_ON/OFF)` (animation time
  is set by the styles' ``anim_time`` property);
- :cpp:expr:`lv_slider_set_range(slider, min , max)`; and
- for orientation, width and height, simply set width and height properties;
- :cpp:expr:`lv_slider_set_orientation(slider, orientation)` to override orientation
  caused by ``width`` and ``height``.  Valid values for ``orientation`` are:

  - :cpp:enumerator:`LV_SLIDER_ORIENTATION_AUTO`
  - :cpp:enumerator:`LV_SLIDER_ORIENTATION_HORIZONTAL`
  - :cpp:enumerator:`LV_SLIDER_ORIENTATION_VERTICAL`

The default drawing direction is from left to right in horizontal orientation and
bottom to top in vertical orientation.  If the minimum value is set to be greater
than the maximum value (e.g. [100..0]), the drawing direction is reversed.

Modes
-----

The Slider can be in one of the following modes:

- :cpp:enumerator:`LV_SLIDER_MODE_NORMAL` A normal Slider as described above (default)
- :cpp:enumerator:`LV_SLIDER_SYMMETRICAL` Draw the indicator from the zero value to
  current value. Requires negative minimum range and positive maximum range.
- :cpp:enumerator:`LV_SLIDER_RANGE` Allows setting the start value as well by
  :cpp:expr:`lv_bar_set_start_value(slider, new_value, LV_ANIM_ON/OFF)`. The start
  value must always be smaller than the end value.

The mode can be changed with :cpp:expr:`lv_slider_set_mode(slider, LV_SLIDER_MODE_...)`

Knob-only mode
--------------

Normally, the Slider can be adjusted either by dragging the knob, or by
clicking on the Slider bar. In the latter case the knob moves to the
point clicked and the Slider value changes accordingly. In some cases it is
desirable to set the Slider to react on dragging the knob only. This
feature is enabled by adding the :cpp:enumerator:`LV_OBJ_FLAG_ADV_HITTEST` flag:
:cpp:expr:`lv_obj_add_flag(slider, LV_OBJ_FLAG_ADV_HITTEST)`.

Any extended click area (set by :cpp:expr:`lv_obj_set_ext_click_area(slider, value)`)
increases the knob's click area.

Data binding
------------

To get familiar with observers, subjects, and data bindings in general, visit the
:ref:`Observer <observer_how_to_use>` page.

This method of subscribing to an integer Subject affects a Slider Widget's integer
value directly.  Note that this is a two-way binding (Subject <===> Widget), so an end
user's direct interaction with the Slider Widget updates the Subject's value and vice
versa.

It supports integer and float subjects.

- :cpp:expr:`lv_slider_bind_value(slider, &subject)`


.. _lv_slider_events:

Events
******

-  :cpp:enumerator:`LV_EVENT_VALUE_CHANGED` Sent while the Slider is being dragged or
   changed with keys. The event is sent continuously while the Slider is
   being dragged.
-  :cpp:enumerator:`LV_EVENT_RELEASED` Sent once when Slider is released.

.. admonition::  Further Reading

    :ref:`Bar Events <lv_bar_events>`.

    Learn more about :ref:`lv_obj_events` emitted by all Widgets.

    Learn more about :ref:`events`.



.. _lv_slider_keys:

Keys
****

-  ``LV_KEY_UP/RIGHT`` Increment Slider's value by 1.
-  ``LV_KEY_DOWN/LEFT`` Decrement Slider's value by 1.

.. admonition::  Further Reading

    Learn more about :ref:`indev_keys`.



.. _lv_slider_example:

Examples
********

.. include:: /examples/widgets/slider/index.rst



.. _lv_slider_api:

API
***
