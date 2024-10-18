.. _widget_basics:

=============
Widget Basics
=============

The following details apply to all types of Widgets.



What is a Widget?
*****************
In LVGL a Widget is the **basic building block** of the user interface.
The most fundamental of all of Widgets is the :ref:`Base Widget <lv_obj>`, on which
all other widgets are based.

Examples:  :ref:`Base Widget (and Screen) <lv_obj>`,
:ref:`Button <lv_button>`, :ref:`Label <lv_label>`,
:ref:`Image <lv_image>`, :ref:`List <lv_list>`,
:ref:`Chart <lv_chart>` and :ref:`Text Area <lv_textarea>`.

See :ref:`widgets` to see all Widget types.

All Widgets are referenced using an :cpp:type:`lv_obj_t` pointer as a handle.
This pointer can later be used to read or change the Widget's attributes.



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
- Event handlers
- Etc

You can set/get these attributes with ``lv_obj_set_...`` and
``lv_obj_get_...`` functions. For example:

.. code-block:: c

   /* Set basic Widget attributes */
   lv_obj_set_size(btn1, 100, 50);   /* Set a button's size */
   lv_obj_set_pos(btn1, 20,30);      /* Set a button's position */

To see the features available to all widgets, see :ref:`Base Widget's documentation <lv_obj>`.


Specific attributes
-------------------

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



.. _widget_working_mechanisms:

Working Mechanisms
******************

Parent-child structure
----------------------

A parent Widget can be considered as the container of its children.
Every Widget has exactly one parent Widget (except Screens), but a
parent can have any number of children. There is no limitation for the
type of the parent but there are Widgets which are typically a parent
(e.g. button) or a child (e.g. label).

Moving together
---------------

If the position of a parent changes, the children will move along with
it. Therefore, all positions are relative to the parent.

.. image:: /misc/par_child1.png

.. code-block:: c

   lv_obj_t * parent = lv_obj_create(lv_screen_active());  /* Create a parent Widget on the current screen */
   lv_obj_set_size(parent, 100, 80);                       /* Set the size of the parent */

   lv_obj_t * obj1 = lv_obj_create(parent);                /* Create a Widget on the previously created parent Widget */
   lv_obj_set_pos(widget1, 10, 10);                        /* Set the position of the new Widget */

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
children to be drawn out of the parent.

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
:ref:`Base Widget <lv_obj>` uses the main and scrollbar parts but a
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



.. _widget_snapshot:

Snapshot
********

A snapshot image can be generated for a Widget together with its
children. Check details in :ref:`snapshot`.


.. _objects_api:

API
***
