==============
List (lv_list)
==============

Overview
********

The List is basically a rectangle with vertical layout to which Buttons
and Texts can be added

Parts and Styles
****************

**Background**

- :cpp:enumerator:`LV_PART_MAIN` The main part of the list that uses all the typical background properties
- :cpp:enumerator:`LV_PART_SCROLLBAR` The scrollbar. See the :ref:`Base objects <widgets/obj/base object (lv_obj) [0-1]>` documentation for details.

**Buttons and Texts** See the :ref:`Button <widgets/button/button (lv_button) [0-1]>`\ 's and :ref:`Label <widgets/label/label (lv_label) [0-1]>`\ 's documentation.

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

Events
******

No special events are sent by the List, but sent by the Button as usual.

Learn more about :ref:`overview/events/events [0-1]`.

Keys
****

No *Keys* are processed by the object type.

Learn more about :ref:`overview/indev/keys [0-3]`.

Example
*******

.. include:: ../examples/widgets/list/index.rst

API
***
