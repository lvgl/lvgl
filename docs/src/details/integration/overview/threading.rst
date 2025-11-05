.. _threading:

========================
Threading Considerations
========================

.. _lvgl_and_threads:

LVGL and Threads
****************

LVGL is **not thread-safe**.

That means it is the programmer's responsibility to see that no LVGL function is
called while another LVGL call is in progress in another thread.  This includes calls
to :cpp:func:`lv_timer_handler`.

Assuming the above is the case, it is safe to call LVGL functions in

- :ref:`event callbacks <events>`, and in
- :ref:`timer callbacks <timer>`

because the thread that drives both of these is the thread that calls
:cpp:func:`lv_timer_handler`.

**Reason:**

LVGL manages many complex data structures, and those structures are "system
resources" that must be protected from being "seen" by other threads in an
inconsistent state.  A high percentage LVGL functions (functions that start with
``lv_``) either read from or change those data structures.  Those that change them
place the data in an inconsistent state during call execution (because such changes are
multi-step sequences), but return them to a consistent state before those functions
return.  For this reason, execution of each LVGL function must be allowed to complete
before any other LVGL function is started.

**Exceptions to the Above:**

These two LVGL functions may be called from any thread:

- :cpp:func:`lv_tick_inc` (if writing to a ``uint32_t`` is atomic on your
  platform; see :ref:`tick_interface` for more information) and
- :cpp:func:`lv_display_flush_ready` (:ref:`flush_callback` for more information)

The reason this is okay is that the LVGL data changed by them is itself atomic.

If an interrupt MUST convey information to part of your application that calls
LVGL functions, set a flag or other atomic value that your LVGL-calling thread
(or an :ref:`LVGL Timer <timer>` you create) can read from and take action.

If you are using an OS, there are a few other options.  See below.


Ensuring Time Updates are Atomic
--------------------------------

For LVGL's time-related tasks to be reliable, the time updates via the Tick Interface
must be reliable and the Tick Value must appear atomic to LVGL.  See
:ref:`tick_interface` for details.


.. _gateway thread:

Common Thread Management Approaches
-----------------------------------

.. container:: tighter-table-3

    +----------------+------------------------+------------+
    | Approach       | Description            | Efficiency |
    +================+========================+============+
    | Gateway Thread | One RTOS Task handles  | most       |
    |                | all LVGL calls         |            |
    +----------------+------------------------+------------+
    | Work Queue     | Useful with OSes that  | less       |
    |                | host a user-accessible |            |
    |                | Work Queue             |            |
    +----------------+------------------------+------------+
    | MUTEX          | Synchronization        | least      |
    |                | Primitive provided     |            |
    |                | by OS                  |            |
    +----------------+------------------------+------------+

Notes on Using MUTEXES
----------------------

Use of MUTEXes requires:

1.  acquiring the MUTEX (locking it) before each LVGL call (or group of calls), and

2.  releasing the MUTEX (unlocking it) afterwards.

If your OS is integrated with LVGL (the macro :c:macro:`LV_USE_OS` has a value
other than ``LV_OS_NONE`` in ``lv_conf.h``) you can use :cpp:func:`lv_lock()` and
:cpp:func:`lv_unlock()` to perform #1 and #2.

When this is the case, :cpp:func:`lv_timer_handler` calls :cpp:func:`lv_lock()`
and :cpp:func:`lv_unlock()` internally, so you do not have to bracket your
calls to :cpp:func:`lv_timer_handler` with them.

If your OS is NOT integrated with LVGL, then these calls either return
immediately with no effect, or are optimized away by the linker.

This pseudocode illustrates the concept of using a MUTEX:

.. code-block:: c

    void lvgl_thread(void)
    {
        while(1) {
            uint32_t time_till_next;
            time_till_next = lv_timer_handler(); /* lv_lock/lv_unlock is called internally */
            if(time_till_next == LV_NO_TIMER_READY) time_till_next = LV_DEF_REFR_PERIOD; /*try again soon because the other thread can make the timer ready*/
            thread_sleep(time_till_next); /* sleep for a while */
        }
    }

    void other_thread(void)
    {
        /* You must always hold (lock) the MUTEX while calling LVGL functions. */
        lv_lock();
        lv_obj_t *img = lv_image_create(lv_screen_active());
        lv_unlock();

        while(1) {
            lv_lock();
            /* Change to next image. */
            lv_image_set_src(img, next_image);
            lv_unlock();
            thread_sleep(2000);
        }
    }



.. _sleep_management:

Sleep Management
****************

The MCU can go to sleep when no user input has been received for a certain period.
In this case, the main ``while(1)`` could look like this:

.. code-block:: c

    while(1) {
        /* Normal operation (no sleep) in < 1 sec inactivity */
        if(lv_display_get_inactive_time(NULL) < 1000) {
            lv_timer_handler();
        }
        /* Sleep after 1 sec inactivity */
        else {
            timer_stop();   /* Stop the timer where lv_tick_inc() is called */
            sleep();        /* Sleep the MCU */
        }
        my_delay_ms(5);
    }

You should also add the following lines to your input device read
function to signal a wake-up (press, touch, click, etc.) has happened:

.. code-block:: c

    lv_tick_inc(LV_DEF_REFR_PERIOD);  /* Force task execution on wake-up */
    timer_start();                    /* Restart timer where lv_tick_inc() is called */
    lv_timer_handler();               /* Call `lv_timer_handler()` manually to process the wake-up event */

In addition to :cpp:func:`lv_display_get_inactive_time` you can check
:cpp:func:`lv_anim_count_running` to see if all animations have finished.



