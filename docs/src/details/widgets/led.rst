.. _lv_led:

============
LED (lv_led)
============

Overview
********

LEDs are rectangle-like (or circle) Widgets whose brightness can be
adjusted. With lower brightness the color of the LED becomes darker.

.. _lv_led_parts_and_styles:

Parts and Styles
****************

- :cpp:enumerator:`LV_LED_PART_MAIN` uses the :ref:`typical background style
  properties <typical bg props>`.

.. _lv_led_usage:

Usage
*****

Color
-----

You set the color of the LED with
:cpp:expr:`lv_led_set_color(led, lv_color_hex(0xff0080))`. This will be used as its
background color, border color, and shadow color.

Brightness
----------

You can set their brightness with :cpp:expr:`lv_led_set_brightness(led, brightness)`.
The ``brightness`` value should be in the range 0 (darkest) to 255 (lightest).

Toggle
------

Use :cpp:expr:`lv_led_on(led)` and :cpp:expr:`lv_led_off(led)` to set the brightness to
a predefined ON or OFF value. The :cpp:expr:`lv_led_toggle(led)` toggles between
the ON and OFF state.

You can set custom LED ON and OFF brightness values by defining macros
``LV_LED_BRIGHT_MAX`` and ``LV_LED_BRIGHT_MIN`` in your project.  Their default
values are 80 and 255. These too must be in the range [0..255].



.. _lv_led_events:

Events
******

No special events are sent by LED Widgets.

.. admonition::  Further Reading

    Learn more about :ref:`lv_obj_events` emitted by all Widgets.

    Learn more about :ref:`events`.



.. _lv_led_keys:

Keys
****

No *Keys* are processed by LED Widgets.

.. admonition::  Further Reading

    Learn more about :ref:`indev_keys`.



.. _lv_led_example:

Example
*******

.. include:: ../../examples/widgets/led/index.rst



.. _lv_led_api:

API
***
