.. _objects:

=======
Objects
=======

In LVGL the **basic building blocks** of a user interface are the
objects, also called *Widgets*. For example a
:ref:`Button <lv_button>`, :ref:`Label <lv_label>`,
:ref:`Image <lv_image>`, :ref:`List <lv_list>`,
:ref:`Chart <lv_chart>` or :ref:`Text area <lv_textarea>`.

You can see all the :ref:`Object types <widgets>` here.

All objects are referenced using an :cpp:type:`lv_obj_t` pointer as a handle.
This pointer can later be used to set or get the attributes of the
object.

.. _objects_attributes:

Attributes
**********

Basic attributes
----------------

All object types share some basic attributes:

- Position
- Size
- Parent
- Styles
- Event handlers
- Etc

You can set/get these attributes with ``lv_obj_set_...`` and
``lv_obj_get_...`` functions. For example:

.. code:: c

   /*Set basic object attributes*/
   lv_obj_set_size(btn1, 100, 50);   /*Set a button's size*/
   lv_obj_set_pos(btn1, 20,30);      /*Set a button's position*/

To see all the available functions visit the :ref:`Base object's documentation <lv_obj>`.

Specific attributes
-------------------

The object types have special attributes too. For example, a slider has

- Minimum and maximum values
- Current value

For these special attributes, every object type may have unique API
functions. For example for a slider:

.. code:: c

   /*Set slider specific attributes*/
   lv_slider_set_range(slider1, 0, 100);                   /*Set the min. and max. values*/
   lv_slider_set_value(slider1, 40, LV_ANIM_ON);       /*Set the current value (position)*/

The API of the widgets is described in their
:ref:`Documentation <widgets>` but you can also check the respective
header files (e.g. *widgets/lv_slider.h*)

.. _objects_working_mechanisms:

Working mechanisms
******************

Parent-child structure
----------------------

A parent object can be considered as the container of its children.
Every object has exactly one parent object (except screens), but a
parent can have any number of children. There is no limitation for the
type of the parent but there are objects which are typically a parent
(e.g. button) or a child (e.g. label).

Moving together
---------------

If the position of a parent changes, the children will move along with
it. Therefore, all positions are relative to the parent.

.. image:: /misc/par_child1.png

.. code:: c

   lv_obj_t * parent = lv_obj_create(lv_screen_active());   /*Create a parent object on the current screen*/
   lv_obj_set_size(parent, 100, 80);                    /*Set the size of the parent*/

   lv_obj_t * obj1 = lv_obj_create(parent);             /*Create an object on the previously created parent object*/
   lv_obj_set_pos(obj1, 10, 10);                        /*Set the position of the new object*/

Modify the position of the parent:

.. image:: /misc/par_child2.png

.. code:: c

   lv_obj_set_pos(parent, 50, 50); /*Move the parent. The child will move with it.*/

(For simplicity the adjusting of colors of the objects is not shown in
the example.)

Visibility only on the parent
-----------------------------

If a child is partially or fully outside its parent then the parts
outside will not be visible.

.. image:: /misc/par_child3.png

.. code:: c

   lv_obj_set_x(obj1, -30);    /*Move the child a little bit off the parent*/

This behavior can be overwritten with
:cpp:expr:`lv_obj_add_flag(obj, LV_OBJ_FLAG_OVERFLOW_VISIBLE)` which allow the
children to be drawn out of the parent.

Create and delete objects
-------------------------

In LVGL, objects can be created and deleted dynamically at run time. It
means only the currently created (existing) objects consume RAM.

This allows for the creation of a screen just when a button is clicked
to open it, and for deletion of screens when a new screen is loaded.

UIs can be created based on the current environment of the device. For
example one can create meters, charts, bars and sliders based on the
currently attached sensors.

Every widget has its own **create** function with a prototype like this:

.. code:: c

   lv_obj_t * lv_<widget>_create(lv_obj_t * parent, <other parameters if any>);

