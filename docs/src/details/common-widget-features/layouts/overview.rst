.. _layout_overview:

========
Overview
========

Layouts assigned to a Widget manage the positions and in some cases the sizes of that
Widget's children. They can be used to automatically arrange the children into a line,
a column, or into more complicated patterns.

The position and size set by the layout overwrites the "normal" x, y, and in some
cases the width, and height settings.

There is only one function that is the same for every layout:
:cpp:func:`lv_obj_set_layout` ``(widget, <LAYOUT_NAME>)`` sets the layout on a Widget.
For further settings of the parent and children see the documentation of the given
layout.



Built-In Layouts
****************

LVGL comes with two very powerful layouts:

* Flexbox: arranges child Widgets into rows or columns, with support for wrapping and
  expanding items.
* Grid: arranges child Widgets into fixed positions in a 2D table.

Both are heavily inspired by the CSS layouts with the same name. Layouts are described
in detail in their own section of documentation.



Flags
*****

There are some flags that can be used on Widgets to affect how they behave with
layouts:

- :cpp:enumerator:`LV_OBJ_FLAG_HIDDEN` Hidden Widgets are ignored in layout
  calculations.
- :cpp:enumerator:`LV_OBJ_FLAG_IGNORE_LAYOUT` The Widget is simply ignored by the
  layouts. Its coordinates can be set as usual.
- :cpp:enumerator:`LV_OBJ_FLAG_FLOATING` Same as
  :cpp:enumerator:`LV_OBJ_FLAG_IGNORE_LAYOUT` but the Widget with
  :cpp:enumerator:`LV_OBJ_FLAG_FLOATING` will be ignored in
  :c:macro:`LV_SIZE_CONTENT` calculations.

These flags can be added/removed with :cpp:expr:`lv_obj_add_flag(widget, FLAG)` and
:cpp:expr:`lv_obj_remove_flag(widget, FLAG)`



Adding New Layouts
******************

LVGL can be freely extended by a custom layout like this:

.. code-block:: c

   uint32_t MY_LAYOUT;

   ...

   MY_LAYOUT = lv_layout_register(my_layout_update, &user_data);

   ...

   void my_layout_update(lv_obj_t * widget, void * user_data)
   {
       /* Will be called automatically if it's required to reposition/resize the children of "widget" */
   }

Custom style properties can be added which can be retrieved and used in the update
callback. For example:

.. code-block:: c

   uint32_t LV_STYLE_MY_PROP;
   ...

   LV_STYLE_MY_PROP = lv_style_register_prop(LV_STYLE_PROP_FLAG_NONE);

   ...
   static inline void lv_style_set_my_prop(lv_style_t * style, uint32_t value)
   {
       lv_style_value_t v = {
           .num = (int32_t)value
       };
       lv_style_set_prop(style, LV_STYLE_MY_PROP, v);
   }
