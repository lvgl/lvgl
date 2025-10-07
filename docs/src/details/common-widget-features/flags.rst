.. _lv_obj_flags:

=====
Flags
=====

Some widget attributes can be enabled or disabled using
:cpp:expr:`lv_obj_add_flag(widget, LV_OBJ_FLAG_...)` and
:cpp:expr:`lv_obj_remove_flag(widget, LV_OBJ_FLAG_...)`.

To save memory, widgets store these flags in a bitfield. To check if a flag is set, use:
:cpp:expr:`lv_obj_has_flag(obj, LV_OBJ_FLAG_...)`.

The available flags are:

- :cpp:enumerator:`LV_OBJ_FLAG_HIDDEN`: Make the widget hidden (as if it weren’t there at all).
- :cpp:enumerator:`LV_OBJ_FLAG_CLICKABLE`: Make the widget clickable by input devices.
- :cpp:enumerator:`LV_OBJ_FLAG_CLICK_FOCUSABLE`: Add the focused state to the widget when clicked.
- :cpp:enumerator:`LV_OBJ_FLAG_CHECKABLE`: Toggle the checked state when the widget is clicked.
- :cpp:enumerator:`LV_OBJ_FLAG_SCROLLABLE`: Make the widget scrollable.
- :cpp:enumerator:`LV_OBJ_FLAG_SCROLL_ELASTIC`: Allow elastic scrolling with slower movement.
- :cpp:enumerator:`LV_OBJ_FLAG_SCROLL_MOMENTUM`: Enable momentum scrolling (continue scrolling when “thrown”).
- :cpp:enumerator:`LV_OBJ_FLAG_SCROLL_ONE`: Allow scrolling only one snappable child.
- :cpp:enumerator:`LV_OBJ_FLAG_SCROLL_CHAIN_HOR`: Propagate horizontal scrolling to the parent.
- :cpp:enumerator:`LV_OBJ_FLAG_SCROLL_CHAIN_VER`: Propagate vertical scrolling to the parent.
- :cpp:enumerator:`LV_OBJ_FLAG_SCROLL_CHAIN`: Shorthand for `SCROLL_CHAIN_HOR | SCROLL_CHAIN_VER`.
- :cpp:enumerator:`LV_OBJ_FLAG_SCROLL_ON_FOCUS`: Automatically scroll to make the widget visible when focused.
- :cpp:enumerator:`LV_OBJ_FLAG_SCROLL_WITH_ARROW`: Allow scrolling the focused widget with arrow keys.
- :cpp:enumerator:`LV_OBJ_FLAG_SNAPPABLE`: Allow the widget to be snapped if the parent has scroll snapping enabled.
- :cpp:enumerator:`LV_OBJ_FLAG_PRESS_LOCK`: Keep the widget in the pressed state even if the pointer moves outside it.
- :cpp:enumerator:`LV_OBJ_FLAG_EVENT_BUBBLE`: Propagate events to the parent.
- :cpp:enumerator:`LV_OBJ_FLAG_EVENT_TRICKLE`: Propagate events to children.
- :cpp:enumerator:`LV_OBJ_FLAG_STATE_TRICKLE`: Propagate state changes to children.
- :cpp:enumerator:`LV_OBJ_FLAG_GESTURE_BUBBLE`: Propagate gestures to the parent.
- :cpp:enumerator:`LV_OBJ_FLAG_ADV_HITTEST`: Enable more accurate hit (click) testing (e.g., account for rounded corners).
- :cpp:enumerator:`LV_OBJ_FLAG_IGNORE_LAYOUT`: Exclude the widget from layout positioning.
- :cpp:enumerator:`LV_OBJ_FLAG_FLOATING`: Do not scroll with the parent and ignore layout.
- :cpp:enumerator:`LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS`: Enable sending ``LV_EVENT_DRAW_TASK_ADDED`` events.
- :cpp:enumerator:`LV_OBJ_FLAG_OVERFLOW_VISIBLE`: Allow children to overflow outside the widget's bounds.
- :cpp:enumerator:`LV_OBJ_FLAG_FLEX_IN_NEW_TRACK`: Start a new flex track on this item.
- :cpp:enumerator:`LV_OBJ_FLAG_LAYOUT_1`: Custom flag, free to use by layouts.
- :cpp:enumerator:`LV_OBJ_FLAG_LAYOUT_2`: Custom flag, free to use by layouts.
- :cpp:enumerator:`LV_OBJ_FLAG_WIDGET_1`: Custom flag, free to use by widgets.
- :cpp:enumerator:`LV_OBJ_FLAG_WIDGET_2`: Custom flag, free to use by widgets.
- :cpp:enumerator:`LV_OBJ_FLAG_USER_1`: Custom flag, free to use by the user.
- :cpp:enumerator:`LV_OBJ_FLAG_USER_2`: Custom flag, free to use by the user.
- :cpp:enumerator:`LV_OBJ_FLAG_USER_3`: Custom flag, free to use by the user.
- :cpp:enumerator:`LV_OBJ_FLAG_USER_4`: Custom flag, free to use by the user.

Some examples:

.. code-block:: c

    /* Hide a Widget */
    lv_obj_add_flag(widget, LV_OBJ_FLAG_HIDDEN);

    /* Make a Widget non-clickable */
    lv_obj_remove_flag(widget, LV_OBJ_FLAG_CLICKABLE);

    /* Check if it is clickable */
    if(lv_obj_has_flag(widget, LV_OBJ_FLAG_CLICKABLE)) printf("Clickable\n");

Adding and/or Removing Multiple Flags
*************************************

When adding or removing multiple flags, you have two options:

**Option 1: Multiple calls (Recommended)**

This approach is clearer and works seamlessly in both C and C++:

.. code-block:: c

    lv_obj_add_flag(widget, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(widget, LV_OBJ_FLAG_EVENT_BUBBLE);

**Option 2: Single call with bitwise OR**

You can combine multiple flags in one call using the bitwise OR operator (``|``). When using a C++ compiler, you must cast the result:

.. code-block:: c

    lv_obj_add_flag(widget, (lv_obj_flag_t)(LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_EVENT_BUBBLE));

.. note::

    The cast to ``lv_obj_flag_t`` is required in C++ due to stricter type checking, but is optional in C.
