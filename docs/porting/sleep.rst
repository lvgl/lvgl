================
Sleep management
================

The MCU can go to sleep when no user input happens. In this case, the
main ``while(1)`` should look like this:

.. code:: c

   while(1) {
     /*Normal operation (no sleep) in < 1 sec inactivity*/
     if(lv_display_get_inactive_time(NULL) < 1000) {
         lv_timer_handler();
     }
     /*Sleep after 1 sec inactivity*/
     else {
         timer_stop();   /*Stop the timer where lv_tick_inc() is called*/
         sleep();        /*Sleep the MCU*/
     }
     my_delay_ms(5);
   }

You should also add the following lines to your input device read
function to signal a wake-up (press, touch or click etc.) has happened:

.. code:: c

   lv_tick_inc(LV_DEF_REFR_PERIOD); /*Force task execution on wake-up*/
   timer_start();                   /*Restart the timer where lv_tick_inc() is called*/
   lv_timer_handler();               /*Call `lv_timer_handler()` manually to process the wake-up event*/

In addition to :cpp:func:`lv_display_get_inactive_time` you can check
:cpp:func:`lv_anim_count_running` to see if all animations have finished.
