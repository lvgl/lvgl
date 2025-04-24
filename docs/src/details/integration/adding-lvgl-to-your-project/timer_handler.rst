.. _timer_handler:

=============
Timer Handler
=============

To drive the timers of LVGL you need to call :cpp:func:`lv_timer_handler`
periodically in one of the following:

- *while(1)* of *main()* function, or
- an OS task periodically.  (See :ref:`lvgl_and_threads`.)

.. image:: /_static/images/intro_data_flow.png
   :scale: 75 %
   :alt:  LVGL Data Flow
   :align:  center

Example:

.. code-block:: c

   while(1) {
     uint32_t time_till_next = lv_timer_handler();
     if(time_till_next == LV_NO_TIMER_READY) time_till_next = LV_DEF_REFR_PERIOD; /*handle LV_NO_TIMER_READY. Another option is to `sleep` for longer*/
     my_delay_ms(time_till_next);
   }

If you want to use :cpp:func:`lv_timer_handler` in a super-loop, a helper
function :cpp:func:`lv_timer_handler_run_in_period` is provided to simplify
supplying LVGL with time awareness:

.. code-block:: c

   while(1) {
      ...
      lv_timer_handler_run_in_period(5); /* run lv_timer_handler() every 5ms */
      ...
   }

Or use the sleep time automatically calculated by LVGL:

.. code-block:: c

   while(1) {
     ...
     lv_timer_periodic_handler();
     ...
   }

In an OS environment, you can use it together with the **delay** or
**sleep** provided by OS to release CPU whenever possible:

.. code-block:: c

   while (1) {
      uint32_t time_till_next = lv_timer_handler();
      if(time_till_next == LV_NO_TIMER_READY) time_till_next = LV_DEF_REFR_PERIOD; /*handle LV_NO_TIMER_READY. Another option is to `sleep` for longer*/
      os_delay_ms(time_till_next); /* delay to avoid unnecessary polling */
   }

See :ref:`timer` section to learn more about timers.


.. _timer_handler_no_timer_ready:

When No Timers Are Ready
************************

:c:func:`lv_timer_handler` will return :c:macro:`LV_NO_TIMER_READY` (``UINT32_MAX``)
if there are no running timers. This can happen if there are no indevs or they are disabled with `lv_indev_enable()`, running
animations, or running user-created timers. :c:func:`lv_timer_handler` will continue
to return :c:macro:`LV_NO_TIMER_READY` until there is a running timer. Display
timers will stay paused when there is no reason to refresh.
:c:func:`lv_timer_handler` should be called after something is created, deleted, or
modified so that a refresh will be performed if necessary. In practice this means
waiting without a timeout for some external event. After the
external events are received and handled, :c:func:`lv_timer_handler` should be
called again.

.. code-block:: c

   while (1) {
      uint32_t time_till_next = lv_timer_handler();
      int timeout;

      /* Wait forever for events upon LV_NO_TIMER_READY, because there
       * is no reason to call lv_timer_handler sooner. */
      if(time_till_next == LV_NO_TIMER_READY) timeout = -1; /*infinite timeout*/

      /*Otherwise, wait for events at least until the timeout expires.*/
      else timeout = time_till_next;

      if(poll(..., timeout)) {
         /*Handle events before calling `lv_timer_handler` again.*/
      }
   }

If there is no external event source, you may choose to exit the loop or simply
delay for a long time.

If another :ref:`thread <threading>` is
calling LVGL functions, you may want to call :c:func:`lv_timer_handler` again
very soon to handle the effects of those other threads.


API
***
