```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/extra/menu.md
```
# Menu (lv_menu)

## Overview
The menu widget can be used to easily create multi-level menus. It handles the traversal between pages automatically.

## Parts and Styles
The menu widget is built from the following objects:
- Main container: lv_menu_main_cont
  - Main header: lv_menu_main_header_cont
    - Back btn: [lv_btn](/widgets/core/btn)
      - Back btn icon: [lv_img](/widgets/core/img)
  - Main page: lv_menu_page
- Sidebar container: lv_menu_sidebar_cont
  - Sidebar header: lv_menu_sidebar_header_cont
    - Back btn: [lv_btn](/widgets/core/btn)
      - Back btn icon: [lv_img](/widgets/core/img)
  - Sidebar page: lv_menu_page

## Usage

### Create a menu
`lv_menu_create(parent)` creates a new empty menu.

### Header mode
The following header modes exist:
- `LV_MENU_HEADER_TOP_FIXED` Header is positioned at the top.
- `LV_MENU_HEADER_TOP_UNFIXED` Header is positioned at the top and can be scrolled out of view.
- `LV_MENU_HEADER_BOTTOM_FIXED` Header is positioned at the bottom.

You can set header modes with `lv_menu_set_mode_header(menu, LV_MENU_HEADER...)`. 

### Root back button mode
The following root back button modes exist:
- `LV_MENU_ROOT_BACK_BTN_DISABLED`
- `LV_MENU_ROOT_BACK_BTN_ENABLED`

You can set root back button modes with `lv_menu_set_mode_root_back_btn(menu, LV_MENU_ROOT_BACK_BTN...)`.

### Create a menu page
`lv_menu_page_create(menu, title)` creates a new empty menu page.
You can add any widgets to the page.

### Set a menu page in the main area
Once a menu page has been created, you can set it to the main area with `lv_menu_set_page(menu, page)`. NULL to clear main and clear menu history.

### Set a menu page in the sidebar
Once a menu page has been created, you can set it to the sidebar with `lv_menu_set_sidebar_page(menu, page)`. NULL to clear sidebar.

### Linking between menu pages
For instance, you have created a btn obj in the main page. When you click the btn obj, you want it to open up a new page, use `lv_menu_set_load_page_event(menu, obj, new page)`.

### Create a menu container, section, separator
The following objects can be created so that it is easier to style the menu:

`lv_menu_cont_create(parent page)` creates a new empty container.

`lv_menu_section_create(parent page)` creates a new empty section.

`lv_menu_separator_create(parent page)` creates a separator.

## Events
- `LV_EVENT_VALUE_CHANGED` Sent when a page is shown.
  - `lv_menu_get_cur_main_page(menu)` returns a pointer to menu page that is currently displayed in main.
  - `lv_menu_get_cur_sidebar_page(menu)` returns a pointer to menu page that is currently displayed in sidebar.
- `LV_EVENT_CLICKED` Sent when a back btn in a header from either main or sidebar is clicked. `LV_OBJ_FLAG_EVENT_BUBBLE` is enabled on the buttons so you can add events to the menu itself. 
  - `lv_menu_back_btn_is_root(menu, btn)` to check if btn is root back btn

See the events of the [Base object](/widgets/obj) too.

Learn more about [Events](/overview/event).

## Keys
No keys are handled by the menu widget.

Learn more about [Keys](/overview/indev).


## Example

```eval_rst
.. include:: ../../../examples/widgets/menu/index.rst
```

## API 

```eval_rst
.. doxygenfile:: lv_menu.h
  :project: lvgl
        
```