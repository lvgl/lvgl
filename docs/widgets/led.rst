.. _lv_led:

============
LED (lv_led)
============

Overview
********

The LEDs are rectangle-like (or circle) object whose brightness can be
adjusted. With lower brightness the colors of the LED become darker.

.. _lv_led_parts_and_styles:

Parts and Styles
****************

-  :cpp:enumerator:`LV_LED_PART_MAIN` uses all the typical background style properties.

.. _lv_led_usage:

Usage
*****

Color
-----

You can set the color of the LED with
:cpp:expr:`lv_led_set_color(led, lv_color_hex(0xff0080))`. This will be used as
background color, border color, and shadow color.

Brightness
----------

You can set their brightness with :cpp:expr:`lv_led_set_bright(led, bright)`.
The brightness should be between 0 (darkest) and 255 (lightest).

Toggle
------

Use :cpp:expr:`lv_led_on(led)` and :cpp:expr:`lv_led_off(led)` to set the brightness to
a predefined ON or OFF value. The :cpp:expr:`lv_led_toggle(led)` toggles between
the ON and OFF state.

.. _lv_led_events:

Events
******

See the events of the :ref:`Base object <lv_obj>` too.

Learn more about :ref:`events`.

.. _lv_led_keys:

Keys
****

No *Keys* are processed by the object type.

Learn more about :ref:`indev_keys`.

.. _lv_led_example:

Example
*******

.. include:: ../examples/widgets/led/index.rst

.. _lv_led_api:

API
***
