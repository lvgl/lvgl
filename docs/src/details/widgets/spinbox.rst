.. _lv_spinbox:

====================
Spinbox (lv_spinbox)
====================


Overview
********

Spinbox contains an integer displayed as a decimal number with a possible fixed
decimal point position and a configurable number of digits.  The value can be
increased or decreased by *Keys* or API functions.  Under the hood Spinbox is a
:ref:`lv_textarea` with behaviors extended to enable a numeric value
to be viewed and modified with configurable constraints.



.. _lv_spinbox_parts_and_styles:

Parts and Styles
****************

Spinbox's parts are identical to those of :ref:`Text Area <lv_textarea_parts_and_styles>`.

Value, range and step
---------------------

- :cpp:expr:`lv_spinbox_set_value(spinbox, 1234)` sets a new value for the Spinbox.
- :cpp:expr:`lv_spinbox_increment(spinbox)` and :cpp:expr:`lv_spinbox_decrement(spinbox)`
  increments/decrements the value of the Spinbox according to the currently-selected digit.
- :cpp:expr:`lv_spinbox_set_range(spinbox, -1000, 2500)` sets its range. If the
  value is changed by :cpp:expr:`lv_spinbox_set_value(spinbox)`, by *Keys*,
  by :cpp:expr:`lv_spinbox_increment(spinbox)` or :cpp:expr:`lv_spinbox_decrement(spinbox)`
  this range will be respected.
- :cpp:expr:`lv_spinbox_set_step(spinbox, 100)` sets which digit to change on
  increment/decrement. Only multiples of ten can be set.
- :cpp:expr:`lv_spinbox_set_cursor_pos(spinbox, 1)` sets the cursor to a specific
  digit to change on increment/decrement. Position '0' sets the cursor to
  the least significant digit.

If an encoder is used as input device, the selected digit is shifted to
the right by default whenever the encoder button is clicked. To change this behavior to shifting
to the left, the :cpp:expr:`lv_spinbox_set_digit_step_direction(spinbox, LV_DIR_LEFT)` can be used.

Format
------

:cpp:expr:`lv_spinbox_set_digit_format(spinbox, digit_count, separator_position)`
sets the number format. ``digit_count`` is the total number of digits to display.
``separator_position`` is the number of leading digits before the decimal point.
Pass 0 for ``separator_position`` to display no decimal point.

Rollover
--------

:cpp:expr:`lv_spinbox_set_rollover(spinbox, true/false)` enables/disables
rollover mode. If either the minimum or maximum value is reached with
rollover enabled, and the user attempts to continue changing the value in
the same direction, the value will change to the other limit.  If rollover
is disabled the value will stop at the minimum or maximum value.



.. _lv_spinbox_events:

Events
******

- :cpp:enumerator:`LV_EVENT_VALUE_CHANGED` Sent when value has changed.

.. admonition::  Further Reading

    :ref:`Textarea Events <lv_textarea_events>`.

    Learn more about :ref:`lv_obj_events` emitted by all Widgets.

    Learn more about :ref:`events`.


Data binding
------------

To get familiar with observers, subjects, and data bindings in general visit the
:ref:`Observer <observer_how_to_use>` page.

This method of subscribing to an integer Subject affects a Subject Widget's integer
value directly.  Note that this is a two-way binding (Subject <===> Widget), so an end
user's direct interaction with the Spinbox Widget updates the Subject's value and vice
versa.

It supports integer subjects.


- :cpp:expr:`lv_spinbox_bind_value(spinbox, &subject)`


.. _lv_spinbox_keys:

Keys
****

- ``LV_KEY_LEFT/RIGHT`` With *Keypad* move the cursor left/right. With
  *Encoder* decrement/increment the selected digit.
- ``LV_KEY_UP/DOWN`` With *Keypad* and *Encoder* increment/decrement
  the value.
- :cpp:enumerator:`LV_KEY_ENTER` With *Encoder*, move focus to next digit.  If focus
  is on last digit, focus moves to first digit.

.. admonition::  Further Reading

    Learn more about :ref:`indev_keys`.



.. _lv_spinbox_example:

Example
*******

.. include:: ../../examples/widgets/spinbox/index.rst



.. _lv_spinbox_api:

API
***
