.. _tick:

==============
Tick interface
==============

LVGL needs a system tick to know elapsed time for animations and other
tasks.

If you want to use a custom function to :cpp:func:`lv_tick_get`, you can
register a "tick_get_cb" with :cpp:func:`lv_tick_set_cb`.

For example:

.. code:: c

   lv_tick_set_cb(SDL_GetTicks);


You need to call the :cpp:expr:`lv_tick_inc(tick_period)` function periodically
and provide the call period in milliseconds. For example,
:cpp:expr:`lv_tick_inc(1)` when calling every millisecond.

:cpp:func:`lv_tick_inc` should be called in a higher priority routine than
:cpp:func:`lv_task_handler` (e.g. in an interrupt) to precisely know the
elapsed milliseconds even if the execution of :cpp:func:`lv_task_handler` takes
more time.

With FreeRTOS :cpp:func:`lv_tick_inc` can be called in ``vApplicationTickHook``.

On Linux based operating systems (e.g. on Raspberry Pi) :cpp:func:`lv_tick_inc`
can be called in a thread like below:

.. code:: c

   void * tick_thread (void *args)
   {
         while(1) {
           usleep(5*1000);   /*Sleep for 5 millisecond*/
           lv_tick_inc(5);      /*Tell LVGL that 5 milliseconds were elapsed*/
       }
   }

API
---
