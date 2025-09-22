===============
API Conventions
===============

In most of the cases the API functions of LVGL widgets are structures like:

- ``lv_ + <widget_name> + create(parent)``
- ``lv_ + <widget_name> + set + <property>(widget, value)``
- ``lv_ + <widget_name> + get + <property>(widget)``
- ``lv_ + <widget_name> + add + <property>(widget)``


Basic attributes
****************

All Widget types share some basic attributes:

- Position
- Size
- Parent
- Styles
- Events callbacks
- Flags like *Clickable*, *Scrollable*, etc.
- Etc.

You can set/get these attributes with ``lv_obj_set_...`` and
``lv_obj_get_...`` functions. For example:

.. code-block:: c

   /* Set basic Widget attributes */
   lv_obj_set_size(btn1, 100, 50);   /* Set a button's size */
   lv_obj_set_pos(btn1, 20,30);      /* Set a button's position */

For complete details on position, size, coordinates and layouts, see :ref:`coord`.


Widget-specific attributes
**************************

The Widget types have special attributes as well. For example, a slider has

- Minimum and maximum values
- Current value

For these special attributes, every Widget type may have unique API
functions. For example for a :ref:`Slider <lv_slider>`:

.. code-block:: c

   /* Set slider specific attributes */
   lv_slider_set_range(slider1, 0, 100);               /* Set the min. and max. values */
   lv_slider_set_value(slider1, 40, LV_ANIM_ON);       /* Set the current value (position) */

The API of the widgets is described in their
:ref:`Documentation <widgets>` but you can also check the respective
header files (e.g. *widgets/lv_slider.h*)


Widget Creation
***************

Widgets can be created and deleted dynamically at run time. It
means only the currently created (existing) Widgets consume RAM.

This allows for the creation of a Screen just when a button is clicked
to open it, and for deletion of Screens when a new Screen is loaded.

UIs can be created based on the current environment of the device. For
example one can create meters, charts, bars and sliders based on the
currently attached sensors.

Every widget has its own **create** function with a prototype like this:

.. code-block:: c

   lv_obj_t * lv_<widget>_create(lv_obj_t * parent

The create functions only have a ``parent`` parameter telling on which Widget to create the new Widget.

The return value is a pointer to the created Widget with ``lv_obj_t *`` type.

Widget Deletion
***************

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

By calling cpp:expr:`lv_obj_null_on_delete(&widget)` the ``lv_obj_t *``
variable of the widget will be NULL-ed when the widget is deleted.
This way it's easy to cehc the widget exists or not.

This in a example for some of functions above:

.. code:: c

   static lv_obj_t * my_label; /*static in the file so it stays calid*/

   /*Call it every 2000 ms*/
   void some_timer_callback(lv_timer_t * t)
   {
      /*If the label is not created yet, creat it, and also delete it 1000 ms.*/
      if(my_label == NULL) {
         my_label = lv_label_create(lv_screen_active());
         lv_obj_delete_delayed(my_label, 1000);
         lv_obj_null_on_delete(&my_label);
      }
      /*Move the label if it exists*/
      else {
         lv_obj_set_x(my_label, lv_obj_get_x(my_label) + 1);
      }
   }


