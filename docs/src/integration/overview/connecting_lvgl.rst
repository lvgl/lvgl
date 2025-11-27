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

.. image:: /_static/images/intro_data_flow.png
   :scale: 75 %
   :alt:  LVGL Data Flow
   :align:  center

There are two ways to provide this information to LVGL:

1.  Supply LVGL with a callback function to retrieve elapsed system milliseconds by
    calling :cpp:expr:`lv_tick_set_cb(my_get_milliseconds)`.
    :cpp:expr:`my_get_milliseconds()` needs to return the number of milliseconds
    elapsed since system start up.  Many platforms have built-in functions that can
    be used as they are.  For example:

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
        called from an interrupt if writing to a ``uint32_t`` value is atomic on your
        platform.  See below and the :ref:`threading` section to learn more.

Either way, the writing of the ``uint32_t`` Tick value must be atomic,
which is usually the case with a 32- or 64-bit platform.  If you are using a 16-bit
system (causing the update of the Tick value to not be atomic) and your platform uses
the Harvard instruction set, you can set a function like this as the callback passed
to :cpp:expr:`lv_tick_set_cb(my_get_milliseconds)`:

.. code-block:: c

    /**
     * @brief  Safe read from 'elapsed_power_on_time_in_ms'
     */
    uint32_t  my_get_milliseconds()
    {
        register uint32_t  u32result;
        /* Disable priority 1-6 interrupts for 2 Fcys. */
        __builtin_disi(2);
        u32result = elapsed_power_on_time_in_ms;   /* Cost: 2 Fcys */
            /* Generally looks like this in assembly:
             *     mov   elapsed_power_on_time_in_ms, W0
             *     mov   0x7898, W1
             * requiring exactly 2 clock cycles.
             * Now value is copied to register pair W0:W1
             * where it can be written to any destination. */
        return u32result;
    }


Reliability
-----------
Advancing the tick value should be done in such a way that its timing is reliable and
not dependent on anything that consumes an unknown amount of time. For an example of
what *not* to do: this can "seem" to work, but LVGL's timing will be incorrect
because the execution time of :cpp:func:`lv_timer_handler` varies from call to call and
thus the delay it introduces cannot be known.

.. code-block:: c

    // Bad idea
    lv_timer_handler();
    lv_tick_inc(5);
    my_delay_ms(5);



.. _display_interface:

Display Interface
*****************
LVGL needs to be supplied with knowledge about each display panel you want it to use.
Specifically:

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

.. API startswith:  lv_tick_
