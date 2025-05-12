.. _lv_list:

==============
List (lv_list)
==============


Overview
********

The List Widget is basically a rectangle with vertical layout to which Buttons
and Text can be added.


.. _lv_list_parts_and_styles:

Parts and Styles
****************

**Background**

- :cpp:enumerator:`LV_PART_MAIN` The main part of the List that uses the
  :ref:`typical background style properties <typical bg props>`.
- :cpp:enumerator:`LV_PART_SCROLLBAR` The scrollbar. See :ref:`base_widget`
  documentation for details.

**Buttons and Text**

- See the :ref:`Button <lv_button>`'s and :ref:`Label <lv_label>`'s documentation.


.. _lv_list_usage:

Usage
*****

Buttons
-------

:cpp:expr:`lv_list_add_button(list, icon, text)` adds a full-width button with an icon
(that can be an image or symbol) and text.  This function returns a pointer to the
button created, which you can use to, for example, add an event call-back.

The text is scrolled horizontally if it is longer than the button.

Text
----

:cpp:expr:`lv_list_add_text(list, text)` adds a text string.  This function returns a
pointer to the label created, which you can use to, for example, change its text
with one of the ``lv_label_set_text...()`` functions.



.. _lv_list_events:

Events
******

No special events are sent by List Widgets, but events can be sent by Buttons as usual.

.. admonition::  Further Reading

    Learn more about :ref:`lv_obj_events` emitted by all Widgets.

    Learn more about :ref:`events`.



.. _lv_list_keys:

Keys
****

No *Keys* are processed by List Widgets.

.. admonition::  Further Reading

    Learn more about :ref:`indev_keys`.



.. _lv_list_example:

Example
*******

.. include:: ../../examples/widgets/list/index.rst



.. _lv_list_api:

API
***
