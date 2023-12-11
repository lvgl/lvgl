Tabview (lv_tabview)
====================

Overview
********

The Tab view object can be used to organize content in tabs. The Tab
view is built from other widgets:

- Main container: `lv_obj </widgets/obj>`__
- Tab buttons: an `lv_obj </widgets/obj>`__` with `lv_button </widgets/button>`__ s
- Container for the tabs: `lv_obj </widgets/obj>`__
- Content of the tabs: `lv_obj </widgets/obj>`__

The tab buttons can be positioned on the top, bottom, left and right
side of the Tab view.

A new tab can be selected either by clicking on a tab button or by
sliding horizontally on the content.

Parts and Styles
****************

There are no special parts on the Tab view but the ``lv_obj`` and
``lv_button`` widgets are used to create the Tab view.

Usage
*****

Create a Tab view
-----------------

:cpp:expr:`lv_tabview_create(parent)` creates a new empty
Tab view.
Add tabs
--------

New tabs can be added with :cpp:expr:`lv_tabview_add_tab(tabview, "Tab name")`.
This will return a pointer to an `lv_obj </widgets/obj>`__ object where
the tab's content can be created.

Rename tabs
-----------

A tab can be renamed with
:cpp:expr:`lv_tabview_rename_tab(tabview, tab_id, "New Name")`.

Change tab
----------

To select a new tab you can:

- Click on its tab button
- Slide horizontally
- Use :cpp:expr:`lv_tabview_set_active(tabview, id, LV_ANIM_ON)` function


Set tab bar position
--------------------

Using the :cpp:expr:`lv_tabview_set_tab_bar_position(tabview, LV_DIR_LEFT/RIGHT/TOP/BOTTOM)`
the tab bar can be moved to any sides.

Set tab bra size
----------------

The size of the tab bar can be adjusted by :spp:expr:`lv_tabview_set_tab_bar_size(tabview, size)`
In case of vertical arrangement is means the height of the tab bar, and in horizontal
arrangement it means the width.

Get the parts
-------------

:cpp:expr:`lv_tabview_get_content(tabview)` returns the container for the tabs,
:cpp:expr:`lv_tabview_get_tab_buttons(tabview)` returns the Tab buttons object
which is a `Button matrix </widgets/buttonmatrix>`__.

Events
******

-  :cpp:enumerator:`LV_EVENT_VALUE_CHANGED` Sent when a new tab is selected by sliding
   or clicking the tab button. :cpp:expr:`lv_tabview_get_tab_active(tabview)`
   returns the zero based index of the current tab.

Learn more about :ref:`events`.

Keys
****

Keys have effect only on the tab buttons (Button matrix). Add manually
to a group if required.

Learn more about :ref:`indev_keys`.

Example
*******

.. include:: ../examples/widgets/tabview/index.rst

API
***
