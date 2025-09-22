
.. _lv_obj_flags:

=====
Flags
=====

There are some Widget attributes which can be enabled/disabled by
:cpp:expr:`lv_obj_add_flag(widget, LV_OBJ_FLAG_...)` and
:cpp:expr:`lv_obj_remove_flag(widget, LV_OBJ_FLAG_...)`.


To save memory the Widget's store these flags a bitfield. To test if a flag is set use :cpp:refr:`lv_obj_has_flag(obj, LV_OBJ_FLAG_...)`.

The aviailable flags are:

-  :cpp:enumerator:`LV_OBJ_FLAG_HIDDEN` Make the Widget hidden. (Like it wasn't there at all)
-  :cpp:enumerator:`LV_OBJ_FLAG_CLICKABLE` Make the Widget clickable by input devices
-  :cpp:enumerator:`LV_OBJ_FLAG_CLICK_FOCUSABLE` Add focused state to the Widget when clicked
-  :cpp:enumerator:`LV_OBJ_FLAG_CHECKABLE` Toggle checked state when the Widget is clicked
-  :cpp:enumerator:`LV_OBJ_FLAG_SCROLLABLE` Make the Widget scrollable
-  :cpp:enumerator:`LV_OBJ_FLAG_SCROLL_ELASTIC` Allow scrolling inside but with slower speed
-  :cpp:enumerator:`LV_OBJ_FLAG_SCROLL_MOMENTUM` Make the Widget scroll further when "thrown"
-  :cpp:enumerator:`LV_OBJ_FLAG_SCROLL_ONE` Allow scrolling only one snappable children
-  :cpp:enumerator:`LV_OBJ_FLAG_SCROLL_CHAIN_HOR` Allow propagating the horizontal scroll to a parent
-  :cpp:enumerator:`LV_OBJ_FLAG_SCROLL_CHAIN_VER` Allow propagating the vertical scroll to a parent
-  :cpp:enumerator:`LV_OBJ_FLAG_SCROLL_CHAIN` Simple packaging for (:cpp:expr:`LV_OBJ_FLAG_SCROLL_CHAIN_HOR | LV_OBJ_FLAG_SCROLL_CHAIN_VER`)
-  :cpp:enumerator:`LV_OBJ_FLAG_SCROLL_ON_FOCUS` Automatically scroll Widget to make it visible when focused
-  :cpp:enumerator:`LV_OBJ_FLAG_SCROLL_WITH_ARROW` Allow scrolling the focused Widget with arrow keys
-  :cpp:enumerator:`LV_OBJ_FLAG_SNAPPABLE` If scroll snap is enabled on the parent it can snap to this Widget
-  :cpp:enumerator:`LV_OBJ_FLAG_PRESS_LOCK` Keep the Widget pressed even if the press slid from the Widget
-  :cpp:enumerator:`LV_OBJ_FLAG_EVENT_BUBBLE` Propagate the events to the parent as well
-  :cpp:enumerator:`LV_OBJ_FLAG_EVENT_TRICKLE` Propagate the events to the children as well
-  :cpp:enumerator:`LV_OBJ_FLAG_STATE_TRICKLE` Propagate the state changes to the children as well
-  :cpp:enumerator:`LV_OBJ_FLAG_GESTURE_BUBBLE` Propagate the gestures to the parent
-  :cpp:enumerator:`LV_OBJ_FLAG_ADV_HITTEST` Allow performing more accurate hit (click) test. E.g. accounting for rounded corners
-  :cpp:enumerator:`LV_OBJ_FLAG_IGNORE_LAYOUT` Make the Widget not positioned by the layouts
-  :cpp:enumerator:`LV_OBJ_FLAG_FLOATING` Do not scroll the Widget when the parent scrolls and ignore layout
-  :cpp:enumerator:`LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS` Enable sending ``LV_EVENT_DRAW_TASK_ADDED`` events
-  :cpp:enumerator:`LV_OBJ_FLAG_OVERFLOW_VISIBLE` Do not clip the children's content to the parent's boundary
-  :cpp:enumerator:`LV_OBJ_FLAG_FLEX_IN_NEW_TRACK` Start a new flex track on this item
-  :cpp:enumerator:`LV_OBJ_FLAG_LAYOUT_1` Custom flag, free to use by layouts
-  :cpp:enumerator:`LV_OBJ_FLAG_LAYOUT_2` Custom flag, free to use by layouts
-  :cpp:enumerator:`LV_OBJ_FLAG_WIDGET_1` Custom flag, free to use by widget
-  :cpp:enumerator:`LV_OBJ_FLAG_WIDGET_2` Custom flag, free to use by widget
-  :cpp:enumerator:`LV_OBJ_FLAG_USER_1` Custom flag, free to use by user
-  :cpp:enumerator:`LV_OBJ_FLAG_USER_2` Custom flag, free to use by user
-  :cpp:enumerator:`LV_OBJ_FLAG_USER_3` Custom flag, free to use by user
-  :cpp:enumerator:`LV_OBJ_FLAG_USER_4` Custom flag, free to use by user

Some examples:

.. code-block:: c

    /* Hide on Widget */
    lv_obj_add_flag(widget, LV_OBJ_FLAG_HIDDEN);

    /* Make a Widget non-clickable */
    lv_obj_remove_flag(widget, LV_OBJ_FLAG_CLICKABLE);


    /* Is it clickable */
    if(lv_obj_has_flag(widget, LV_OBJ_FLAG_CLICKABLE)) printf("Clickable\n");


