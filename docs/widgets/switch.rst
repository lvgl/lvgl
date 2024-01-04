.. _lv_switch:

==================
Switch (lv_switch)
==================

Overview
********

The Switch looks like a little slider and can be used to turn something
on and off.

.. _lv_switch_parts_and_styles:

Parts and Styles
****************

-  :cpp:enumerator:`LV_PART_MAIN` The background of the switch uses all the typical
   background style properties. ``padding`` makes the indicator smaller
   in the respective direction.
-  :cpp:enumerator:`LV_PART_INDICATOR` The indicator that shows the current state of
   the switch. Also uses all the typical background style properties.
-  :cpp:enumerator:`LV_PART_KNOB` A rectangle (or circle) drawn at left or right side
   of the indicator. Also uses all the typical background properties to
   describe the knob(s). By default, the knob is square (with an
   optional corner radius) with side length equal to the smaller side of
   the slider. The knob can be made larger with the ``padding`` values.
   Padding values can be asymmetric too.

.. _lv_switch_usage:

Usage
*****

Change state
------------

The switch uses the standard :cpp:enumerator:`LV_STATE_CHECKED` state.

To get the current state of the switch (with ``true`` being on), use
:cpp:expr:`lv_obj_has_state(obj, LV_STATE_CHECKED)`.

Call :cpp:expr:`lv_obj_add_state(obj, LV_STATE_CHECKED)` to turn it on, or
:cpp:expr:`lv_obj_remove_state(obj, LV_STATE_CHECKED)` to turn it off.

.. _lv_switch_events:

Events
******

-  :cpp:enumerator:`LV_EVENT_VALUE_CHANGED` Sent when the switch changes state.

See the events of the :ref:`Base object <lv_obj>` too.

Learn more about :ref:`events`.

.. _lv_switch_keys:

Keys
****

-  ``LV_KEY_UP/RIGHT`` Turns on the slider
-  ``LV_KEY_DOWN/LEFT`` Turns off the slider
-  :cpp:enumerator:`LV_KEY_ENTER` Toggles the switch

Learn more about :ref:`indev_keys`.

.. _lv_switch_example:

Example
*******

.. include:: ../examples/widgets/switch/index.rst

.. _lv_switch_api:

API
***
