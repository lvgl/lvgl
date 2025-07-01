.. _lv_tabview:

=====================
Tab View (lv_tabview)
=====================


Overview
********

The Tab View Widget can be used to organize content in tabs. The Tab
View is built from other Widgets:

- Main container: :ref:`base_widget`
- Tab buttons: an :ref:`base_widget` with :ref:`lv_button`
- Container for the tabs: :ref:`base_widget`
- Content of the tabs: :ref:`base_widget`

The tab buttons can be positioned on the top, bottom, left, and right
side of the Tab View.

A new tab can be selected either by clicking on a tab button or by
sliding horizontally on the content.



.. _lv_tabview_parts_and_styles:


Parts and Styles
****************

There are no special parts on the Tab View but the :ref:`base_widget` and
:ref:`lv_button` Widgets are used to create the Tab View.



.. _lv_tabview_usage:

Usage
*****

Creating a Tab View
-------------------

:cpp:expr:`lv_tabview_create(parent)` creates a new empty Tab View.

Adding tabs
-----------

New tabs can be added with :cpp:expr:`lv_tabview_add_tab(tabview, "Tab name")`.
This will return a pointer to a :ref:`base_widget` where
the tab's content can be created.

Renaming tabs
-------------

A tab can be renamed with
:cpp:expr:`lv_tabview_rename_tab(tabview, tab_id, "New Name")`.

Navigating to a new tab
-----------------------

To select a new tab you can:

- Click on its tab button
- Slide horizontally
- Use :cpp:expr:`lv_tabview_set_active(tabview, tab_id, LV_ANIM_ON / OFF)` function


Setting tab bar position
------------------------

Using the :cpp:expr:`lv_tabview_set_tab_bar_position(tabview, LV_DIR_LEFT/RIGHT/TOP/BOTTOM)`
the tab bar can be moved to any side.

Setting tab bar size
--------------------

The size of the tab bar can be adjusted by :cpp:expr:`lv_tabview_set_tab_bar_size(tabview, size)`
When tabs are on the top or bottom, this means the height of the tab bar, and when
they are on the sides, it means the width.

Accessing the parts
-------------------

- :cpp:expr:`lv_tabview_get_content(tabview)` returns a pointer to the container for
  tabs content (a :ref:`base_widget`)
- :cpp:expr:`lv_tabview_get_tab_bar(tabview)` returns a pointer to the container for
  tab buttons (a :ref:`base_widget`)



.. _lv_tabview_events:

Events
******

-  :cpp:enumerator:`LV_EVENT_VALUE_CHANGED` Sent when a new tab is selected by sliding
   or clicking the tab button. :cpp:expr:`lv_tabview_get_tab_active(tabview)`
   returns the zero based index of the current tab.

.. admonition::  Further Reading

    Learn more about :ref:`lv_obj_events` emitted by all Widgets.

    Learn more about :ref:`events`.



.. _lv_tabview_keys:

Keys
****

Keys have effect only on the tab buttons.
Programmatically add these buttons to a group if required.

.. admonition::  Further Reading

    Learn more about :ref:`indev_keys`.



.. _lv_tabview_example:

Example
*******

.. include:: ../../examples/widgets/tabview/index.rst



.. _lv_tabview_api:

API
***
