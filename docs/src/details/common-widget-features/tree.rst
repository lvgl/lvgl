
===========
Widget Tree
===========

Parents and Children
********************

LVGL stores the widgets in a parent-child structure (tree).
The root element is screen which has no parent.

When a widget is created a pointer to its parent is passed to the create function.

LVGL provides many useful function the modify the widget tree and get information about it:

- :cpp:expr:`lv_obj_get_screen(widget)`: Get a Widget's Screen ("root" parent).
- :cpp:expr:`lv_obj_get_parent(widget)`: Gets a Widget's current parent.
- `lv_obj_set_parent(widget, new_parent)`: Moves the Widget to a new parent.
   The widget will be the top-most (last/youngest child of the new parent)
- :cpp:expr:`lv_obj_get_child(parent, idx)`: Returns a specific child of a parent.
  Some examples for ``idx``:

     - ``0`` get the child created first
     - ``1`` get the child created second
     - ``-1`` get the child created last
- :cpp:expr:`lv_obj_find_by_name(parent, "text")`: Find a Widget with a given name on a parent. Doesn't have to be the direct children.
- :cpp:expr:`lv_obj_get_child_by_name(parent, "container/button/text")`: Gets the Widget considering parent names too (separated by ``/``)
- :cpp:expr:`lv_obj_get_index(widget)` returns the index of the Widget in its parent.
It is equivalent to the number of older children in the parent.
- :cpp:expr:`lv_obj_move_to_index(widget, idx)`: Move the widget to an index. 0: is the oldest child's position, -1 (the first from the back)
  is the youngest.
- :cpp:expr:`lv_obj_swap(widget1, widget2)`: Swaps the position of two Widgets.
- To iterate through a parent Widget's children:

.. code-block:: c

    uint32_t i;
    for(i = 0; i < lv_obj_get_child_count(parent); i++) {
        lv_obj_t * child = lv_obj_get_child(parent, i);
        /* Do something with `child`. */
    }



.. _widget_names:

Names
*****

When a widget is created, its reference can be stored in an :cpp:expr:`lv_obj_t *` pointer
variable. To use this widget in multiple places in the code, the variable can be passed
as a function parameter or made a global variable. However, this approach has some drawbacks:

- Using global variables is not clean and generally not recommended.
- It's not scalable. Passing references to 20 widgets as function parameters is not ideal.
- It's hard to track whether a widget still exists or has been deleted.


Setting names
-------------

To address these issues, LVGL introduces a powerful widget naming system that can be enabled
by setting ``LV_USE_OBJ_NAME`` in ``lv_conf.h``.

A custom name can be assigned using :cpp:expr:`lv_obj_set_name(obj, "name")` or
:cpp:expr:`lv_obj_set_name_static(obj, "name")`. The "static" variant means the passed name
must remain valid while the widget exists, as only the pointer is stored. Otherwise, LVGL will
allocate memory to store a copy of the name.

If a name ends with ``#``, LVGL will automatically replace it with an index based on the
number of siblings with the same base name. If no name is provided, the default is
``<widget_type>_#``.

Below is an example showing how manually- and automatically-assigned names are resolved:

- Main ``lv_obj`` container named ``"cont"``: "cont"

  - ``lv_obj`` container named ``"header"``: "header"

    - ``lv_label`` with no name: "lv_label_0"
    - ``lv_label`` named ``"title"``: "title"
    - ``lv_label`` with no name: "lv_label_1" (It's the third label, but custom-named widgets are not counted)

  - ``lv_obj`` container named ``"buttons"``:

    - ``lv_button`` with no name: "lv_button_0"
    - ``lv_button`` named ``"second_button"``: "second_button"
    - ``lv_button`` with no name: "lv_button_1"
    - ``lv_button`` named ``lv_button_#``: "lv_button_2"
    - ``lv_button`` named ``mybtn_#``: "mybtn_0"
    - ``lv_button`` with no name: "lv_button_2"
    - ``lv_button`` named ``mybtn_#``: "mybtn_1"
    - ``lv_button`` named ``mybtn_#``: "mybtn_2"
    - ``lv_button`` named ``mybtn_#``: "mybtn_3"


Finding widgets
---------------

Widgets can be found by name in two ways:

1. **Get a direct child by name** using :cpp:expr:`lv_obj_get_child_by_name(parent, "child_name")`.
   Example:
   :cpp:expr:`lv_obj_get_child_by_name(header, "title")`
   You can also use a "path" to find nested children:
   :cpp:expr:`lv_obj_get_child_by_name(cont, "buttons/mybtn_2")`

2. **Find a descendant at any level** using :cpp:expr:`lv_obj_find_by_name(parent, "child_name")`.
   Example:
   :cpp:expr:`lv_obj_find_by_name(cont, "mybtn_1")`
   Note that ``"mybtn_1"`` is a child of ``buttons``, not a direct child of ``cont``.
   This is useful when you want to ignore hierarchy and search by name alone.

Since both functions start searching from a specific parent, it’s possible to have multiple widget
subtrees with identical names under different parents.

For example, if ``my_listitem_create(parent)`` creates a widget named ``"list_item_#"``
with children like ``"icon"``, ``"title"``, ``"ok_button"``, and ``"lv_label_0"``,
and it's called 10 times, a specific ``"ok_button"`` can be found like this:

.. code-block:: c

    lv_obj_t * item = lv_obj_find_by_name(lv_screen_active(), "list_item_5");
    lv_obj_t * ok_btn = lv_obj_find_by_name(item, "ok_button");

    // Or
    lv_obj_t * ok_btn = lv_obj_get_child_by_name(some_list_container, "list_item_5/ok_button");

Names are resolved **when they are retrieved**, not when they are set.
This means the indices always reflect the current state of the widget tree
at the time the name is used.

