.. _lv_spinbox:

====================
Spinbox (lv_spinbox)
====================

Overview
********

The Spinbox contains a number as text which can be increased or
decreased by *Keys* or API functions. Under the hood the Spinbox is a
modified :ref:`Text area <lv_textarea>`.

.. _lv_spinbox_parts_and_styles:

Parts and Styles
****************

The parts of the Spinbox are identical to the :ref:`Text area <lv_textarea>`.

Value, range and step
---------------------

- :cpp:expr:`lv_spinbox_set_value(spinbox, 1234)` sets a new value on the Spinbox.
- :cpp:expr:`lv_spinbox_increment(spinbox)` and :cpp:expr:`lv_spinbox_decrement(spinbox)`
increments/decrements the value of the Spinbox according to the currently selected digit.
- :cpp:expr:`lv_spinbox_set_range(spinbox, -1000, 2500)` sets a range. If the
value is changed by :cpp:func:`lv_spinbox_set_value`, by
*Keys*,\ ``lv_spinbox_increment/decrement`` this range will be respected.
- :cpp:expr:`lv_spinbox_set_step(spinbox, 100)` sets which digits to change on
increment/decrement. Only multiples of ten can be set, and not for example 3.
- :cpp:expr:`lv_spinbox_set_cursor_pos(spinbox, 1)` sets the cursor to a specific
digit to change on increment/decrement. For example position '0' sets the cursor to the least significant digit.

If an encoder is used as input device, the selected digit is shifted to
the right by default whenever the encoder button is clicked. To change this behaviour to shifting
to the left, the :cpp:expr:`lv_spinbox_set_digit_step_direction(spinbox, LV_DIR_LEFT)` can be used

Format
------

:cpp:expr:`lv_spinbox_set_digit_format(spinbox, digit_count, separator_position)`
sets the number format. ``digit_count`` is the number of digits
excluding the decimal separator and the sign. ``separator_position`` is
the number of digits before the decimal point. If 0, no decimal point is displayed.

Rollover
--------

:cpp:expr:`lv_spinbox_set_rollover(spinbox, true/false)` enables/disabled
rollover mode. If either the minimum or maximum value is reached with
rollover enabled, the value will change to the other limit. If rollover
is disabled the value will remain at the minimum or maximum value.

.. _lv_spinbox_events:

Events
******

-  :cpp:enumerator:`LV_EVENT_VALUE_CHANGED` Sent when the value has changed.

See the events of the :ref:`Text area <lv_textarea>` too.

Learn more about :ref:`events`.

.. _lv_spinbox_keys:

Keys
****

-  ``LV_KEY_LEFT/RIGHT`` With *Keypad* move the cursor left/right. With
   *Encoder* decrement/increment the selected digit.
-  ``LV_KEY_UP/DOWN`` With *Keypad* and *Encoder* increment/decrement
   the value.
-  :cpp:enumerator:`LV_KEY_ENTER` With *Encoder* got the net digit. Jump to the first
   after the last.

.. _lv_spinbox_example:

Example
*******

.. include:: ../examples/widgets/spinbox/index.rst

.. _lv_spinbox_api:

API
***
