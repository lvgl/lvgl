LED (lv_led)
============

Overview
********

The LEDs are rectangle-like (or circle) object whose brightness can be
adjusted. With lower brightness the colors of the LED become darker.

Parts and Styles
****************

The LEDs have only one main part, called :cpp:enumerator:`LV_LED_PART_MAIN` and it
uses all the typical background style properties.

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

Events
******

See the events of the `Base object </widgets/obj>`__ too.

Learn more about :ref:`events`.

Keys
****

No *Keys* are processed by the object type.

Learn more about :ref:`indev_keys`.

Example
*******

.. include:: ../examples/widgets/led/index.rst

API
***