Typically, the create functions only have a *parent* parameter telling
them on which object to create the new widget.

The return value is a pointer to the created object with :cpp:type:`lv_obj_t` ``*``
type.

There is a common **delete** function for all object types. It deletes
the object and all of its children.

.. code:: c

   void lv_obj_delete(lv_obj_t * obj);

:cpp:func:`lv_obj_delete` will delete the object immediately. If for any reason you
can't delete the object immediately you can use
:cpp:expr:`lv_obj_delete_async(obj)` which will perform the deletion on the next
call of :cpp:func:`lv_timer_handler`. This is useful e.g. if you want to
delete the parent of an object in the child's :cpp:enumerator:`LV_EVENT_DELETE`
handler.

You can remove all the children of an object (but not the object itself)
using :cpp:expr:`lv_obj_clean(obj)`.

You can use :cpp:expr:`lv_obj_delete_delayed(obj, 1000)` to delete an object after
some time. The delay is expressed in milliseconds.

Sometimes you're not sure whether an object was deleted and you need some way to
check if it's still "alive". Anytime before the object is deleted, you can use
cpp:expr:`lv_obj_null_on_delete(&obj)` to cause your object pointer to be set to ``NULL``
when the object is deleted.

Make sure the pointer variable itself stays valid until the object is deleted. Here
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

.. _objects_screens:

Screens
*******

Create screens
--------------

The screens are special objects which have no parent object. So they can
be created like:

.. code:: c

   lv_obj_t * scr1 = lv_obj_create(NULL);

Screens can be created with any object type. For example, a
:ref:`Base object <lv_obj>` or an image to make a wallpaper.

Get the active screen
---------------------

There is always an active screen on each display. By default, the
library creates and loads a "Base object" as a screen for each display.

To get the currently active screen use the :cpp:func:`lv_screen_active` function.

.. _objects_load_screens:

Load screens
------------

To load a new screen, use :cpp:expr:`lv_screen_load(scr1)`.

Layers
------

There are two automatically generated layers:

- top layer
- system layer

They are independent of the screens and they will be shown on every
screen. The *top layer* is above every object on the screen and the
*system layer* is above the *top layer*. You can add any pop-up windows
to the *top layer* freely. But, the *system layer* is restricted to
system-level things (e.g. mouse cursor will be placed there with
:cpp:func:`lv_indev_set_cursor`).

The :cpp:func:`lv_layer_top` and :cpp:func:`lv_layer_sys` functions return pointers
to the top and system layers respectively.

Read the :ref:`Layer overview <layers>` section to learn more
about layers.

Load screen with animation
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

Handling multiple displays
--------------------------

Screens are created on the currently selected *default display*. The
*default display* is the last registered display with
:cpp:func:`lv_display_create`. You can also explicitly select a new default
display using :cpp:expr:`lv_display_set_default(disp)`.

:cpp:func:`lv_screen_active`, :cpp:func:`lv_screen_load` and :cpp:func:`lv_screen_load_anim` operate
on the default display.

Visit :ref:`display_multi_display_support` to learn more.

.. _objects_parts:

Parts
*****

The widgets are built from multiple parts. For example a
:ref:`Base object <lv_obj>` uses the main and scrollbar parts but a
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

.. _objects_states:

States
******

The object can be in a combination of the following states:

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
interacts with an object (presses, releases, focuses, etc.). However,
the states can be changed manually too. To set or clear given state (but
leave the other states untouched) use
``lv_obj_add/remove_state(obj, LV_STATE_...)`` In both cases OR-ed state
values can be used as well. E.g.
:cpp:expr:`lv_obj_add_state(obj, part, LV_STATE_PRESSED | LV_PRESSED_CHECKED)`.

To learn more about the states read the related section of the
:ref:`Style overview <styles>`.

.. _objects_snapshot:

Snapshot
********

A snapshot image can be generated for an object together with its
children. Check details in :ref:`snapshot`.

.. _objects_api:

API
***
