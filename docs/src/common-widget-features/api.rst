================
API Conventions
================

In most cases, the API functions of LVGL widgets are structured like:

- ``lv_ + <widget_name> + create(parent)``
- ``lv_ + <widget_name> + set + <property>(widget, value)``
- ``lv_ + <widget_name> + get + <property>(widget)``
- ``lv_ + <widget_name> + add + <property>(widget)``



Basic Attributes
****************

All widget types share some basic attributes:

- Position
- Size
- Parent
- Styles
- Event callbacks
- Flags like *Clickable*, *Scrollable*, etc.
- Etc.

You can set/get these attributes with ``lv_obj_set_...`` and
``lv_obj_get_...`` functions. For example:

.. code-block:: c

   /* Set basic widget attributes */
   lv_obj_set_size(btn1, 100, 50);   /* Set a button's size */
   lv_obj_set_pos(btn1, 20, 30);     /* Set a button's position */

For complete details on position, size, coordinates, and layouts, see :ref:`coord`.



Widget-Specific Attributes
**************************

The widget types have special attributes as well. For example, a slider has:

- Minimum and maximum values
- Current value

For these special attributes, every widget type may have unique API functions. For
example, for a :ref:`Slider <lv_slider>`:

.. code-block:: c

   /* Set slider-specific attributes */
   lv_slider_set_range(slider1, 0, 100);               /* Set the min and max values */
   lv_slider_set_value(slider1, 40, LV_ANIM_ON);       /* Set the current value (position) */

The API of the Widgets is described in their :ref:`Documentation <widgets>`, but you
can also consult each Widget's respective header file (e.g., *widgets/lv_slider.h*) to
find a quick reference to function prototypes with brief documentation about each.



Widget Creation
***************

Widgets can be created and deleted dynamically at runtime. Only currently created
(existing) Widgets consume RAM.

This allows you to create a Screen only when a button is clicked to open it, and to
delete Screens when a new screen is loaded.

UIs can be created based on the current environment of the device. For example, you
can create meters, charts, bars, and sliders based on the currently attached sensors.

Every widget has its own **create** function with a prototype like this:

.. code-block:: c

   lv_obj_t * lv_<widget>_create(lv_obj_t * parent)

The create functions only have a ``parent`` parameter specifying on which widget to
create the new widget.

The return value is a pointer to the created widget of type ``lv_obj_t *``.



Widget Deletion
***************

There is a common **delete** function for all widget types. It deletes the widget and
all of its children.

.. code-block:: c

   void lv_obj_delete(lv_obj_t * widget);

:cpp:func:`lv_obj_delete` deletes the widget immediately. If for any reason you can't
delete the widget immediately, you can use :cpp:expr:`lv_obj_delete_async(widget)`
which will perform the deletion on the next call of :cpp:func:`lv_timer_handler`. This
is useful, for example, if you want to delete the parent of a widget in the child's
:cpp:enumerator:`LV_EVENT_DELETE` handler. Once deleted, the RAM a Widget occupies is
freed.

You can remove all the children of a widget (but not the widget itself) using
:cpp:expr:`lv_obj_clean(widget)`.

You can use :cpp:expr:`lv_obj_delete_delayed(widget, 1000)` to delete a widget after
some time. The delay is expressed in milliseconds.

By calling :cpp:expr:`lv_obj_null_on_delete(&widget)`, the ``lv_obj_t *`` variable of
the widget will be set to NULL when the widget is deleted. This makes it easy to check
whether the widget exists or not.

Here is an example that uses some of the functions above:

.. code:: c

   static lv_obj_t * my_label; /* Static in the file so it stays valid */

   /* Call it every 2000 ms */
   void some_timer_callback(lv_timer_t * t)
   {
      /* If the label is not created yet, create it and also delete it after 1000 ms */
      if(my_label == NULL) {
         my_label = lv_label_create(lv_screen_active());
         lv_obj_delete_delayed(my_label, 1000);
         lv_obj_null_on_delete(&my_label);
      }
      /* Move the label if it exists */
      else {
         lv_obj_set_x(my_label, lv_obj_get_x(my_label) + 1);
      }
   }
