.. _widget_basics:

=============
Widget Basics
=============



What is a Widget?
*****************
A Widget is the **basic building block** of the LVGL user interface.

Examples of Widgets:  :ref:`Base Widget (and Screen) <base_widget>`,
:ref:`Button <lv_button>`, :ref:`Label <lv_label>`,
:ref:`Image <lv_image>`, :ref:`List <lv_list>`,
:ref:`Chart <lv_chart>` and :ref:`Text Area <lv_textarea>`.

See :ref:`widgets` to see all Widget types.

All Widgets are referenced using an :cpp:type:`lv_obj_t` pointer as a handle.
This pointer can later be used to read or change the Widget's attributes.



.. _base_widget:

Base Widget
***********
The most fundamental of all Widgets is the Base Widget, on which all other widgets
are based.  From an Object-Oriented perspective, think of the Base Widget as the
Widget class from which all other Widgets inherit.

The functions and functionalities of the Base Widget can be used with
other widgets as well.  For example :cpp:expr:`lv_obj_set_width(slider, 100)`.

The Base Widget can be used directly as a simple widget.  While it is a simple
rectangle, it has a large number of features shared with all Widgets, detailed
below and in subsequent pages.  In HTML terms, think of it as a ``<div>``.



.. _widget_attributes:

Attributes
**********


Basic attributes
----------------

All Widget types share some basic attributes:

- Position
- Size
- Parent
- Styles
- Events it emits
- Flags like *Clickable*, *Scollable*, etc.
- Etc.

You can set/get these attributes with ``lv_obj_set_...`` and
``lv_obj_get_...`` functions. For example:

.. code-block:: c

   /* Set basic Widget attributes */
   lv_obj_set_size(btn1, 100, 50);   /* Set a button's size */
   lv_obj_set_pos(btn1, 20,30);      /* Set a button's position */

For complete details on position, size, coordinates and layouts, see :ref:`coord`.


Widget-specific attributes
--------------------------

The Widget types have special attributes as well. For example, a slider has

- Minimum and maximum values
- Current value

For these special attributes, every Widget type may have unique API
functions. For example for a slider:

.. code-block:: c

   /* Set slider specific attributes */
   lv_slider_set_range(slider1, 0, 100);               /* Set the min. and max. values */
   lv_slider_set_value(slider1, 40, LV_ANIM_ON);       /* Set the current value (position) */

The API of the widgets is described in their
:ref:`Documentation <widgets>` but you can also check the respective
header files (e.g. *widgets/lv_slider.h*)


.. _lv_obj_parents_and_children:

Parents and children
--------------------

A Widget's parent is set when the widget is created --- the parent is passed to the
creation function.

To get a Widget's current parent, use :cpp:expr:`lv_obj_get_parent(widget)`.

You can move the Widget to a new parent with :cpp:expr:`lv_obj_set_parent(widget, new_parent)`.

To get a specific child of a parent use :cpp:expr:`lv_obj_get_child(parent, idx)`.
Some examples for ``idx``:

- ``0`` get the child created first
- ``1`` get the child created second
- ``-1`` get the child created last

You can iterate through a parent Widget's children like this:

.. code-block:: c

    uint32_t i;
    for(i = 0; i < lv_obj_get_child_count(parent); i++) {
        lv_obj_t * child = lv_obj_get_child(parent, i);
        /* Do something with child. */
    }

:cpp:expr:`lv_obj_get_index(widget)` returns the index of the Widget in its parent.
It is equivalent to the number of older children in the parent.

You can bring a Widget to the foreground or send it to the background with
:cpp:expr:`lv_obj_move_foreground(widget)` and :cpp:expr:`lv_obj_move_background(widget)`.

You can change the index of a Widget in its parent using :cpp:expr:`lv_obj_move_to_index(widget, index)`.

You can swap the position of two Widgets with :cpp:expr:`lv_obj_swap(widget1, widget2)`.

To get a Widget's Screen (highest-level parent) use :cpp:expr:`lv_obj_get_screen(widget)`.



.. _widget_working_mechanisms:

Working Mechanisms
******************

