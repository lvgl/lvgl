.. _display_events:

==============
Display Events
==============

:cpp:expr:`lv_display_add_event_cb(disp, event_cb, LV_EVENT_..., user_data)` adds
an event handler to a display.

If you added ``user_data`` to the Display, you can retrieve it in an event like this:

.. code-block:: c

    lv_display_t * display1;
    my_type_t    * my_user_data;
    display1 = (lv_display_t *)lv_event_get_current_target(e);
    my_user_data = lv_display_get_user_data(display1);

The following events are sent for Display (lv_display_t) objects:

.. include:: display_events_list.txt



API
***

.. API equals:  lv_display_add_event_cb
