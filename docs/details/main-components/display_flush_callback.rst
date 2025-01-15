.. _display_flush_callback:

===================
Flush-Wait Callback
===================

By using :cpp:func:`lv_display_flush_ready` LVGL will normally spin in a loop
while waiting for flushing.

However with the help of :cpp:func:`lv_display_set_flush_wait_cb` a custom
wait callback be set for flushing.  This callback can use a semaphore, mutex,
or anything else to optimize waiting for the flush to be completed.  The callback
need not call :cpp:func:`lv_display_flush_ready` since the caller takes care of
that (clearing the display's ``flushing`` flag) when your callback returns.

However, if a Flush-Wait Callback is not set, LVGL assumes that
:cpp:func:`lv_display_flush_ready` is called after the flush has completed.