Parent-child structure
----------------------

A parent Widget can be considered as the container of its children.  Every Widget has
exactly one parent Widget (except Screens), but a parent Widget can have any number
of children.  There is no limitation for the type of the parent but there are Widgets
which are typically a parent (e.g. button) or a child (e.g. label).



Moving together
---------------

If the position of a parent changes, the children will move along with
it. Therefore, all positions are relative to the parent.

.. image:: /misc/par_child1.png

.. code-block:: c

   lv_obj_t * parent = lv_obj_create(lv_screen_active());  /* Create a parent Widget on current screen */
   lv_obj_set_size(parent, 100, 80);                       /* Set size of parent */

   lv_obj_t * obj1 = lv_obj_create(parent);                /* Create a Widget on previously created parent Widget */
   lv_obj_set_pos(widget1, 10, 10);                        /* Set position of new Widget */

Modify the position of the parent:

.. image:: /misc/par_child2.png

.. code-block:: c

   lv_obj_set_pos(parent, 50, 50); /* Move the parent. The child will move with it. */

(For simplicity the adjusting of colors of the Widgets is not shown in
the example.)

Visibility only on the parent
-----------------------------

If a child is partially or fully outside its parent then the parts
outside will not be visible.

.. image:: /misc/par_child3.png

.. code-block:: c

   lv_obj_set_x(widget1, -30);    /* Move the child a little bit off the parent */

This behavior can be overwritten with
:cpp:expr:`lv_obj_add_flag(widget, LV_OBJ_FLAG_OVERFLOW_VISIBLE)` which allow the
children to be drawn out of the parent. In addition to this, you must register
the following event callback (this was not required in previous versions).

Note: ``ext_width`` should be the maximum absolute width the children will be
drawn within.

.. code-block:: c

    static void ext_draw_size_event_cb(lv_event_t * e)
    {
        lv_event_set_ext_draw_size(e, 30); /*Set 30px extra draw area around the widget*/
    }

Create and delete Widgets
-------------------------

In LVGL, Widgets can be created and deleted dynamically at run time. It
means only the currently created (existing) Widgets consume RAM.

This allows for the creation of a screen just when a button is clicked
to open it, and for deletion of screens when a new screen is loaded.

UIs can be created based on the current environment of the device. For
example one can create meters, charts, bars and sliders based on the
currently attached sensors.

Every widget has its own **create** function with a prototype like this:

.. code-block:: c

   lv_obj_t * lv_<widget>_create(lv_obj_t * parent, <other parameters if any>);

Typically, the create functions only have a ``parent`` parameter telling
them on which Widget to create the new Widget.

The return value is a pointer to the created Widget with :cpp:type:`lv_obj_t` ``*``
type.

There is a common **delete** function for all Widget types. It deletes
the Widget and all of its children.

.. code-block:: c

   void lv_obj_delete(lv_obj_t * widget);

:cpp:func:`lv_obj_delete` will delete the Widget immediately. If for any reason you
can't delete the Widget immediately you can use
:cpp:expr:`lv_obj_delete_async(widget)` which will perform the deletion on the next
call of :cpp:func:`lv_timer_handler`. This is useful e.g. if you want to
delete the parent of a Widget in the child's :cpp:enumerator:`LV_EVENT_DELETE`
handler.

You can remove all the children of a Widget (but not the Widget itself)
using :cpp:expr:`lv_obj_clean(widget)`.

You can use :cpp:expr:`lv_obj_delete_delayed(widget, 1000)` to delete a Widget after
some time. The delay is expressed in milliseconds.

Sometimes you're not sure whether a Widget was deleted and you need some way to
check if it's still "alive". Anytime before the Widget is deleted, you can use
cpp:expr:`lv_obj_null_on_delete(&widget)` to cause your Widget pointer to be set to ``NULL``
when the Widget is deleted.

Make sure the pointer variable itself stays valid until the Widget is deleted. Here
is an example:

