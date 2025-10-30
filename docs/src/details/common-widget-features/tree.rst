.. _obj_tree:

===========
Widget Tree
===========

Parents and Children
********************

LVGL stores Widgets in a parent-child structure (tree).
The root element is the Screen, which has no parent.

When a Widget is created, a pointer to its parent is passed to the create function.

LVGL provides many useful functions to modify the Widget tree and get information about it:

- :cpp:expr:`lv_obj_get_screen(widget)`: Get a Widget's screen ("root" parent).
- :cpp:expr:`lv_obj_get_parent(widget)`: Get the Widget's current parent.
- `lv_obj_set_parent(widget, new_parent)`: Move the Widget to a new parent.
  The Widget will become the top-most (last/youngest) child of the new parent.
- :cpp:expr:`lv_obj_get_child(parent, idx)`: Return a specific child of a parent.
  Some examples for ``idx``:

  - ``0``: get the first created child
  - ``1``: get the second created child
  - ``-1``: get the last created child
- :cpp:expr:`lv_obj_get_child_by_type(parent, idx, &lv_slider_class)`: Similar to :cpp:expr:`lv_obj_get_child` but filters to children
  with a given type.
- :cpp:expr:`lv_obj_find_by_name(parent, "text")`: Find a Widget with a given name under a parent. It does not have to be a direct child.
- :cpp:expr:`lv_obj_get_child_by_name(parent, "container/button/text")`: Get a Widget by navigating a path using parent names (separated by ``/``).
- :cpp:expr:`lv_obj_get_index(widget)`: Return the index of the Widget in its parent.
  It is equivalent to the number of older siblings in the parent.
- :cpp:expr:`lv_obj_move_to_index(widget, idx)`: Move the Widget to a specified index.
  ``0`` is the oldest child's position, ``-1`` is the youngest.
- :cpp:expr:`lv_obj_swap(widget1, widget2)`: Swap the positions of two Widgets.

To iterate through a parent widget's children:

.. code-block:: c

    uint32_t i;
    uint32_t count = lv_obj_get_child_count(parent);
    for(i = 0; i < count; i++) {
        lv_obj_t * child = lv_obj_get_child(parent, i);
        /* Do something with `child`. */
    }



.. _widget_names:

Names
*****

When a Widget is created, its reference can be stored in an :cpp:expr:`lv_obj_t *` pointer
variable. To use this Widget in multiple places in the code, the variable can be passed
as a function parameter or made global. However, this approach has some drawbacks:

- Using global variables adds names to the global namespace and is thus generally not recommended.
- It's not scalable. Passing references to 20 Widgets as function parameters is not ideal.
- Tracking whether a Widget still exists or has been deleted requires extra logic and can be tricky.


Setting Names
-------------

To address these issues, LVGL introduces a powerful Widget naming system that can be enabled
by setting ``LV_USE_OBJ_NAME`` in ``lv_conf.h``.

A custom name can be assigned using :cpp:expr:`lv_obj_set_name(obj, "name")` or
:cpp:expr:`lv_obj_set_name_static(obj, "name")`. The "static" variant requires that the passed
name remains valid while the Widget exists, since only the pointer is stored. Otherwise, LVGL will
allocate memory to store a copy of the name.

If a name ends with ``#``, LVGL will automatically replace it with an index based on the
number of siblings with the same base name. If no name is provided, the default is
``<widget_type>_#``.

Below is an example showing how manually and automatically assigned names are resolved:

- Main ``lv_obj`` container named ``"cont"``: "cont"

  - ``lv_obj`` container named ``"header"``: "header"

    - ``lv_label`` with no name: "lv_label_0"
    - ``lv_label`` named ``"title"``: "title"
    - ``lv_label`` with no name: "lv_label_1" (It's the third label, but custom-named Widgets are not counted)

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


Finding Widgets
---------------

Widgets can be found by name in two ways:

1. **Get a direct child by name** using :cpp:expr:`lv_obj_get_child_by_name(parent, "child_name")`.
   Example:
   :cpp:expr:`lv_obj_get_child_by_name(header, "title")`.
   You can also use a path to find nested children:
   :cpp:expr:`lv_obj_get_child_by_name(cont, "buttons/mybtn_2")`.

2. **Find a descendant at any level** using :cpp:expr:`lv_obj_find_by_name(parent, "child_name")`.
   Example:
   :cpp:expr:`lv_obj_find_by_name(cont, "mybtn_1")`
   Note that ``"mybtn_1"`` is a child of ``cont`` at any level, not necessarily a
   direct child.  This is useful when you want to ignore the hierarchy and search by
   name alone.

Since both functions start searching from a specific parent, it's possible to have
multiple Widget subtrees with identical names under different parents.

For example, if ``my_listitem_create(parent)`` creates a Widget named ``"list_item_#"``
with direct children ``"icon"``, ``"title"``, ``"ok_button"``, and ``"lv_label_0"``,
and it is called 10 times, a specific ``"ok_button"`` can be found like this:

.. code-block:: c

    lv_obj_t * item = lv_obj_find_by_name(lv_screen_active(), "list_item_5");
    lv_obj_t * ok_btn = lv_obj_find_by_name(item, "ok_button");

    // Or
    lv_obj_t * ok_btn = lv_obj_get_child_by_name(some_list_container, "list_item_5/ok_button");

.. caution::

    When a Widget is named with a trailing "#", the index used for name comparison is
    computed in #1 and #2 above *when those calls are made*, thus always reflecting
    the current state of the Widget tree.  Deletion of a sibling Widget otherwise
    having the same name, with a smaller "apparent" index, will cause this index to
    change.
