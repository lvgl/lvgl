.. _lv_list:

==============
List (lv_list)
==============

Overview
********

The List is basically a rectangle with vertical layout to which Buttons
and Texts can be added

.. _lv_list_parts_and_styles:

Parts and Styles
****************

**Background**

- :cpp:enumerator:`LV_PART_MAIN` The main part of the list that uses all the typical background properties
- :cpp:enumerator:`LV_PART_SCROLLBAR` The scrollbar. See the :ref:`Base objects <lv_obj>` documentation for details.

**Buttons and Texts** See the :ref:`Button <lv_button>`\ 's and :ref:`Label <lv_label>`\ 's documentation.

.. _lv_list_usage:

Usage
*****

Buttons
-------

:cpp:expr:`lv_list_add_button(list, icon, text)` adds a full-width button with an icon

- that can be an image or symbol
- and a text.

The text starts to scroll horizontally if it's too long.

Texts
-----

:cpp:expr:`lv_list_add_text(list, text)` adds a text.

.. _lv_list_events:

Events
******

No special events are sent by the List, but sent by the Button as usual.

Learn more about :ref:`events`.

.. _lv_list_keys:

Keys
****

No *Keys* are processed by the object type.

Learn more about :ref:`indev_keys`.

.. _lv_list_example:

Example
*******

.. include:: ../examples/widgets/list/index.rst

.. _lv_list_api:

API
***
