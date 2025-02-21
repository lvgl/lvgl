.. _threading:

========================
Threading Considerations
========================

.. _threading_definitions:

Definitions
***********

.. _thread:

Thread
    In the context of this document, a thread is any sequence of CPU instructions.
    In "bare-metal" implementations (i.e. no OS), threads include:

    - the main thread executing a while(1) loop that runs the system, and
    - interrupt service routines (ISRs).

    When running under an OS, threads include:

    - each task (or process),
    - ISRs, and
    - advanced OSes can have multiple "execution threads" within a processes.

.. _atomic operation:

Atomic Operation
    If operation X is atomic, that means that any thread observing the operation will
    see it either as not yet started, or as completed, and not in any state that is
    partially completed.

    If other threads can see the operation in a partially performed state, or
    interfere with it, then operation X is not atomic.

    If an atomic operation can fail, its implementation must return the the resource
    back to the state before the operation was started.  To other threads it must
    appear as though the operation had not yet started.

.. _atomic data:
.. _atomic:
.. _non-atomic data:

Atomic Data
    A datum (i.e. contents of a variable or data structure) is atomic if any thread
    observing it will always see it in a consistent state, as if operations on it
    have either not yet started, or have been successfully completed, and not in a
    state that is partially changed or otherwise inconsistent.

    When reading or writing a value is started and completed with 1 CPU instruction,
    it is automatically atomic, since it can never been seen in an inconsistent
    (partially-changed) state, even from a CPU interrupt or exception.  With such
    values, no special protection is required by programmers to ensure all threads
    see it in a consistent state.



.. _lvgl_and_threads:

LVGL and Threads
****************

LVGL is **not thread-safe**.

That means it is the programmer's responsibility to see that no LVGL function is
called while another LVGL call is in progress in another thread.  This includes calls
to :cpp:func:`lv_timer_handler`.

.. note::
    Assuming the above is the case, it is safe to call LVGL functions in

    - :ref:`event callbacks <events>`, and in
    - :ref:`timer callbacks <timer>`

    because the thread that drives both of these is the thread that calls
    :cpp:func:`lv_timer_handler`.

Reason:

LVGL manages many complex data structures, and those structures are "system
resources" that must be protected from being "seen" by other threads in an
inconsistent state.  A high percentage LVGL functions (functions that start with
``lv_``) either read from or change those data structures.  Those that change them
place the data in an inconsistent state during execution (because such changes are
multi-step sequences), but return them to a consistent state before those functions
return.  For this reason, execution of each LVGL function must be allowed to complete
before any other LVGL function is started.

.. _os_exception:

.. admonition:: Exceptions to the Above:

    These two LVGL functions may be called from any thread:

    - :cpp:func:`lv_tick_inc` (if writing to a ``uint32_t`` is atomic on your
      platform; see :ref:`tick_interface` for more information) and
    - :cpp:func:`lv_display_flush_ready` (:ref:`flush_callback` for more information)

    The reason this is okay is that the LVGL data changed by them is :ref:`atomic <atomic>`.

    If an interrupt MUST convey information to part of your application that calls
    LVGL functions, set a flag or other atomic value that your LVGL-calling thread
    (or an :ref:`LVGL Timer <timer>` you create) can read from and take action.

    If you are using an OS, there are a few other options.  See below.


Ensuring Time Updates are Atomic
--------------------------------
For LVGL's time-related tasks to be reliable, the time updates via the Tick Interface
must be reliable and the Tick Value must appear :ref:`atomic <atomic>` to LVGL.  See
:ref:`tick_interface` for details.



.. _tasks:

Tasks
*****
Under an OS, it is common to have many threads of execution ("tasks" in some OSes)
performing services for the application.  In some cases, such threads can acquire
data that should be shown (or otherwise reflected) in the user interface, and doing
so requires making LVGL calls to get that data (or change) shown.

Yet it still remains the programmer's responsibility to see that no LVGL function is
called while another LVGL call is in progress.

How do you do this?


.. _gateway thread:

Method 1:  Use a Gateway Thread
-------------------------------
A "Gateway Thread" (or "Gateway Task" in some OSes) is a thread (task) that the
system designer designates to *exclusively* manage a system resource.  An example is
management of a remote chip, such as an EEPROM or other device that always needs to
be brought into a consistent state before something new is started.  Another example
is management of multiple devices on an I2C bus (or any data bus).  In this case the
I2C bus is the "exclusively-managed resource", and having only one thread managing it
guarantees that each action started is allowed to complete before another action with
it is started.

LVGL's data structures are a system resource that requires such protection.

Using this method, creation, modification and deletion of all Widgets and other
LVGL resources (i.e. all LVGL function calls excluding the :ref:`exceptions
<os_exception>` mentioned above) are called by that thread.  That means
that thread is also the ONLY caller of :cpp:func:`lv_timer_handler`.  (See
:ref:`add_lvgl_to_your_project` for more information.)

This ensures LVGL's data structures "appear" atomic_ (all threads using this data
"see" it in a consistent state) by the fact that no other threads are "viewing" those
data structures.  This is enforced by programmer discipline that ensures the `Gateway
Thread`_ is the only thread making LVGL calls (excluding the :ref:`exceptions
<os_exception>` mentioned above).

If `atomic data`_ relevant to the user interface is updated in another thread (i.e.
by another task or in an interrupt), the thread calling LVGL functions can read that
data directly without worry that it is in an inconsistent state.  (To avoid
unnecessary CPU overhead, a mechanism can be provided [such as a flag raised by the
updating thread] so that the user interface is only updated when it will result in a
change visible to the end user.)

If `non-atomic data`_ relevant to the user interface is updated in another thread
(i.e. by another task or in an interrupt), an alternate (and safe) way of convey that
data to the thread calling LVGL functions is to pass a private copy of that data to
that thread via a QUEUE or other OS mechanism that protects that data from being seen
in an inconsistent state.

Use of a `Gateway Thread`_ avoids the CPU-overhead (and coding overhead) of using a
MUTEX to protect LVGL data structures.


Method 2:  Use a MUTEX
----------------------
A MUTEX stands for "MUTually EXclusive" and is a synchronization primitive that
protects the state of a system resource from being modified or accessed by multiple
threads of execution at once.  In other words, it makes data so protected "appear"
atomic (all threads using this data "see" it in a consistent state).  Most OSes
provide MUTEXes.

The system designer assigns a single MUTEX to product a single system resource.  Once
assigned, that MUTEX performs such protection by programmers:

1.  acquiring the MUTEX (a.k.a. locking it) before accessing or modifying that
    resource, and

2.  releasing the MUTEX (a.k.a. unlocking it) after that access or modification
    is complete.

If a thread attempts to acquire (lock) the MUTEX while another thread "owns" it,
that thread waits on the other thread to release (unlock) it before it is allowed
to continue execution.

To be clear:  this must be done *both* by threads that READ from that resource, and
threads that MODIFY that resource.

If a MUTEX is used to protect LVGL data structures, that means *every* LVGL function
call (or group of function calls) must be preceded by #1, and followed by #2,
including calls to :cpp:func:`lv_timer_handler`.

.. note::
    If your OS is integrated with LVGL (the macro :c:macro:`LV_USE_OS` has a value
    other than ``LV_OS_NONE`` in ``lv_conf.h``) you can use :cpp:func:`lv_lock()` and
    :cpp:func:`lv_unlock()` to perform #1 and #2.

    When this is the case, :cpp:func:`lv_timer_handler` calls :cpp:func:`lv_lock()`
    and :cpp:func:`lv_unlock()` internally, so you do not have to bracket your
    calls to :cpp:func:`lv_timer_handler` with them.

    If your OS is NOT integrated with LVGL, then these calls either return
    immediately with no effect, or are optimized away by the linker.

    To enable :cpp:func:`lv_lock()` and :cpp:func:`lv_unlock()`, set ``LV_USE_OS``
    to a value other than ``LV_OS_NONE``.

This pseudocode illustrates the concept of using a MUTEX:

.. code-block:: c

    void lvgl_thread(void)
    {
        while(1) {
            uint32_t time_till_next;
            time_till_next = lv_timer_handler(); /* lv_lock/lv_unlock is called internally */
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



