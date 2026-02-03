.. _timer:

================
Timer (lv_timer)
================

LVGL has a built-in Timer system. You can register a function to have it
be called periodically. The Timers are handled and called in
:cpp:func:`lv_timer_handler`, which needs to be called every few milliseconds.
See :ref:`timer_handler` for more information.

By default, LVGL itself uses Timers to:

- refresh each display --- during the creation of each :ref:`Display`, a Timer is
  created for that Display.  That Timer refreshes the display based on the configured
  value of :c:macro:`LV_DEF_REFR_PERIOD`, and also sends all display-related events,
  like :cpp:enumerator:`LV_EVENT_REFR_START`, :cpp:enumerator:`LV_EVENT_REFR_READY`,
  etc.
- read input devices --- during the creation of each :ref:`indev`, a Timer is
  created for that Input Device based on the configured value of
  :c:macro:`LV_DEF_REFR_PERIOD`.  That Timer causes that input device to be read and
  also sends all input-device-related events, like :cpp:enumerator:`LV_EVENT_CLICKED`,
  :cpp:enumerator:`LV_EVENT_PRESSED`, etc.
- update system-monitor values --- if :c:macro:`LV_USE_SYSMON` is set to ``1`` in
  ``lv_conf.h``, one or more timers are created to periodically compute and
  monitor system performance statistics and LVGL's memory usage.

Timers are non-preemptive, which means a Timer cannot interrupt another
Timer. Therefore, you can call any LVGL-related function in a Timer.



Creating a Timer
****************

To create a new Timer, use
:cpp:expr:`lv_timer_create(timer_cb, period_ms, user_data)`. It returns an
:cpp:type:`lv_timer_t` ``*`` which can be used later to modify the
parameters of the Timer, pause it, or delete it when it is no longer needed.
:cpp:func:`lv_timer_create_basic` can also be used to create a new Timer without
specifying any parameters.

A Timer callback should have this prototype:  ``void (*lv_timer_cb_t)(lv_timer_t *)``.

For example:

.. code-block:: c

   void my_timer(lv_timer_t * timer)
   {
     /* Use the user_data */
     uint32_t * user_data = lv_timer_get_user_data(timer);
     printf("my_timer called with user data: %d\n", *user_data);

     /* Do something with LVGL */
     if(something_happened) {
       something_happened = false;
       lv_button_create(lv_screen_active());
     }
   }

   ...

   static uint32_t user_data = 10;
   lv_timer_t * timer = lv_timer_create(my_timer, 500,  &user_data);



Ready and Reset
***************

:cpp:expr:`lv_timer_ready(timer)` makes a Timer run on the next call of
:cpp:func:`lv_timer_handler`.

:cpp:expr:`lv_timer_reset(timer)` resets the period of a Timer. It will be
called again after its currently-set period (in milliseconds) has elapsed.

See what happens when :ref:`no timers are ready <timer_handler_no_timer_ready>`
and :cpp:func:`lv_timer_handler` is called.



Setting Parameters
******************

You can modify these Timer parameters at any time during its life:

- :cpp:expr:`lv_timer_set_cb(timer, new_cb)`
- :cpp:expr:`lv_timer_set_period(timer, new_period_ms)`
- :cpp:expr:`lv_timer_set_user_data(timer, user_data)`



Repeat Count
************

When a Timer is created, its repeat-count is set to ``-1`` to cause it to repeat
indefinitely.  You can make a Timer repeat only a given number of times with
:cpp:expr:`lv_timer_set_repeat_count(timer, count)`.  By default, once the Timer has
run ``count`` times, it will be automatically deleted.

You can use :cpp:expr:`lv_timer_set_auto_delete(timer, false)` if you want the timer
to instead be paused after it has run ``count`` times.  This can be handy if you
reuse that timer repeatedly and want to avoid the CPU and :cpp:func:`lv_malloc`
overhead of repeatedly creating and deleting a timer.  If you use this option, you
will need to set its repeat count (to either ``-1`` or a positive repeat count, since
it will have decremented to ``0``) and :ref:`resume <timer_pause_and_resume>` it to
make it active again.



.. _timer_pause_and_resume:

