.. _connecting_lvgl:

================================
Connecting LVGL to Your Hardware
================================


.. _initializing_lvgl:

Initializing LVGL
*****************
After you have:

- :ref:`acquired LVGL <getting_lvgl>`,
- added the appropriate LVGL files to your project, and
- :ref:`created a lv_conf.h file <lv_conf>` for your project,

you will need to complete a few more steps to get your project up and running with LVGL.

1.  Initialize LVGL once early during system execution by calling :cpp:func:`lv_init`.
    This needs to be done before making any other LVGL calls.

2.  Initialize your drivers.

3.  Connect the :ref:`tick_interface`.

4.  Connect the :ref:`display_interface`.

5.  Connect the :ref:`indev_interface`.

6.  Drive LVGL time-related tasks by calling :cpp:func:`lv_timer_handler` every few
    milliseconds to manage LVGL timers.  See :ref:`timer_handler` for different ways
    to do this.

7.  Optionally set a theme with :cpp:func:`lv_display_set_theme`.

8.  Thereafter #include "lvgl/lvgl.h" in source files wherever you need to use LVGL
    functions.



.. _tick_interface:

Tick Interface
**************

LVGL needs awareness of what time it is (i.e. elapsed time in milliseconds) for
all of its tasks for which time is a factor:  refreshing displays, reading user
input, firing events, animations, etc.

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
        called from an interrupt.  See the :ref:`threading` section to learn more.

The ticks (milliseconds) should be independent from any other activities of the MCU.

For example this works, but LVGL's timing will be incorrect as the execution time of
:c:func:`lv_timer_handler` is not considered:

.. code-block:: c

    // Bad idea
    lv_timer_handler();
    lv_tick_inc(5);
    my_delay_ms(5);



.. _display_interface:

Display Interface
*****************
LVGL needs to be supplied with knowledge about each display panel you want it to use.
Specificially:

- its pixel format and size (:ref:`creating_a_display`),
- where to render pixels for it (:ref:`draw_buffers`), and
- how to send those rendered pixels to it (:ref:`flush_callback`).

See the respective links for how to supply LVGL with this knowledge.



.. _indev_interface:

Input-Device Interface
**********************
LVGL needs to know how to get input from all user-input devices that will be used in
your project.  LVGL supports a wide variety of user-input devices:

- touch-screens,
- touch-pads,
- mice,
- crowns,
- encoders,
- keypads,
- keyboards,
- etc.

See :ref:`indev_creation` to see how to do this.



API
***
:ref:`lv_tick_h`