.. code:: c

   void some_timer_callback(lv_timer_t * t)
   {
      static lv_obj_t * my_label;
      if(my_label == NULL) {
         my_label = lv_label_create(lv_screen_active());
         lv_obj_delete_delayed(my_label, 1000);
         lv_obj_null_on_delete(&my_label);
      }
      else {
         lv_obj_set_x(my_label, lv_obj_get_x(my_label) + 1);
      }
   }



.. _screens:

Screens
*******

What are Screens?
-----------------

Not to be confused with a :ref:`display`, Screens are simply any Widget created
without a parent (i.e. passing NULL for the ``parent`` argument during creation).  As
such, they form the "root" of a Widget Tree.

Normally the Base Widget is used for this purpose since it has all the features most
Screens need.  But an :ref:`lv_image` Widget can also be used to create a wallpaper
background for the Widget Tree.

All Screens:

- are automatically attached to the :ref:`default_display` current when the Screen
  was created;
- automatically occupy the full area of the associated display;
- cannot be moved, i.e. functions such as :cpp:func:`lv_obj_set_pos` and
  :cpp:func:`lv_obj_set_size` cannot be used on screens.

Each :ref:`display` object can have multiple screens associated with it, but not vice
versa.  Thus the relationship::

       Display
          |
         --- (one or more)
         /|\
    Screen Widgets  (root of a Widget Tree)
          |
          O  (zero or more)
         /|\
    Child Widgets


Creating Screens
----------------

Screens are created like this:

.. code-block:: c

   lv_obj_t * scr1 = lv_obj_create(NULL);

Screens can be deleted with :cpp:expr:`lv_obj_delete(scr)`, but be sure you do not
delete the :ref:`active_screen`.


.. _active_screen:

Active Screen
-------------
While each :ref:`display` object can have any number of Screens Widgets associated
with it, only one of those Screens is considered "Active" at any given time.  That
Screen is referred to as the Display's "Active Screen".  For this reason, only one
Screen and its child Widgets will ever be shown on a display at one time.

When each :ref:`display` object was created, a default screen was created with it and
set as its "Active Screen".

To get a pointer to the "Active Screen", call :cpp:func:`lv_screen_active`.

To set a Screen to be the "Active Screen", call :cpp:func:`lv_screen_load` or
:cpp:func:`lv_screen_load_anim`.


.. _loading_screens:

Loading Screens
---------------

To load a new screen, use :cpp:expr:`lv_screen_load(scr1)`.  This sets ``scr1`` as
the Active Screen.

Load Screen with Animation
^^^^^^^^^^^^^^^^^^^^^^^^^^

A new screen can be loaded with animation by using
:cpp:expr:`lv_screen_load_anim(scr, transition_type, time, delay, auto_del)`. The
following transition types exist:

- :cpp:enumerator:`LV_SCR_LOAD_ANIM_NONE`: Switch immediately after ``delay`` milliseconds
- :cpp:enumerator:`LV_SCR_LOAD_ANIM_OVER_LEFT`, :cpp:enumerator:`LV_SCR_LOAD_ANIM_OVER_RIGHT`, :cpp:enumerator:`LV_SCR_LOAD_ANIM_OVER_TOP` and :cpp:enumerator:`LV_SCR_LOAD_ANIM_OVER_BOTTOM`: Move the new screen over the current towards the given direction
- :cpp:enumerator:`LV_SCR_LOAD_ANIM_OUT_LEFT`, :cpp:enumerator:`LV_SCR_LOAD_ANIM_OUT_RIGHT`, :cpp:enumerator:`LV_SCR_LOAD_ANIM_OUT_TOP` and :cpp:enumerator:`LV_SCR_LOAD_ANIM_OUT_BOTTOM`: Move out the old screen over the current towards the given direction
- :cpp:enumerator:`LV_SCR_LOAD_ANIM_MOVE_LEFT`, :cpp:enumerator:`LV_SCR_LOAD_ANIM_MOVE_RIGHT`, :cpp:enumerator:`LV_SCR_LOAD_ANIM_MOVE_TOP` and :cpp:enumerator:`LV_SCR_LOAD_ANIM_MOVE_BOTTOM`: Move both the current and new screens towards the given direction
- :cpp:enumerator:`LV_SCR_LOAD_ANIM_FADE_IN` and :cpp:enumerator:`LV_SCR_LOAD_ANIM_FADE_OUT`: Fade the new screen over the old screen, or vice versa

