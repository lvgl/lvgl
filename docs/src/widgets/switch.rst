.. _lv_switch:

==================
Switch (lv_switch)
==================


Overview
********

Switch Widgets look like little sliders and are used to display, and optionally
modify, a value that can be "on" or "off".

By default, a Switch is oriented horizontally. Its orientation will be vertical
if you set ``width`` < ``height``.



.. _lv_switch_parts_and_styles:

Parts and Styles
****************

-  :cpp:enumerator:`LV_PART_MAIN` Switch's background; uses the :ref:`typical
   background style properties <typical bg props>`. ``padding`` makes the indicator smaller
   in the respective direction.
-  :cpp:enumerator:`LV_PART_INDICATOR` The indicator that shows the current state of
   the Switch; also uses the :ref:`typical background style properties <typical bg props>`.
-  :cpp:enumerator:`LV_PART_KNOB` A rectangle (or circle) drawn at the left or right
   side of the indicator; also uses the :ref:`typical background style properties
   <typical bg props>` to modify the knob's appearance. By default, the knob is round
   (radius-style can modify this) with diameter slightly smaller than the smaller
   side of the slider.  The knob can be made larger with the ``padding`` values.
   Padding values can be asymmetric as well.



.. _lv_switch_usage:

Usage
*****

Change state
------------

The Switch uses the standard :cpp:enumerator:`LV_STATE_CHECKED` state.

To get the current state of the Switch (with ``true`` being ON), use
:cpp:expr:`lv_obj_has_state(widget, LV_STATE_CHECKED)`.

Call :cpp:expr:`lv_obj_add_state(widget, LV_STATE_CHECKED)` to turn it on, or
:cpp:expr:`lv_obj_remove_state(widget, LV_STATE_CHECKED)` to turn it off
programmatically.

Change orientation
------------------

With a Switch is created, its default orientation is
:cpp:enumerator:`LV_SWITCH_ORIENTATION_AUTO`, which causes it to be oriented based
on ``width`` and ``height``.  You can change this behavior using
:cpp:expr:`lv_switch_set_orientation(widget, orientation)`.  Possible values for
``orientation`` are:

- :cpp:enumerator:`LV_SWITCH_ORIENTATION_AUTO`
- :cpp:enumerator:`LV_SWITCH_ORIENTATION_HORIZONTAL`
- :cpp:enumerator:`LV_SWITCH_ORIENTATION_VERTICAL`



.. _lv_switch_events:

Events
******

-  :cpp:enumerator:`LV_EVENT_VALUE_CHANGED` Sent when Switch changes state.

.. admonition::  Further Reading

    Learn more about :ref:`lv_obj_events` emitted by all Widgets.

    Learn more about :ref:`events`.



.. _lv_switch_keys:

Keys
****

-  ``LV_KEY_UP/RIGHT`` Turns Switch ON
-  ``LV_KEY_DOWN/LEFT`` Turns Switch OFF
-  :cpp:enumerator:`LV_KEY_ENTER` Toggles the Switch

.. admonition::  Further Reading

    Learn more about :ref:`indev_keys`.



.. _lv_switch_example:

Examples
********

.. include:: /examples/widgets/switch/index.rst



.. _lv_switch_api:

API
***
