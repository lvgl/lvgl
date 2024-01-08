==================
Button (lv_button)
==================

Overview
********

Buttons have no new features compared to the :ref:`Base object <widgets/obj/base object (lv_obj) [0-1]>`. They are useful for semantic purposes and have
slightly different default settings.

Buttons, by default, differ from Base object in the following ways: -
Not scrollable - Added to the default group - Default height and width
set to :cpp:enumerator:`LV_SIZE_CONTENT`

Parts and Styles
****************

-  :cpp:enumerator:`LV_PART_MAIN` The background of the button. Uses the typical
   background style properties.

Usage
*****

There are no new features compared to :ref:`Base object <widgets/obj/base object (lv_obj) [0-1]>`.

Events
******

-  :cpp:enumerator:`LV_EVENT_VALUE_CHANGED` when the :cpp:enumerator:`LV_OBJ_FLAG_CHECKABLE` flag is
   enabled and the object is clicked. The event happens on transition
   to/from the checked state.

Learn more about :ref:`overview/events/events [0-1]`.

Keys
****

Note that the state of :cpp:enumerator:`LV_KEY_ENTER` is translated to
:cpp:enumerator:`LV_EVENT_PRESSED`, :cpp:enumerator:`LV_EVENT_PRESSING`
and :cpp:enumerator:`LV_EVENT_RELEASED` etc.

See the events of the :ref:`Base object <widgets/obj/base object (lv_obj) [0-1]>` too.

Learn more about :ref:`overview/indev/keys [0-3]`.

Example
*******

.. include:: ../examples/widgets/button/index.rst

API
***