Setting ``auto_del`` to ``true`` will automatically delete the old
screen when the animation is finished.

The new screen will become active (returned by :cpp:func:`lv_screen_active`) when
the animation starts after ``delay`` time. All inputs are disabled
during the screen animation.


.. _layers_overview:

Layers
------

When an ``lv_display_t`` object is created, 4 Screens (layers) are created and
attached to it.

1.  Bottom Layer
2.  Active Screen
3.  Top Layer
4.  System Layer

1, 3 and 4 are independent of the :ref:`active_screen` and they will be shown (if
they contain anything that is visible) regardless of which screen is the Active
Screen.  See :ref:`screen_layers` for more information.


.. _transparent_screens:

Transparent Screens
-------------------

Usually, the opacity of the Screen is :cpp:enumerator:`LV_OPA_COVER` to provide a
solid background for its children. If this is not the case (opacity <
100%) the display's ``bottom_layer`` will be visible.  If the bottom layer's
opacity is also not :cpp:enumerator:`LV_OPA_COVER` LVGL will have no solid background
to draw.

This configuration (transparent Screen) could be useful to create, for example,
on-screen display (OSD) menus where a video is played on a different hardware layer
of the display panel, and a menu is overlaid on a higher layer.

To properly render a UI on a transparent Screen the Display's color format needs to
be set to one with an alpha channel (for example LV_COLOR_FORMAT_ARGB8888).

In summary, to enable transparent screens and displays for OSD menu-like UIs:

- Set the screen's ``bg_opa`` to transparent:
  :cpp:expr:`lv_obj_set_style_bg_opa(lv_screen_active(), LV_OPA_TRANSP, LV_PART_MAIN)`
- Set the bottom layer's ``bg_opa`` to transparent:
  :cpp:expr:`lv_obj_set_style_bg_opa(lv_layer_bottom(), LV_OPA_TRANSP, LV_PART_MAIN)`
- Set a color format with alpha channel. E.g.
  :cpp:expr:`lv_display_set_color_format(disp, LV_COLOR_FORMAT_ARGB8888)`



.. _widget_parts:

Parts
*****

The widgets are built from multiple parts. For example a
:ref:`Base Widget <base_widget>` uses the main and scrollbar parts but a
:ref:`Slider <lv_slider>` uses the main, indicator and knob parts.
Parts are similar to *pseudo-elements* in CSS.

The following predefined parts exist in LVGL:

- :cpp:enumerator:`LV_PART_MAIN`: A background like rectangle
- :cpp:enumerator:`LV_PART_SCROLLBAR`: The scrollbar(s)
- :cpp:enumerator:`LV_PART_INDICATOR`: Indicator, e.g. for slider, bar, switch, or the tick box of the checkbox
- :cpp:enumerator:`LV_PART_KNOB`: Like a handle to grab to adjust the value
- :cpp:enumerator:`LV_PART_SELECTED`: Indicate the currently selected option or section
- :cpp:enumerator:`LV_PART_ITEMS`: Used if the widget has multiple similar elements (e.g. table cells)
- :cpp:enumerator:`LV_PART_CURSOR`: Mark a specific place e.g. text area's or chart's cursor
- :cpp:enumerator:`LV_PART_CUSTOM_FIRST`: Custom parts can be added from here.

The main purpose of parts is to allow styling the "components" of the
widgets. They are described in more detail in the
:ref:`Style overview <styles>` section.



.. _widget_states:

States
******

The Widget can be in a combination of the following states:

