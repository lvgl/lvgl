.. _lv_menu:

==============
Menu (lv_menu)
==============


Overview
********

The Menu Widget can be used to easily create multi-level menus. It
handles the traversal between Pages automatically.


.. _lv_menu_parts_and_styles:

Parts and Styles
****************

The Menu Widget is built from the following Widgets:

- Main container: :cpp:type:`lv_menu_main_cont`
- Main header: :cpp:type:`lv_menu_main_header_cont`
- Back button: :ref:`lv_button`
- Back button icon: :ref:`lv_image`
- Main Page: :cpp:type:`lv_menu_page`
- Sidebar container: :cpp:type:`lv_menu_sidebar_cont`
- Sidebar header: :cpp:type:`lv_menu_sidebar_header_cont`
- Back button: :ref:`lv_button`
- Back button icon: :ref:`lv_image`
- Sidebar Page: :cpp:type:`lv_menu_page`


.. _lv_menu_usage:

Usage
*****

Create a Menu
-------------

:cpp:expr:`lv_menu_create(parent)` creates a new empty Menu.

Header mode
-----------

The following header modes exist:

- :cpp:enumerator:`LV_MENU_HEADER_TOP_FIXED` Header is positioned at the top.
- :cpp:enumerator:`LV_MENU_HEADER_TOP_UNFIXED` Header is positioned at the top and can be scrolled out of view.
- :cpp:enumerator:`LV_MENU_HEADER_BOTTOM_FIXED` Header is positioned at the bottom.

You can set header modes with :cpp:expr:`lv_menu_set_mode_header(menu, LV_MENU_HEADER...)`.

Root back button mode
---------------------

The following root back button modes exist:

- :cpp:enumerator:`LV_MENU_ROOT_BACK_BTN_DISABLED`
- :cpp:enumerator:`LV_MENU_ROOT_BACK_BTN_ENABLED`

You can set root back button modes with
:cpp:expr:`lv_menu_set_mode_root_back_button(menu, LV_MENU_ROOT_BACK_BTN...)`.

Create a Menu Page
------------------

:cpp:expr:`lv_menu_page_create(menu, title)` creates a new empty Menu Page. You
can add any Widgets to the Page.

Set a Menu Page in the main area
--------------------------------

Once a Menu Page has been created, you can set it to the main area with
:cpp:expr:`lv_menu_set_page(menu, page)`. ``NULL`` to clear main and clear Menu
history.

Set a Menu Page in the sidebar
------------------------------

Once a Menu Page has been created, you can set it to the sidebar with
:cpp:expr:`lv_menu_set_sidebar_page(menu, page)`. ``NULL`` to clear sidebar.

Linking between Menu Pages
--------------------------

If you have, for instance, created a button obj in the main Page. When you
click the button Widget, you want it to open up a new Page, use
:cpp:expr:`lv_menu_set_load_page_event(menu, btn, new page)`.

Create a Menu container, section, separator
-------------------------------------------

The following objects can be created so that it is easier to style the
Menu:

- :cpp:expr:`lv_menu_cont_create(parent_page)` creates a new empty container.
- :cpp:expr:`lv_menu_section_create(parent_page)` creates a new empty section.
- :cpp:expr:`lv_menu_separator_create(parent_page)` creates a separator.



.. _lv_menu_events:

Events
******

-  :cpp:enumerator:`LV_EVENT_VALUE_CHANGED` Sent when a Page is shown.

   -  :cpp:expr:`lv_menu_get_cur_main_page(menu)` returns a pointer to Menu Page
      that is currently displayed in the main container.
   -  :cpp:expr:`lv_menu_get_cur_sidebar_page(menu)` returns a pointer to Menu
      Page that is currently displayed in the sidebar container.

-  :cpp:enumerator:`LV_EVENT_CLICKED` Sent when a back button in a header from either
   main or sidebar is clicked. :cpp:enumerator:`LV_OBJ_FLAG_EVENT_BUBBLE` is enabled
   on the buttons so you can add events to the Menu itself.

   -  :cpp:expr:`lv_menu_back_button_is_root(menu, button)` to check if button is root
      back button.

.. admonition::  Further Reading

    Learn more about :ref:`lv_obj_events` emitted by all Widgets.

    Learn more about :ref:`events`.



.. _lv_menu_keys:

Keys
****

No *Keys* are processed by Menu Widgets.

.. admonition::  Further Reading

    Learn more about :ref:`indev_keys`.



.. _lv_menu_example:

Example
*******

.. include:: ../../examples/widgets/menu/index.rst



.. _lv_menu_api:

API
***
