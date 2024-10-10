.. _lv_switch:

==================
Switch (lv_switch)
==================

Overview
********

The Switch looks like a little slider and can be used to turn something
on and off.

Vertical Switch can be created if the width of the object is smaller than its height.

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

.. code-block:: c
   uint8_t dummy;
   /*Check if sw1 is turned on and increment dummy when so*/
   if(lv_obj_has_state(sw1, LV_STATE_CHECKED)){
     ++dummy;
   }

Call :cpp:expr:`lv_obj_add_state(obj, LV_STATE_CHECKED)` to turn it on, or
:cpp:expr:`lv_obj_remove_state(obj, LV_STATE_CHECKED)` to turn it off.

.. code-block:: c
   lv_obj_add_state(sw0, LV_STATE_CHECKED); /*turn on switch0 manually*/
   lv_obj_remove_state(sw2, LV_STATE_CHECKED); /*turn off switch2 manually*/

Change orientation
------------------

:cpp:expr:`lv_switch_set_orientation(obj, LV_SWITCH_ORIENTATION_VERTICAL)` change orientation, default orientation is :cpp:enumerator:`LV_SWITCH_ORIENTATION_AUTO`, adaptive based on the width and height of the object.

.. code-block:: c
   lv_switch_set_orientation(sw5, LV_SWITCH_ORIENTATION_VERTICAL); /*set the orientation of switch5 to vertical*/
   lv_switch_set_orientation(sw7, LV_SWITCH_ORIENTATION_HORIZONTAL); /*set the orientation of switch7 to horizontal*/

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
