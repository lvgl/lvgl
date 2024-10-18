.. _tick:

==============
Tick Interface
==============

LVGL needs a awareness of elapsed time (in milliseconds) for animations and other tasks.

.. image:: /misc/intro_data_flow.png
   :scale: 75 %
   :alt:  LVGL Data Flow
   :align:  center

There are two ways to provide this information to LVGL:

1.  Supply LVGL with a callback function to retrieve elapsed system milliseconds by
    calling :cpp:expr:`lv_tick_set_cb(my_get_milliseconds_function)`.
    :cpp:expr:`my_get_milliseconds_function()` needs to return the number of
    milliseconds elapsed since system start up.  Many platforms have built-in
    functions that can be used as they are.  For example:

    - SDL:  ``lv_tick_set_cb(SDL_GetTicks);``
    - Arduino:  ``lv_tick_set_cb(my_tick_get_cb);``, where ``my_tick_get_cb`` is:
      ``static uint32_t my_tick_get_cb(void) { return millis(); }``
    - FreeRTOS:  ``lv_tick_set_cb(xTaskGetTickCount);``
    - STM32:  ``lv_tick_set_cb(HAL_GetTick);``
    - ESP32:  ``lv_tick_set_cb(my_tick_get_cb);``, where ``my_tick_get_cb`` is a
      wrapper for ``esp_timer_get_time() / 1000;``

2.  Call :cpp:expr:`lv_tick_inc(x)` periodically, where ``x`` is the elapsed
    milliseconds since the last call.  If :cpp:func:`lv_tick_inc` is called from an
    ISR, it should be from either a high priority interrupt or an interrupt that
    cannot be missed when the system is under high load.

    .. note::  :cpp:func:`lv_tick_inc` is only one of two LVGL functions that may be
        called from an ISR.  See the :ref:`os` section to learn more.

The ticks (milliseconds) should be independent from any other activities of the MCU.

For example this works, but LVGL's timing will be incorrect as the execution time of
:c:func:`lv_timer_handler` is not considered:

.. code-block:: c

    // Bad idea
    lv_timer_handler();
    lv_tick_inc(5);
    my_delay_ms(5);


API
***
