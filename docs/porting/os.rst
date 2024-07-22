.. _os_interrupt:

===============================
Operating system and interrupts
===============================

LVGL is **not thread-safe** by default.

However, in the following conditions it's valid to call LVGL related
functions:

- In *events*. Learn more in :ref:`events`.
- In *lv_timer*. Learn more in :ref:`timer`.

Tasks and threads
-----------------

If you need to use real tasks or threads, you need a mutex which should
be invoked before the call of :cpp:func:`lv_timer_handler` and released after
it. Also, you have to use the same mutex in other tasks and threads
around every LVGL (``lv_...``) related function call and code. This way
you can use LVGL in a real multitasking environment. Just make use of a
mutex to avoid the concurrent calling of LVGL functions.

LVGL has a built-in mutex which can be used with:
- :cpp:func:`lv_lock()` and :cpp:func:`lv_lock_isr()`
- :cpp:func:`lv_unlock()`

These functions are called internally in :cpp:func:`lv_timer_handler`
and the users need to call them only from their own threads.

To enable ``lv_lock/lv_unlock`` ``LV_USE_OS`` needs to be set to other
than ``LV_OS_NONE``.


Here is some pseudocode to illustrate the concept:

.. code:: c

   void lvgl_thread(void)
   {
       while(1) {
           uint32_t time_till_next;
           time_till_next = lv_timer_handler(); /*lv_lock/lv_unlock is called internally*/
           thread_sleep(time_till_next); /* sleep for a while */
       }
   }

   void other_thread(void)
   {
       /* You must always hold the mutex while using LVGL APIs */
       lv_lock();
       lv_obj_t *img = lv_image_create(lv_screen_active());
       lv_unlock();

       while(1) {
           lv_lock();
           /* change to the next image */
           lv_image_set_src(img, next_image);
           lv_unlock();
           thread_sleep(2000);
       }
   }

Interrupts
----------

Try to avoid calling LVGL functions from interrupt handlers (except
:cpp:func:`lv_tick_inc` and :cpp:func:`lv_display_flush_ready`). But if you need to do
this you have to disable the interrupt which uses LVGL functions while
:cpp:func:`lv_timer_handler` is running.

It's a better approach to simply set a flag or some value in the
interrupt, and periodically check it in an LVGL timer (which is run by
:cpp:func:`lv_timer_handler`).
