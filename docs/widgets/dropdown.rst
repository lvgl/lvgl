.. _lv_dropdown:

============================
Drop-down list (lv_dropdown)
============================

Overview
********

The drop-down list allows the user to select one value from a list.

The drop-down list is closed by default and displays a single value or a
predefined text. When activated (by click on the drop-down list), a list
is created from which the user may select one option. When the user
selects a new value, the list is deleted again.

The Drop-down list is added to the default group (if it is set). Besides
the Drop-down list is an editable object to allow selecting an option
with encoder navigation too.

.. _lv_dropdown_parts_and_styles:

Parts and Styles
****************

The Dropdown widget is built from the elements: "button" and "list"
(both not related to the button and list widgets)

Button
------

-  :cpp:enumerator:`LV_PART_MAIN` The background of the button. Uses the typical
   background properties and text properties for the text on it.
-  :cpp:enumerator:`LV_PART_INDICATOR` Typically an arrow symbol that can be an image
   or a text (:cpp:enumerator:`LV_SYMBOL`).

The button goes to :cpp:enumerator:`LV_STATE_CHECKED` when it's opened.

List
----

-  :cpp:enumerator:`LV_PART_MAIN` The list itself. Uses the typical background
   properties. ``max_height`` can be used to limit the height of the
   list.
-  :cpp:enumerator:`LV_PART_SCROLLBAR` The scrollbar background, border, shadow
   properties and width (for its own width) and right padding for the
   spacing on the right.
-  :cpp:enumerator:`LV_PART_SELECTED` Refers to the currently pressed, checked or
   pressed+checked option. Also uses the typical background properties.

The list is hidden/shown on open/close. To add styles to it use
:cpp:expr:`lv_dropdown_get_list(dropdown)` to get the list object. For example:

.. code:: c

   lv_obj_t * list = lv_dropdown_get_list(dropdown) /*Get the list*/
   lv_obj_add_style(list, &my_style, selector)      /*Add the styles to the list*/

Alternatively the theme can be extended with the new styles.

.. _lv_dropdown_usage:

Usage
*****

.. _lv_dropdown_options:

Options
*******

Set options
-----------

Options are passed to the drop-down list as a string with
:cpp:expr:`lv_dropdown_set_options(dropdown, options)`. Options should be
separated by ``\n``. For example: ``"First\nSecond\nThird"``. This
string will be saved in the drop-down list, so it can in a local
variable.

The :cpp:expr:`lv_dropdown_add_option(dropdown, "New option", pos)` function
inserts a new option to ``pos`` index.

To save memory the options can set from a static(constant) string too
with :cpp:expr:`lv_dropdown_set_static_options(dropdown, options)`. In this case
the options string should be alive while the drop-down list exists and
:cpp:func:`lv_dropdown_add_option` can't be used

You can select an option manually with
:cpp:expr:`lv_dropdown_set_selected(dropdown, id)`, where ``id`` is the index of
an option.

Get selected option
-------------------

The get the *index* of the selected option, use
:cpp:expr:`lv_dropdown_get_selected(dropdown)`.

:cpp:expr:`lv_dropdown_get_selected_str(dropdown, buf, buf_size)` copies the
*name* of the selected option to ``buf``.

Direction
---------

The list can be created on any side. The default :cpp:enumerator:`LV_DIR_BOTTOM` can
be modified by :cpp:expr:`lv_dropdown_set_dir(dropdown, LV_DIR_LEFT)` function.

If the list would be vertically out of the screen, it will be aligned to
the edge.

Symbol
------

A symbol (typically an arrow) can be added to the dropdown list with
:cpp:expr:`lv_dropdown_set_symbol(dropdown, LV_SYMBOL_...)`

If the direction of the drop-down list is :cpp:enumerator:`LV_DIR_LEFT` the symbol
will be shown on the left, otherwise on the right.

Show selected
-------------

The main part can either show the selected option or a static text. If a
static is set with :cpp:expr:`lv_dropdown_set_text(dropdown, "Some text")` it
will be shown regardless to th selected option. If the text is ``NULL``
the selected option is displayed on the button.

Manually open/close
-------------------

To manually open or close the drop-down list the
``lv_dropdown_open/close(dropdown)`` function can be used.

.. _lv_dropdown_events:

Events
******

Apart from the `Generic events <../overview/event.html#generic-events>`__, the following
`Special events <../overview/event.html#special-events>`__ are sent by
the drop-down list:

- :cpp:enumerator:`LV_EVENT_VALUE_CHANGED` Sent when the new option is selected or the list is opened/closed.
- :cpp:enumerator:`LV_EVENT_CANCEL` Sent when the list is closed
- :cpp:enumerator:`LV_EVENT_READY` Sent when the list is opened

See the events of the :ref:`Base object <lv_obj>` too.

Learn more about :ref:`events`.

.. _lv_dropdown_keys:

Keys
****

-  ``LV_KEY_RIGHT/DOWN`` Select the next option.
-  ``LV_KEY_LEFT/UP`` Select the previous option.
-  :cpp:enumerator:`LV_KEY_ENTER` Apply the selected option (Sends
   :cpp:enumerator:`LV_EVENT_VALUE_CHANGED` event and closes the drop-down list).

Learn more about :ref:`indev_keys`.

.. _lv_dropdown_example:

Example
*******

.. include:: ../examples/widgets/dropdown/index.rst

.. _lv_dropdown_api:

API
***
