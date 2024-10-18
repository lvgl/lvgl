.. _events:

======
Events
======

Events are triggered in LVGL when something happens which might be
interesting to the user, e.g. when a Widget:

- is clicked
- is scrolled
- has its value changed
- is redrawn, etc.

Besides Widgets, events can registered from displays and input devices as well.
It is not detailed below, but you can do this by changing the prefix of the functions
from ``lv_obj_`` to ``lv_display_`` or ``lv_indev_``.


.. _adding_events_to_a_widget:

Adding Events to a Widget
*************************

The user can assign callback functions to a widget to process events.
In practice, it looks like this:

.. code-block:: c

   lv_obj_t * btn = lv_button_create(lv_screen_active());
   lv_obj_add_event_cb(btn, my_event_cb, LV_EVENT_CLICKED, user_data);   /* Assign an event callback */

   ...

   static void my_event_cb(lv_event_t * event)
   {
       printf("Clicked\n");
   }

In the example :cpp:enumerator:`LV_EVENT_CLICKED` means that only the click event will
call ``my_event_cb``. See the :ref:`list of event codes <events_codes>` for
all the options. :cpp:enumerator:`LV_EVENT_ALL` can be used to receive all events.

The last parameter of :cpp:func:`lv_obj_add_event` is a pointer to any custom
data that will be available in the event.  NULL may be passed for this argument if
there is no need to use that data when the event is processed.  You can retrieve the
pointer passed when setting the callback function like this:

.. code-block:: c

    my_user_data_t  * user_data;
    ...
    user_data = lv_event_get_user_data(e);

More events can be added to a Widget, like this:

.. code-block:: c

   lv_obj_add_event_cb(widget, my_event_cb_1, LV_EVENT_CLICKED, NULL);
   lv_obj_add_event_cb(widget, my_event_cb_2, LV_EVENT_PRESSED, NULL);
   lv_obj_add_event_cb(widget, my_event_cb_3, LV_EVENT_ALL, NULL);       /* No filtering, receive all events */

Even the same event callback can be used on a Widget with different
``user_data``. For example:

.. code-block:: c

   lv_obj_add_event_cb(widget, increment_on_click, LV_EVENT_CLICKED, &num1);
   lv_obj_add_event_cb(widget, increment_on_click, LV_EVENT_CLICKED, &num2);

The events will be called in the order as they were added.

Other Widgets can use the same *event callback*.

In the very same way, events can be attached to input devices and displays like this:

.. code-block:: c

   lv_display_add_event_cb(disp, event_cb, LV_EVENT_RESOLUTION_CHANGED, NULL);
   lv_indev_add_event_cb(indev, event_cb, LV_EVENT_CLICKED, NULL);


Removing Event(s) from Widgets
******************************

.. code-block:: c

   uint32_t i;
   uint32_t event_cnt = lv_obj_get_event_count(widget);
   for(i = 0; i < event_cnt; i++) {
       lv_event_dsc_t * event_dsc = lv_obj_get_event_dsc(widget, i);
       if(lv_event_dsc_get_cb(event_dsc) == some_event_cb) {
           lv_obj_remove_event(widget, i);
           break;
       }
   }


.. _events_codes:

Event Codes
***********

The event codes can be grouped into these categories: - Input device
events - Drawing events - Other events - Special events - Custom events

All Widgets (such as Buttons/Labels/Sliders etc.) regardless their type
receive the *Input device*, *Drawing* and *Other* events.

However, the *Special events* are specific to a particular widget type.
See the :ref:`widgets' documentation <widgets>` to learn when they
are sent,

*Custom events* are added by the user and are never sent by LVGL.

The following event codes exist:

Input Device Events
-------------------

