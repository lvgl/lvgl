.. _monkey:

======
Monkey
======

The Monkey module provides LVGL applications with a simple monkey test.  Monkey
Testing is a technique where the user tests the application or system by providing
random inputs and checking the behavior or seeing whether the application or system
will crash.  This module provides this service as simulated random input to stress
test an LVGL application.



.. _monkey_usage:

Usage
*****

First, enable :c:macro:`LV_USE_MONKEY` in ``lv_conf.h``.

Next, declare a variable (it can be local) of type :c:type:`lv_monkey_config_t` to
define the configuration structure, initialize it using
:cpp:expr:`lv_monkey_config_init(cfg)` then set its ``type`` member to the desired
type of :ref:`input device <indev>`, and set the ``min`` and ``max`` values for its
``period_range`` and ``input_range`` members to set the time ranges (in milliseconds)
and input ranges the Monkey module will use to generate random input at random times.

Next, call :cpp:expr:`lv_monkey_create(cfg)` to create the Monkey.  It returns a
pointer to the ``lv_monkey_t`` created.

Finally call :cpp:expr:`lv_monkey_set_enable(monkey, true)` to enable Monkey.

If you want to pause the monkey, call
:cpp:expr:`lv_monkey_set_enable(monkey, false)`. To delete it, call
:cpp:expr:`lv_monkey_delete(monkey)`.

Note that ``input_range`` has different meanings depending on the ``type`` input device:

- :cpp:enumerator:`LV_INDEV_TYPE_POINTER`: No effect, click randomly within the pixels of the screen resolution.
- :cpp:enumerator:`LV_INDEV_TYPE_ENCODER`: The minimum and maximum values of ``enc_diff``.
- :cpp:enumerator:`LV_INDEV_TYPE_BUTTON`: The minimum and maximum values of ``btn_id``.
  Use :cpp:func:`lv_monkey_get_indev` to get the input device, and use
  :cpp:func:`lv_indev_set_button_points` to map the key ID to the coordinates.
- :cpp:enumerator:`LV_INDEV_TYPE_KEYPAD`: No effect, Send random :ref:`indev_keys`.



.. _monkey_example:

Example
*******

.. include:: ../../examples/others/monkey/index.rst



.. _monkey_api:

API
***

