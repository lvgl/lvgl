.. _lv_checkbox:

======================
Checkbox (lv_checkbox)
======================

Overview
********

The Checkbox object is created from a "tick box" and a label. When the
Checkbox is clicked the tick box is toggled.

.. _lv_checkbox_parts_and_styles:

Parts and Styles
****************

-  :cpp:enumerator:`LV_PART_MAIN` The is the background of the Checkbox and it uses
   the text and all the typical background style properties.
   ``pad_column`` adjusts the spacing between the tickbox and the label
-  :cpp:enumerator:`LV_PART_INDICATOR` The "tick box" is a square that uses all the
   typical background style properties. By default, its size is equal to
   the height of the main part's font. Padding properties make the tick
   box larger in the respective directions.

The Checkbox is added to the default group (if it is set).

.. _lv_checkbox_usage:

Usage
*****

Text
----

The text can be modified with the
:cpp:expr:`lv_checkbox_set_text(cb, "New text")` function and will be
dynamically allocated.

To set a static text, use :cpp:expr:`lv_checkbox_set_static_text(cb, txt)`. This
way, only a pointer to ``txt`` will be stored. The text then shouldn't
be deallocated while the checkbox exists.

Check, uncheck, disable
-----------------------

You can manually check, un-check, and disable the Checkbox by using the
common state add/clear function:

.. code:: c

   lv_obj_add_state(cb, LV_STATE_CHECKED);   /*Make the checkbox checked*/
   lv_obj_remove_state(cb, LV_STATE_CHECKED); /*Make the checkbox unchecked*/
   lv_obj_add_state(cb, LV_STATE_CHECKED | LV_STATE_DISABLED); /*Make the checkbox checked and disabled*/

To get whether the checkbox is checked or not use:
:cpp:expr:`lv_obj_has_state(cb, LV_STATE_CHECKED)`.

.. _lv_checkbox_events:

Events
******

-  :cpp:enumerator:`LV_EVENT_VALUE_CHANGED` Sent when the checkbox is toggled.

See the events of the :ref:`Base object <lv_obj>` too.

Learn more about :ref:`events`.

.. _lv_checkbox_keys:

Keys
****

The following *Keys* are processed by the 'Buttons': -
``LV_KEY_RIGHT/UP`` Go to toggled state if toggling is enabled -
``LV_KEY_LEFT/DOWN`` Go to non-toggled state if toggling is enabled -
:cpp:enumerator:`LV_KEY_ENTER` Clicks the checkbox and toggles it

Note that, as usual, the state of :cpp:enumerator:`LV_KEY_ENTER` is translated to
``LV_EVENT_PRESSED/PRESSING/RELEASED`` etc.

Learn more about :ref:`indev_keys`.

.. _lv_checkbox_example:

Example
*******

.. include:: ../examples/widgets/checkbox/index.rst

.. _lv_checkboxapi:

API
***
