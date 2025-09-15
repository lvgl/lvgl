.. _lv_dropdown:

============================
Drop-Down List (lv_dropdown)
============================

Overview
********

The Drop-Down List allows the user to select a value from a list.

The Drop-Down List is closed by default and displays a single value or
predefined text. When activated (by click on the Drop-Down List), a list
is created from which the user may select one item. When the user
selects a new value, the list is deleted again.

The Drop-down list is added to the default group (if one is set).
The Drop-down list is an editable Widget allowing list-item selection via
encoder or keyboard navigation as well.


.. _lv_dropdown_parts_and_styles:

Parts and Styles
****************

The Drop-Down List Widget is built from the elements: "button" and "list"
(lightweight versions of the Button and List Widgets).

Button
------

-  :cpp:enumerator:`LV_PART_MAIN` Background of button, uses the :ref:`typical
   background <typical bg props>` and text style properties for its text.
-  :cpp:enumerator:`LV_PART_INDICATOR` Typically an arrow symbol that can be an Image
   or text (e.g. :cpp:enumerator:`LV_SYMBOL`).

The button goes to :cpp:enumerator:`LV_STATE_CHECKED` when it's opened.

List
----

-  :cpp:enumerator:`LV_PART_MAIN` The list itself; uses the :ref:`typical background
   style properties <typical bg props>`. ``max_height`` can be used to limit the
   height of the list.
-  :cpp:enumerator:`LV_PART_SCROLLBAR` The scrollbar background, border, shadow
   properties and width (for its own width) and right padding for the
   spacing on the right.
-  :cpp:enumerator:`LV_PART_SELECTED` Refers to the currently pressed, checked or
   pressed+checked option.  Also uses the :ref:`typical background style properties
   <typical bg props>`.

The list is shown/hidden on open/close. To add styles to it use
:cpp:expr:`lv_dropdown_get_list(dropdown)` to get the list object.  Example:

.. code-block:: c

   lv_obj_t * list = lv_dropdown_get_list(dropdown) /* Get list */
   lv_obj_add_style(list, &my_style, selector)      /* Add styles to list */

Alternatively the theme can be extended with new styles.


.. _lv_dropdown_usage:

Usage
*****


.. _lv_dropdown_list_items:

List items
----------

The list items are passed to the Drop-Down List as a newline-separated list in a string
as the ``options`` argument to :cpp:expr:`lv_dropdown_set_options(dropdown, options)`.
Each list item should be separated by ``\n``.  Example: ``"First\nSecond\nThird"``.
This string is copied by the Drop-Down List, so its contents do not need to remain
available beyond this call.

The :cpp:expr:`lv_dropdown_add_option(dropdown, "New option", pos)` function
inserts a new option at index ``pos``.

To save memory the options can be set from a static (const) string as well
with :cpp:expr:`lv_dropdown_set_options_static(dropdown, options)`. In this case
the options string's contents must remain available for the life of the Drop-Down
List and :cpp:func:`lv_dropdown_add_option` cannot be used.

You can select an option programmatically with
:cpp:expr:`lv_dropdown_set_selected(dropdown, id)`, where ``id`` is the index of
the target option.

Get selected option
-------------------

To get the *index* of the selected option, use
:cpp:expr:`lv_dropdown_get_selected(dropdown)`.

:cpp:expr:`lv_dropdown_get_selected_str(dropdown, buf, buf_size)` copies the
*name* of the selected option to ``buf``.

Direction
---------

The list can be created on any side. The default :cpp:enumerator:`LV_DIR_BOTTOM` can
be modified using :cpp:expr:`lv_dropdown_set_dir(dropdown, LV_DIR_LEFT)`.

If the list would be vertically out of the screen, it will be aligned to
the edge.

Symbol
------

A symbol (typically an arrow) can be added to the Drop-Down List with
:cpp:expr:`lv_dropdown_set_symbol(dropdown, LV_SYMBOL_...)`

If the direction of the Drop-Down List is :cpp:enumerator:`LV_DIR_LEFT` the symbol
will be shown on the left, otherwise on the right.

Show selected
-------------

The main part can either show the selected item or static text. If
static is set with :cpp:expr:`lv_dropdown_set_text(dropdown, "Some text")` it
will be shown regardless of the selected item. If the text is ``NULL``
the selected option is displayed on the button.

Programmatically open/close
---------------------------

To programmatically open or close the Drop-Down List use
:cpp:expr:`lv_dropdown_open(dropdown)` or :cpp:expr:`lv_dropdown_close(dropdown)`.


Data binding
------------

To get familiar with observers, subjects, and data bindings in general visit the
:ref:`Observer <observer_how_to_use>` page.

This method of subscribing to an integer Subject affects a Drop-Down Widget's integer
value directly.  Note that this is a two-way binding (Subject <===> Widget) so an end
user's direct interaction with the Drop-Down Widget updates the Subject's value and
vice versa.

It support only integer Subjects.

- :cpp:expr:`lv_dropdown_bind_value(dropdown, &subject)`


.. _lv_dropdown_events:

Events
******

- :cpp:enumerator:`LV_EVENT_VALUE_CHANGED` Sent when a new option is selected or the list is opened/closed.
- :cpp:enumerator:`LV_EVENT_CANCEL` Sent when list is closed.
- :cpp:enumerator:`LV_EVENT_READY` Sent when list is opened.

.. admonition::  Further Reading

    Learn more about :ref:`lv_obj_events` emitted by all Widgets.

    Learn more about :ref:`events`.



.. _lv_dropdown_keys:

Keys
****

-  ``LV_KEY_RIGHT/DOWN`` Select next list item.
-  ``LV_KEY_LEFT/UP`` Select previous list item.
-  :cpp:enumerator:`LV_KEY_ENTER` Apply selected list item (sends
   :cpp:enumerator:`LV_EVENT_VALUE_CHANGED` event and closes Drop-Down List).

.. admonition::  Further Reading

    Learn more about :ref:`indev_keys`.



.. _lv_dropdown_example:

Example
*******

.. include:: ../../examples/widgets/dropdown/index.rst



.. _lv_dropdown_api:

API
***
