.. _lv_roller:

==================
Roller (lv_roller)
==================

Overview
********

Roller allows you to simply select one option from a list by scrolling.

.. _lv_roller_parts_and_styles:

Parts and Styles
****************

-  :cpp:enumerator:`LV_PART_MAIN` The background of the roller uses all the typical
   background properties and text style properties.
   ``style_text_line_space`` adjusts the space between the options. When
   the Roller is scrolled and doesn't stop exactly on an option it will
   scroll to the nearest valid option automatically in ``anim_time``
   milliseconds as specified in the style.
-  :cpp:enumerator:`LV_PART_SELECTED` The selected option in the middle. Besides the
   typical background properties it uses the text style properties to
   change the appearance of the text in the selected area.

.. _lv_roller_usage:

Usage
*****

Set options
-----------

Options are passed to the Roller as a string with
:cpp:expr:`lv_roller_set_options(roller, options, LV_ROLLER_MODE_NORMAL)`.
The options should be separated by ``\n``. For example:
``"First\nSecond\nThird"``.

:cpp:enumerator:`LV_ROLLER_MODE_INFINITE` makes the roller circular.

You can select an option manually with
:cpp:expr:`lv_roller_set_selected(roller, id, LV_ANIM_ON)`,
where *id* is the index of an option.

Get selected option
-------------------

To get the *index* of the currently selected option use :cpp:expr:`lv_roller_get_selected(roller)`.

:cpp:expr:`lv_roller_get_selected_str(roller, buf, buf_size)` will copy the name of the selected option to ``buf``.

Visible rows
------------

The number of visible rows can be adjusted with :cpp:expr:`lv_roller_set_visible_row_count(roller, num)`.

This function calculates the height with the current style. If the font,
line space, border width, etc. of the roller changes this function needs
to be called again.

.. _lv_roller_events:

Events
******

-  :cpp:enumerator:`LV_EVENT_VALUE_CHANGED` Sent when a new option is selected.

See the events of the :ref:`Base object <lv_obj>` too.

Learn more about :ref:`events`.

.. _lv_roller_keys:

Keys
****

-  ``LV_KEY_RIGHT/DOWN`` Select the next option
-  ``LV_KEY_LEFT/UP`` Select the previous option
-  :cpp:enumerator:`LY_KEY_ENTER` Apply the selected option (Send :cpp:enumerator:`LV_EVENT_VALUE_CHANGED` event)

.. _lv_roller_example:

Example
*******

.. include:: ../examples/widgets/roller/index.rst

.. _lv_roller_api:

API
***
