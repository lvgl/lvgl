.. _tick:

==============
Tick interface
==============

LVGL needs a system tick to know the elapsed time for animations and other
tasks.

There are two ways to provide the tick to LVGL:

1. Call ``lv_tick_set_cb(my_get_milliseconds_function);``: `my_get_milliseconds_function` needs to tell how many milliseconds have elapsed since start up. Most of the platforms have built-in functions that can be used as they are. For example

   - SDL: ``lv_tick_set_cb(SDL_GetTicks);``
   - Arduino: ``lv_tick_set_cb(my_tick_get_cb);``, where ``my_tick_get_cb`` is: ``static uint32_t my_tick_get_cb(void) { return millis(); }``
   - FreeRTOS: ``lv_tick_set_cb(xTaskGetTickCount);``
   - STM32: ``lv_tick_set_cb(HAL_GetTick);``
   - ESP32: ``lv_tick_set_cb(my_tick_get_cb);``, where ``my_tick_get_cb`` is a wrapper for ``esp_timer_get_time() / 1000;``

2. Call ``lv_tick_inc(x)`` periodically, where ``x`` is the elapsed milliseconds since the last call. ``lv_tick_inc`` should be called from a high priority interrupt.

The ticks (milliseconds)  should be independent from any other activities of the MCU.

For example this works, but LVGL's timing will be incorrect as the execution time of ``lv_timer_handler`` is not considered:

.. code:: c
   // Bad idea
   lv_timer_handler();
   lv_tick_inc(5);
   my_delay_ms(5);

API
---