- :cpp:enumerator:`LV_STATE_DEFAULT`: Normal, released state
- :cpp:enumerator:`LV_STATE_CHECKED`: Toggled or checked state
- :cpp:enumerator:`LV_STATE_FOCUSED`: Focused via keypad or encoder or clicked via touchpad/mouse
- :cpp:enumerator:`LV_STATE_FOCUS_KEY`: Focused via keypad or encoder but not via touchpad/mouse
- :cpp:enumerator:`LV_STATE_EDITED`: Edit by an encoder
- :cpp:enumerator:`LV_STATE_HOVERED`: Hovered by mouse (not supported now)
- :cpp:enumerator:`LV_STATE_PRESSED`: Being pressed
- :cpp:enumerator:`LV_STATE_SCROLLED`: Being scrolled
- :cpp:enumerator:`LV_STATE_DISABLED`: Disabled state
- :cpp:enumerator:`LV_STATE_USER_1`: Custom state
- :cpp:enumerator:`LV_STATE_USER_2`: Custom state
- :cpp:enumerator:`LV_STATE_USER_3`: Custom state
- :cpp:enumerator:`LV_STATE_USER_4`: Custom state

The states are usually automatically changed by the library as the user
interacts with a Widget (presses, releases, focuses, etc.). However,
the states can be changed manually as well. To set or clear given state (but
leave the other states untouched) use
:cpp:expr:`lv_obj_add_state(widget, LV_STATE_...)` and
:cpp:expr:`lv_obj_remove_state(widget, LV_STATE_...)`.  In both cases OR-ed state
values can be used as well. E.g.
:cpp:expr:`lv_obj_add_state(widget, part, LV_STATE_PRESSED | LV_PRESSED_CHECKED)`.

To learn more about the states read the related section of the
:ref:`Style overview <styles>`.



.. _lv_obj_flags:

Flags
*****

There are some Widget attributes which can be enabled/disabled by
:cpp:expr:`lv_obj_add_flag(widget, LV_OBJ_FLAG_...)` and
:cpp:expr:`lv_obj_remove_flag(widget, LV_OBJ_FLAG_...)`.

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



.. _lv_obj_events:

Base-Widget Events
******************

.. _widget_events:

Events from Input Devices
-------------------------
-  :cpp:enumerator:`LV_EVENT_PRESSED`              Widget has been pressed.
-  :cpp:enumerator:`LV_EVENT_PRESSING`             Widget is being pressed (sent continuously while pressing).
-  :cpp:enumerator:`LV_EVENT_PRESS_LOST`           Widget is still being pressed but slid cursor/finger off Widget.
-  :cpp:enumerator:`LV_EVENT_SHORT_CLICKED`        Widget was pressed for a short period of time, then released. Not sent if scrolled.
-  :cpp:enumerator:`LV_EVENT_SINGLE_CLICKED`       Sent for first short click within a small distance and short time.
-  :cpp:enumerator:`LV_EVENT_DOUBLE_CLICKED`       Sent for second short click within small distance and short time.
-  :cpp:enumerator:`LV_EVENT_TRIPLE_CLICKED`       Sent for third short click within small distance and short time.
-  :cpp:enumerator:`LV_EVENT_LONG_PRESSED`         Object has been pressed for at least `long_press_time`.  Not sent if scrolled.
-  :cpp:enumerator:`LV_EVENT_LONG_PRESSED_REPEAT`  Sent after `long_press_time` in every `long_press_repeat_time` ms.  Not sent if scrolled.
-  :cpp:enumerator:`LV_EVENT_CLICKED`              Sent on release if not scrolled (regardless to long press).
-  :cpp:enumerator:`LV_EVENT_RELEASED`             Sent in every cases when Widget has been released.
-  :cpp:enumerator:`LV_EVENT_SCROLL_BEGIN`         Scrolling begins. The event parameter is a pointer to the animation of the scroll. Can be modified.
-  :cpp:enumerator:`LV_EVENT_SCROLL_THROW_BEGIN`   Received when scrolling begins.
-  :cpp:enumerator:`LV_EVENT_SCROLL_END`           Scrolling ended.
-  :cpp:enumerator:`LV_EVENT_SCROLL`               Scrolling
-  :cpp:enumerator:`LV_EVENT_GESTURE`              A gesture is detected. Get gesture with `lv_indev_get_gesture_dir(lv_indev_active());`
-  :cpp:enumerator:`LV_EVENT_KEY`                  A key is sent to Widget. Get key with `lv_indev_get_key(lv_indev_active());`
-  :cpp:enumerator:`LV_EVENT_FOCUSED`              Widget received focus,
-  :cpp:enumerator:`LV_EVENT_DEFOCUSED`            Widget's focus has been lost.
-  :cpp:enumerator:`LV_EVENT_LEAVE`                Widget's focus has been lost but is still selected.
-  :cpp:enumerator:`LV_EVENT_HIT_TEST`             Perform advanced hit-testing.