-  :cpp:enumerator:`LV_EVENT_PRESSED`: Widget has been pressed
-  :cpp:enumerator:`LV_EVENT_PRESSING`: Widget is being pressed (called continuously while pressing)
-  :cpp:enumerator:`LV_EVENT_PRESS_LOST`: Widget is still being pressed but slid cursor/finger off Widget
-  :cpp:enumerator:`LV_EVENT_SHORT_CLICKED`: Widget was pressed for a short period of time, and then released without scrolling.
-  :cpp:enumerator:`LV_EVENT_SINGLE_CLICKED`: Widget was pressed for a short period of time, and then released without scrolling, for the first time in a click streak. A click streak refers to multiple short clicks within a short period of time and a small distance.
-  :cpp:enumerator:`LV_EVENT_DOUBLE_CLICKED`: Widget was pressed for a short period of time, and then released without scrolling, for the second time in a click streak.
-  :cpp:enumerator:`LV_EVENT_TRIPLE_CLICKED`: Widget was pressed for a short period of time, and then released without scrolling, for the third time in a click streak.
-  :cpp:enumerator:`LV_EVENT_LONG_PRESSED`: Widget has been pressed for at least `long_press_time`.  Not called if scrolled.
-  :cpp:enumerator:`LV_EVENT_LONG_PRESSED_REPEAT`: Called after `long_press_time` in every `long_press_repeat_time` ms.  Not called if scrolled.
-  :cpp:enumerator:`LV_EVENT_CLICKED`: Called on release if not scrolled (regardless of long press)
-  :cpp:enumerator:`LV_EVENT_RELEASED`: Called in every cases when Widget has been released
-  :cpp:enumerator:`LV_EVENT_SCROLL_BEGIN`: Scrolling begins. The event parameter is a pointer to the animation of the scroll. Can be modified
-  :cpp:enumerator:`LV_EVENT_SCROLL_THROW_BEGIN`:
-  :cpp:enumerator:`LV_EVENT_SCROLL_END`: Scrolling ends
-  :cpp:enumerator:`LV_EVENT_SCROLL`: Scrolling
-  :cpp:enumerator:`LV_EVENT_GESTURE`: A gesture is detected. Get the gesture with :cpp:expr:`lv_indev_get_gesture_dir(lv_indev_active())`
-  :cpp:enumerator:`LV_EVENT_KEY`: A key is sent to Widget. Get the key with :cpp:expr:`lv_indev_get_key(lv_indev_active())`
-  :cpp:enumerator:`LV_EVENT_FOCUSED`: Widget received focus
-  :cpp:enumerator:`LV_EVENT_DEFOCUSED`: Widget is defocused
-  :cpp:enumerator:`LV_EVENT_LEAVE`: Widget is defocused but still selected
-  :cpp:enumerator:`LV_EVENT_HIT_TEST`: Perform advanced hit-testing
-  :cpp:enumerator:`LV_EVENT_INDEV_RESET`: Indev has been reset
-  :cpp:enumerator:`LV_EVENT_HOVER_OVER`: Indev hover over Widget
-  :cpp:enumerator:`LV_EVENT_HOVER_LEAVE`: Indev hover leave Widget

Drawing Events
--------------

-  :cpp:enumerator:`LV_EVENT_COVER_CHECK`: Check if Widget fully covers an area. The event parameter is :cpp:type:`lv_cover_check_info_t` ``*``.
-  :cpp:enumerator:`LV_EVENT_REFR_EXT_DRAW_SIZE`: Get the required extra draw area around Widget (e.g. for shadow). The event parameter is :cpp:type:`int32_t` ``*`` to store the size.
-  :cpp:enumerator:`LV_EVENT_DRAW_MAIN_BEGIN`: Starting the main drawing phase
-  :cpp:enumerator:`LV_EVENT_DRAW_MAIN`: Perform the main drawing
-  :cpp:enumerator:`LV_EVENT_DRAW_MAIN_END`: Finishing the main drawing phase
-  :cpp:enumerator:`LV_EVENT_DRAW_POST_BEGIN`: Starting the post draw phase (when all children are drawn)
-  :cpp:enumerator:`LV_EVENT_DRAW_POST`: Perform the post draw phase (when all children are drawn)
-  :cpp:enumerator:`LV_EVENT_DRAW_POST_END`: Finishing the post draw phase (when all children are drawn)
-  :cpp:enumerator:`LV_EVENT_DRAW_TASK_ADDED`: Adding a draw task

Special Events
--------------