Pause and Resume
****************

:cpp:expr:`lv_timer_pause(timer)` pauses the specified Timer.

:cpp:expr:`lv_timer_resume(timer)` resumes the specified Timer.



Measuring Idle Time
*******************

You can get the idle percentage time of :cpp:func:`lv_timer_handler` with
:cpp:func:`lv_timer_get_idle`. Note that it does not measure the idle time of
the overall system, only of :cpp:func:`lv_timer_handler`.  This can be misleading if
you are using an operating system and DMA and/or GPU are used during rendering, as it
does not actually measure the time the OS spends in an idle thread.

If you are using an OS and wish to get the time the CPU is spending in an idle
thread, one way of doing so is configuring :c:macro:`LV_USE_SYSMON` and
:c:macro:`LV_USE_PERF_MONITOR` to ``1`` in ``lv_conf.h`` (if they are not already),
and setting the macro :c:macro:`LV_SYSMON_GET_IDLE` to the name of a function that
fetches the percent of CPU time spent in the OS's idle thread.  An example of such
a function is :cpp:func:`lv_os_get_idle_percent` in ``lv_freertos.c``.  While the
configuration is set this way, some system performance statistics (including CPU
load) will appear on the display in a partially-transparent label whose location is
set by the :c:macro:`LV_USE_PERF_MONITOR_POS` macro.



Enable and Disable
******************

You can temporarily disable Timer handling with :cpp:expr:`lv_timer_enable(false)`.
Be advised:  this also pauses handling of Timers that refresh Display(s) and read
from input devices, so don't forget to re-enable it with
:cpp:expr:`lv_timer_enable(true)` as soon as the need for the pause is over.



Timer Handler Resume Callback
*****************************

When the Timer system has been disabled (causing :cpp:func:`lv_timer_handler` to
return early before it has processed any timers), if you want to take some action
when the Timer system is re-enabled again, set a resume callback using
:cpp:expr:`lv_timer_handler_set_resume_cb(cb, user_data)`. The callback should have
this prototype:  ``void (*lv_timer_handler_resume_cb_t)(void*)``.



Asynchronous calls
******************

There are several cases in which you may not want to perform an action immediately.
Some examples are:

- you cannot delete a Widget because something else is still using it,
- you don't want to block execution now, or
- you detect the need to delete a Widget in a thread other than the thread making
  LVGL calls (e.g. in a case where you are using a :ref:`Gateway Thread <Gateway
  Thread>` to make all LVGL calls in a multi-threaded environment).

For these cases,
:cpp:expr:`lv_async_call(my_function, data_p)` can be used to call ``my_function`` on
the next invocation of :cpp:func:`lv_timer_handler`.  As a side effect, this also
ensures it is called in a thread in which it is safe to make LVGL calls.
``data_p`` will be passed to the function when it's called. Note that only the data's
pointer is saved, so whatever it is pointing to needs to remain valid until the
function is called, so it can point to ``static``, global or dynamically allocated
data. If you want to cancel an asynchronous call, call
:cpp:expr:`lv_async_call_cancel(my_function, data_p)`, which will remove all
asynchronous calls matching ``my_function`` and ``data_p``.

Note that if :cpp:expr:`lv_async_call(my_function, data_p)` is called from a thread
other than the one that normally makes LVGL calls, you are still obligated to protect
the LVGL data structure using a MUTEX.

For example:

.. code-block:: c

   void my_screen_clean_up(void * scr)
   {
     /* Free some resources related to `scr`*/

     /* Finally delete the screen */
     lv_obj_delete(scr);
   }

   ...

   /* Do something with the Widget on the current screen */

   /* Delete screen on next call of `lv_timer_handler`, not right now. */
   lv_lock();
   lv_async_call(my_screen_clean_up, lv_screen_active());
   lv_unlock();

   /* The screen is still valid so you can do other things with it */

If you just want to delete a Widget and don't need to clean anything up
in ``my_screen_cleanup`` you could just use :cpp:expr:`lv_obj_delete_async(widget)` which
will delete the Widget on the next call to :cpp:func:`lv_timer_handler`.



.. _timer_api:

API
***
