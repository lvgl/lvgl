.. _lv_menu:

==============
Menu (lv_menu)
==============

Overview
********

The menu widget can be used to easily create multi-level menus. It
handles the traversal between pages automatically.

.. _lv_menu_parts_and_styles:

Parts and Styles
****************

The menu widget is built from the following objects: 

- Main container: :cpp:type:`lv_menu_main_cont`
- Main header: :cpp:type:`lv_menu_main_header_cont`
- Back button: :ref:`lv_button`
- Back button icon: :ref:`lv_image`
- Main page: :cpp:type:`lv_menu_page`
- Sidebar container: :cpp:type:`lv_menu_sidebar_cont`
- Sidebar header: :cpp:type:`lv_menu_sidebar_header_cont`
- Back button: :ref:`lv_button`
- Back button icon: :ref:`lv_image`
- Sidebar page: :cpp:type:`lv_menu_page`

.. _lv_menu_usage:

Usage
*****

Create a menu
-------------

:cpp:expr:`lv_menu_create(parent)` creates a new empty menu.

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

Create a menu page
------------------

:cpp:expr:`lv_menu_page_create(menu, title)` creates a new empty menu page. You
can add any widgets to the page.

Set a menu page in the main area
--------------------------------

Once a menu page has been created, you can set it to the main area with
:cpp:expr:`lv_menu_set_page(menu, page)`. ``NULL`` to clear main and clear menu
history.

Set a menu page in the sidebar
------------------------------

Once a menu page has been created, you can set it to the sidebar with
:cpp:expr:`lv_menu_set_sidebar_page(menu, page)`. ``NULL`` to clear sidebar.

Linking between menu pages
--------------------------

For instance, you have created a button obj in the main page. When you
click the button obj, you want it to open up a new page, use
:cpp:expr:`lv_menu_set_load_page_event(menu, obj, new page)`.

Create a menu container, section, separator
-------------------------------------------

The following objects can be created so that it is easier to style the
menu:

- :cpp:expr:`lv_menu_cont_create(parent page)` creates a new empty container.
- :cpp:expr:`lv_menu_section_create(parent page)` creates a new empty section.
- :cpp:expr:`lv_menu_separator_create(parent page)` creates a separator.

.. _lv_menu_events:

Events
******

-  :cpp:enumerator:`LV_EVENT_VALUE_CHANGED` Sent when a page is shown.

   -  :cpp:expr:`lv_menu_get_cur_main_page(menu)` returns a pointer to menu page
      that is currently displayed in main.
   -  :cpp:expr:`lv_menu_get_cur_sidebar_page(menu)` returns a pointer to menu
      page that is currently displayed in sidebar.

-  :cpp:enumerator:`LV_EVENT_CLICKED` Sent when a back button in a header from either
   main or sidebar is clicked. :cpp:enumerator:`LV_OBJ_FLAG_EVENT_BUBBLE` is enabled
   on the buttons so you can add events to the menu itself.

   -  :cpp:expr:`lv_menu_back_button_is_root(menu, button)` to check if button is root
      back button

See the events of the :ref:`Base object <lv_obj>` too.

Learn more about :ref:`events`.

.. _lv_menu_keys:

Keys
****

No keys are handled by the menu widget.

Learn more about :ref:`indev_keys`.

.. _lv_menu_example:

Example
*******

.. include:: ../examples/widgets/menu/index.rst

.. _lv_menu_api:

API
***
