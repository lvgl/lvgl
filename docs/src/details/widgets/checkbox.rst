.. _lv_checkbox:

======================
Checkbox (lv_checkbox)
======================

Overview
********

The Checkbox Widget is created from a "tick box" and a label. When the
Checkbox is clicked the tick box is toggled.


.. _lv_checkbox_parts_and_styles:

Parts and Styles
****************

-  :cpp:enumerator:`LV_PART_MAIN` Background of Checkbox and it uses
   the text and the :ref:`typical background style properties <typical bg props>`.
   ``pad_column`` adjusts spacing between tickbox and label
-  :cpp:enumerator:`LV_PART_INDICATOR` The "tick box" is a square that uses the
   :ref:`typical background style properties <typical bg props>`.  By default, its
   size is equal to the height of the main part's font. Padding properties make the
   tick box larger in the respective directions.

The Checkbox is added to the default group (if one is set).


.. _lv_checkbox_usage:

Usage
*****

Text
----

The text can be modified with
:cpp:expr:`lv_checkbox_set_text(cb, "New text")` and will be
dynamically allocated.

To set static text, use :cpp:expr:`lv_checkbox_set_text_static(cb, txt)`. This
way, only a pointer to ``txt`` will be stored. The provided text buffer must remain
available for the lifetime of the Checkbox.

Check, uncheck, disable
-----------------------

You can programmatically check, un-check, and disable the Checkbox by using the
common state add/clear function:

.. code-block:: c

   lv_obj_add_state(cb, LV_STATE_CHECKED);    /* Make Checkbox checked */
   lv_obj_remove_state(cb, LV_STATE_CHECKED); /* Make Checkbox unchecked */
   lv_obj_add_state(cb, LV_STATE_CHECKED);    /* Make Checkbox checked */
   lv_obj_add_state(cb, LV_STATE_DISABLED);   /* Make Checkbox disabled */

To find out whether the Checkbox is checked use
:cpp:expr:`lv_obj_has_state(cb, LV_STATE_CHECKED)`.



.. _lv_checkbox_events:

Events
******

-  :cpp:enumerator:`LV_EVENT_VALUE_CHANGED` Sent when Checkbox is toggled.

.. admonition::  Further Reading

    Learn more about :ref:`lv_obj_events` emitted by all Widgets.

    Learn more about :ref:`events`.



.. _lv_checkbox_keys:

Keys
****

The following *Keys* are processed by Checkbox:

- ``LV_KEY_RIGHT/UP`` Go to CHECKED state if Checkbox is enabled
- ``LV_KEY_LEFT/DOWN`` Go to non-CHECKED state if Checkbox is enabled
- :cpp:enumerator:`LV_KEY_ENTER` Clicks the Checkbox and toggles its value.

Note that, as usual, the state of :cpp:enumerator:`LV_KEY_ENTER` is translated to
``LV_EVENT_PRESSED/PRESSING/RELEASED`` etc.

.. admonition::  Further Reading

    Learn more about :ref:`indev_keys`.



.. _lv_checkbox_example:

Example
*******

.. include:: ../../examples/widgets/checkbox/index.rst



.. _lv_checkboxapi:

API
***
