.. _timer:

=============
Timer Handler
=============

To handle the tasks of LVGL you need to call :cpp:func:`lv_timer_handler`
periodically in one of the following:

- *while(1)* of *main()* function
- timer interrupt periodically (lower priority than :cpp:func:`lv_tick_inc`)
- an OS task periodically

Example:

.. code:: c

   while(1) {
     uint32_t time_till_next = lv_timer_handler();
     my_delay_ms(time_till_next);
   }

If you want to use :cpp:func:`lv_timer_handler` in a super-loop, a helper
function :cpp:func:`lv_timer_handler_run_in_period` is provided to simplify
the porting:

.. code:: c

   while(1) {
      ...
      lv_timer_handler_run_in_period(5); /* run lv_timer_handler() every 5ms */
      ...
   }

Or use the sleep time automatically calculated by LVGL:

.. code:: c

   while(1) {
     ...
     lv_timer_periodic_handler();
     ...
   }

In an OS environment, you can use it together with the **delay** or
**sleep** provided by OS to release CPU whenever possible:

.. code:: c

   while (1) {
      uint32_t time_till_next = lv_timer_handler(); 
      os_delay_ms(time_till_next); /* delay to avoid unnecessary polling */
   }

To learn more about timers visit the :ref:`timer`
section.

API
***