Special Events
--------------
-  :cpp:enumerator:`LV_EVENT_VALUE_CHANGED` when the :cpp:enumerator:`LV_OBJ_FLAG_CHECKABLE` flag is
   enabled and the Widget was clicked (on transition to/from the checked state)

Drawing Events
--------------
-  :cpp:enumerator:`LV_EVENT_DRAW_MAIN`            Performing drawing of main part
-  :cpp:enumerator:`LV_EVENT_DRAW_MAIN_BEGIN`      Starting drawing of main part
-  :cpp:enumerator:`LV_EVENT_DRAW_MAIN_END`        Finishing drawing of main part
-  :cpp:enumerator:`LV_EVENT_DRAW_POST`            Perform the post draw phase (when all children are drawn)
-  :cpp:enumerator:`LV_EVENT_DRAW_POST_BEGIN`      Starting the post draw phase (when all children are drawn)
-  :cpp:enumerator:`LV_EVENT_DRAW_POST_END`        Finishing the post draw phase (when all children are drawn)

Other Events
------------
-  :cpp:enumerator:`LV_EVENT_DELETE`               Object is being deleted
-  :cpp:enumerator:`LV_EVENT_CHILD_CHANGED`        Child was removed, added, or its size, position were changed
-  :cpp:enumerator:`LV_EVENT_CHILD_CREATED`        Child was created, always bubbles up to all parents
-  :cpp:enumerator:`LV_EVENT_CHILD_DELETED`        Child was deleted, always bubbles up to all parents
-  :cpp:enumerator:`LV_EVENT_SIZE_CHANGED`         Object coordinates/size have changed
-  :cpp:enumerator:`LV_EVENT_STYLE_CHANGED`        Object's style has changed
-  :cpp:enumerator:`LV_EVENT_LAYOUT_CHANGED`       A child's position has changed due to a layout recalculation (when container has flex or grid layout style)
-  :cpp:enumerator:`LV_EVENT_GET_SELF_SIZE`        Get internal size of a widget

.. admonition::  Further Reading

    Learn more about :ref:`events`.



.. _lv_obj_keys:

Keys
****

If :cpp:enumerator:`LV_OBJ_FLAG_CHECKABLE` is enabled, :cpp:enumerator:`LV_KEY_RIGHT` and
:cpp:enumerator:`LV_KEY_UP` make the Widget checked, and :cpp:enumerator:`LV_KEY_LEFT` and
:cpp:enumerator:`LV_KEY_DOWN` make it unchecked.

If :cpp:enumerator:`LV_OBJ_FLAG_SCROLLABLE` is enabled, but the Widget is not editable
(as declared by the widget class), the arrow keys (:cpp:enumerator:`LV_KEY_UP`,
:cpp:enumerator:`LV_KEY_DOWN`, :cpp:enumerator:`LV_KEY_LEFT`, :cpp:enumerator:`LV_KEY_RIGHT`) scroll the Widget.
If the Widget can only scroll vertically, :cpp:enumerator:`LV_KEY_LEFT` and
:cpp:enumerator:`LV_KEY_RIGHT` will scroll up/down instead, making it compatible with
an encoder input device. See :ref:`Input devices overview <indev>` for
more on encoder behaviors and the edit mode.

.. admonition::  Further Reading

    Learn more about :ref:`indev_keys`.





.. _widget_snapshot:

Snapshot
********

A snapshot image can be generated for a Widget together with its
children. Check details in :ref:`snapshot`.



Example
*******

.. include:: ../../examples/widgets/obj/index.rst



.. _lv_obj_api:

API
***