-  :cpp:enumerator:`LV_EVENT_VALUE_CHANGED`: Widget's value has changed (i.e. slider moved)
-  :cpp:enumerator:`LV_EVENT_INSERT`: A text is inserted to Widget. The event data is ``char `*`` being inserted.
-  :cpp:enumerator:`LV_EVENT_REFRESH`: Notify Widget to refresh something on it (for the user)
-  :cpp:enumerator:`LV_EVENT_READY`: A process has finished
-  :cpp:enumerator:`LV_EVENT_CANCEL`: A process has been cancelled

Other Events
------------

-  :cpp:enumerator:`LV_EVENT_CREATE`: Widget is being created
-  :cpp:enumerator:`LV_EVENT_DELETE`: Widget is being deleted
-  :cpp:enumerator:`LV_EVENT_CHILD_CHANGED`: Child was removed, added, or its size, position were changed
-  :cpp:enumerator:`LV_EVENT_CHILD_CREATED`: Child was created, always bubbles up to all parents
-  :cpp:enumerator:`LV_EVENT_CHILD_DELETED`: Child was deleted, always bubbles up to all parents
-  :cpp:enumerator:`LV_EVENT_SCREEN_UNLOAD_START`: A screen unload started, fired immediately when scr_load is called
-  :cpp:enumerator:`LV_EVENT_SCREEN_LOAD_START`: A screen load started, fired when the screen change delay is expired
-  :cpp:enumerator:`LV_EVENT_SCREEN_LOADED`: A screen was loaded
-  :cpp:enumerator:`LV_EVENT_SCREEN_UNLOADED`: A screen was unloaded
-  :cpp:enumerator:`LV_EVENT_SIZE_CHANGED`: Widget coordinates/size have changed
-  :cpp:enumerator:`LV_EVENT_STYLE_CHANGED`: Widget's style has changed
-  :cpp:enumerator:`LV_EVENT_LAYOUT_CHANGED`: The children position has changed due to a layout recalculation
-  :cpp:enumerator:`LV_EVENT_GET_SELF_SIZE`: Get the internal size of a widget

Display Events
--------------

-  :cpp:enumerator:`LV_EVENT_INVALIDATE_AREA`
-  :cpp:enumerator:`LV_EVENT_RESOLUTION_CHANGED`
-  :cpp:enumerator:`LV_EVENT_COLOR_FORMAT_CHANGED`
-  :cpp:enumerator:`LV_EVENT_REFR_REQUEST`
-  :cpp:enumerator:`LV_EVENT_REFR_START`
-  :cpp:enumerator:`LV_EVENT_REFR_READY`
-  :cpp:enumerator:`LV_EVENT_RENDER_START`
-  :cpp:enumerator:`LV_EVENT_RENDER_READY`
-  :cpp:enumerator:`LV_EVENT_FLUSH_START`
-  :cpp:enumerator:`LV_EVENT_FLUSH_FINISH`


Custom Events
-------------

Any number of custom event codes can be registered by
``uint32_t MY_EVENT_1 =`` :cpp:func:`lv_event_register_id`

They can be sent to any Widget with
:cpp:expr:`lv_obj_send_event(widget, MY_EVENT_1, &some_data)`


Refresh Event
-------------

:cpp:enumerator:`LV_EVENT_REFRESH` is a special event because it's designed to let the
user notify a Widget to refresh itself. Some examples:

- notify a label to refresh its text according to one or more variables (e.g. current time)
- refresh a label when the language changes
- enable a button if some conditions are met (e.g. the correct PIN is entered)
- add/remove styles to/from a Widget if a limit is exceeded, etc


Sending Events Manually
***********************

To manually send events to a Widget, use
``lv_obj_send_event(widget, <EVENT_CODE>, &some_data)``.

For example, this can be used to manually close a message box by
simulating a button press (although there are simpler ways to do this):

.. code-block:: c

   /* Simulate the press of the first button (indexes start from zero) */
   uint32_t btn_id = 0;
   lv_obj_send_event(mbox, LV_EVENT_VALUE_CHANGED, &btn_id);

The same works for display and input devices with
``lv_display_send_event(widget, <EVENT_CODE>, &some_data)`` and
``lv_indev_send_event(widget, <EVENT_CODE>, &some_data)``.


Fields of lv_event_t
********************

:cpp:type:`lv_event_t` is the only parameter passed to the event callback and it
contains all data about the event. The following values can be gotten from it:

- :cpp:expr:`lv_event_get_code(e)`: get the event code
- :cpp:expr:`lv_event_get_current_target(e)`: get Widget to which an event was sent. I.e. the Widget whose event handler is being called.
- :cpp:expr:`lv_event_get_target(e)`: get Widget that originally triggered the event (different from :cpp:func:`lv_event_get_target` if :ref:`event bubbling <event_bubbling>` is enabled)
- :cpp:expr:`lv_event_get_user_data(e)`: get the pointer passed as the last parameter of :cpp:func:`lv_obj_add_event`.
- :cpp:expr:`lv_event_get_param(e)`: get the parameter passed as the last parameter of :cpp:func:`lv_obj_send_event`

.. _event_bubbling:


Event Bubbling
**************

If :cpp:expr:`lv_obj_add_flag(widget, LV_OBJ_FLAG_EVENT_BUBBLE)` is enabled all
events will be sent to a Widget's parent as well.  If the parent also has
:cpp:enumerator:`LV_OBJ_FLAG_EVENT_BUBBLE` enabled the event will be sent to its
parent, and so on.

The *target* parameter of the event is always the current target Widget,
not the original Widget. To get the original target call
:cpp:expr:`lv_event_get_target_obj(e)` in the event handler.

.. _events_examples:


Examples
********

.. include:: ../../examples/event/index.rst


.. _events_api:

API
***
