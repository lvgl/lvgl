.. _lv_roller:

==================
Roller (lv_roller)
==================


Overview
********

Roller allows the end user to select an item from a list by scrolling it.
The item in the middle is the selected item and normally stands out from
the other items due to different styles applied to it.




.. _lv_roller_parts_and_styles:

Parts and Styles
****************

-  :cpp:enumerator:`LV_PART_MAIN` The background of the roller uses the :ref:`typical
   background <typical bg props>` and text style properties.

   - Style ``text_line_space`` adjusts the space between list items.
     Use :cpp:func:`lv_obj_set_style_text_line_space` to set this value.

   - When the Roller is scrolled and doesn't stop exactly on an item, it will
     automatically scroll to the nearest valid item in ``anim_time``
     milliseconds as specified in the ``anim_duration`` style.  Use
     :cpp:func:`lv_obj_set_style_anim_duration` to set this value.

-  :cpp:enumerator:`LV_PART_SELECTED` The selected item (displayed in the  middle of
   the Roller).  Besides the typical background properties, it uses text style
   properties to change the appearance of the text of the selected item.



.. _lv_roller_usage:

Usage
*****

Setting the list items
----------------------

List items are passed to the Roller as a string with
:cpp:expr:`lv_roller_set_options(roller, string_list, LV_ROLLER_MODE_NORMAL)`.
The items should be separated by ``\n``. For example:
``"First\nSecond\nThird"``.

:cpp:enumerator:`LV_ROLLER_MODE_INFINITE` makes the Roller circular.

You can select an option programmatically with
:cpp:expr:`lv_roller_set_selected(roller, id, LV_ANIM_ON)`,
where *id* is the zero-based index of the list item to be selected.

If you don't know the index of an option can also select an item with
:cpp:expr:`lv_roller_set_selected_str(roller, str, LV_ANIM_ON)`,
where *str* is the string equal to one of the list items.

Get selected option
-------------------

To get the *index* of the currently selected item use :cpp:expr:`lv_roller_get_selected(roller)`.

:cpp:expr:`lv_roller_get_selected_str(roller, buf, buf_size)` will copy the name of the selected item to ``buf``.

Visible rows
------------

The number of visible rows can be adjusted with :cpp:expr:`lv_roller_set_visible_row_count(roller, num)`.

This function calculates the height with the current style. If the font,
line space, border width, etc. of the Roller changes, this function needs
to be called again.



.. _lv_roller_events:

Events
******

-  :cpp:enumerator:`LV_EVENT_VALUE_CHANGED` Sent when a new list item is selected.

.. admonition::  Further Reading

    Learn more about :ref:`lv_obj_events` emitted by all Widgets.

    Learn more about :ref:`events`.



.. _lv_roller_keys:

Keys
****

-  ``LV_KEY_RIGHT/DOWN`` Select next option
-  ``LV_KEY_LEFT/UP`` Select previous option
-  :cpp:enumerator:`LY_KEY_ENTER` Accept the selected option (sends :cpp:enumerator:`LV_EVENT_VALUE_CHANGED` event)

.. admonition::  Further Reading

    Learn more about :ref:`indev_keys`.



.. _lv_roller_example:

Example
*******

.. include:: ../../examples/widgets/roller/index.rst



.. _lv_roller_api:

API
***
