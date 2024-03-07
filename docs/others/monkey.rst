.. _monkey:

======
Monkey
======

A simple monkey test. Use random input to stress test the application.

.. _monkey_usage:

Usage
-----

Enable :c:macro:`LV_USE_MONKEY` in ``lv_conf.h``.

First configure monkey, use :c:struct:`lv_monkey_config_t` to define the
configuration structure, set the ``type`` (check `input devices </overview/indev>`__ for the supported types), and then set the
range of ``period_range`` and ``input_range``, the monkey will output
random operations at random times within this range. Call
:cpp:func:`lv_monkey_create` to create monkey. Finally call
:cpp:expr:`lv_monkey_set_enable(monkey, true)` to enable monkey.

If you want to pause the monkey, call
:cpp:expr:`lv_monkey_set_enable(monkey, false)`. To delete the monkey, call
:cpp:expr:`lv_monkey_delete(monkey)`.

Note that ``input_range`` has different meanings in different ``type``:

- :cpp:enumerator:`LV_INDEV_TYPE_POINTER`: No effect, click randomly within the pixels of the screen resolution.
- :cpp:enumerator:`LV_INDEV_TYPE_ENCODER`: The minimum and maximum values of ``enc_diff``.
- :cpp:enumerator:`LV_INDEV_TYPE_BUTTON`: The minimum and maximum values of ``btn_id``.
  Use :cpp:func:`lv_monkey_get_indev` to get the input device, and use
  :cpp:func:`lv_indev_set_button_points` to map the key ID to the coordinates.
- :cpp:enumerator:`LV_INDEV_TYPE_KEYPAD`: No effect, Send random :ref:`indev_keys`.

.. _monkey_example:

Example
-------

.. include:: ../examples/others/monkey/index.rst

.. _monkey_api:

API
---

