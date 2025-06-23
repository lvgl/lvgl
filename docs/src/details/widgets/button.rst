.. _lv_button:

==================
Button (lv_button)
==================

Overview
********

Buttons have no new features compared to the :ref:`base_widget`.
They are useful for semantic purposes and have slightly different default settings.

Buttons, by default, differ from the Base Widget in the following ways:

- Not scrollable
- Added to the default group
- Default height and width set to :cpp:enumerator:`LV_SIZE_CONTENT`

.. _lv_button_parts_and_styles:

Parts and Styles
****************

-  :cpp:enumerator:`LV_PART_MAIN` The background of the button; uses the :ref:`typical
   background style properties <typical bg props>`.

.. _lv_button_usage:

Usage
*****

There are no new features compared to :ref:`Base Widget <base_widget>`.



.. _lv_button_events:

Events
******

-  :cpp:enumerator:`LV_EVENT_VALUE_CHANGED` when the :cpp:enumerator:`LV_OBJ_FLAG_CHECKABLE` flag is
   enabled and the Widget is clicked. The event happens on transition
   to/from the checked state.

.. admonition::  Further Reading

    Learn more about :ref:`lv_obj_events` emitted by all Widgets.

    Learn more about :ref:`events`.



.. _lv_button_keys:

Keys
****

Note that the state of :cpp:enumerator:`LV_KEY_ENTER` is translated to
:cpp:enumerator:`LV_EVENT_PRESSED`, :cpp:enumerator:`LV_EVENT_PRESSING`
and :cpp:enumerator:`LV_EVENT_RELEASED` etc.

.. admonition::  Further Reading

    Learn more about :ref:`indev_keys`.



.. _lv_button_example:

Example
*******

.. include:: ../../examples/widgets/button/index.rst



.. _lv_button_api:

API
***
