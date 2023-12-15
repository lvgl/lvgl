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
- :cpp:enumerator:`LV_PART_SCROLLBAR` The scrollbar. See the `Base objects </widgets/obj>`__ documentation for details.

**Buttons and Texts** See the `Button </widgets/button>`__\ 's and `Label </widgets/label>`__\ 's documentation.

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

Learn more about :ref:`events`.

Keys
****

No *Keys* are processed by the object type.

Learn more about :ref:`indev_keys`.

Example
*******

.. include:: ../examples/widgets/list/index.rst

API
***
