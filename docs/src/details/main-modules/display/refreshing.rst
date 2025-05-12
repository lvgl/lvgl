.. _display_refreshing:

==========
Refreshing
==========

Default Refresh Behavior
************************

Normally the dirty (a.k.a invalid) areas are checked and redrawn in
every :c:macro:`LV_DEF_REFR_PERIOD` milliseconds (set in ``lv_conf.h``).
This happens as a result of a refresh :ref:`timer` created that gets created when
the display is created, and is executed at that interval.



.. _display_decoupling_refresh_timer:

Decoupling the Display Refresh Timer
************************************

However, in some cases you might need more control on when display
refreshing happens, for example:

- to synchronize rendering with VSYNC or the TE signal;

- to time display refreshes immediately after a single screen update of all widgets
  that needed to have their display data updated (i.e. only updated once immediately
  before display refresh to reduce CPU overhead).

You can do this in the following way:

.. code-block:: c

   /* Delete original display refresh timer */
   lv_display_delete_refr_timer(display1);

   /* Call this to refresh dirty (changed) areas of the display. */
   lv_display_refr_timer(NULL);

If you have multiple displays call :cpp:expr:`lv_display_set_default(display1)` to
select the display to refresh before :cpp:expr:`lv_display_refr_timer(NULL)`.


.. note:: :cpp:func:`lv_timer_handler` and :cpp:func:`lv_display_refr_timer` must not run at the same time.


If the performance monitor is enabled, the value of :c:macro:`LV_DEF_REFR_PERIOD` needs to be set to be
consistent with the refresh period of the display to ensure that the statistical results are correct.



.. _display_force_refresh:

Forcing a Refresh
*****************

Normally the invalidated areas (marked for redrawing) are rendered in
:cpp:func:`lv_timer_handler` in every :c:macro:`LV_DEF_REFR_PERIOD` milliseconds.
However, by using :cpp:expr:`lv_refr_now(display)` you can tell LVGL to redraw the
invalid areas immediately. The refreshing will happen in :cpp:func:`lv_refr_now`
which might take longer.

The parameter of :cpp:func:`lv_refr_now` is a pointer to the display to refresh.  If
``NULL`` is passed, all displays that have active refresh timers will be refreshed.



API
***

.. API equals:
    LV_DEF_REFR_PERIOD
    lv_display_refr_timer
    lv_display_set_default
    lv_refr_now
    lv_timer_handler
