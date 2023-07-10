
# Tabview (lv_tabview)

## Overview

The Tab view object can be used to organize content in tabs.
The Tab view is built from other widgets:
- Main container: [lv_obj](/widgets/obj))
  - Tab buttons: [lv_btnmatrix](/widgets/core/btnmatrix)
  - Container for the tabs: [lv_obj](/widgets/obj)
     - Content of the tabs: [lv_obj](/widgets/obj)

The tab buttons can be positioned on the top, bottom, left and right side of the Tab view.

A new tab can be selected either by clicking on a tab button or by sliding horizontally on the content.

## Parts and Styles
There are no special parts on the Tab view but the `lv_obj` and `lv_btnnmatrix` widgets are used to create the Tab view.

## Usage

### Create a Tab view

`lv_tabview_create(parent, tab_pos, tab_size);` creates a new empty Tab view.  `tab_pos` can be `LV_DIR_TOP/BOTTOM/LEFT/RIGHT` to position the tab buttons to a side.
`tab_size` is the height (in case of `LV_DIR_TOP/BOTTOM`) or width (in case of `LV_DIR_LEFT/RIGHT`) tab buttons.

### Add tabs

New tabs can be added with `lv_tabview_add_tab(tabview, "Tab name")`. This will return a pointer to an [lv_obj](/widgets/obj) object where the tab's content can be created.

### Rename tabs

A tab can be renamed with `lv_tabview_rename_tab(tabview, tab_id, "New Name")`.

### Change tab

To select a new tab you can:
- Click on its tab button
- Slide horizontally
- Use `lv_tabview_set_act(tabview, id, LV_ANIM_ON/OFF)` function

### Get the parts

`lv_tabview_get_content(tabview)` returns the container for the tabs, `lv_tabview_get_tab_btns(tabview)` returns the Tab buttons object which is a [Button matrix](/widgets/core/btnmatrix).

## Events
- `LV_EVENT_VALUE_CHANGED` Sent when a new tab is selected by sliding or clicking the tab button. `lv_tabview_get_tab_act(tabview)` returns the zero based index of the current tab.

Learn more about [Events](/overview/event).

## Keys

Keys have effect only on the tab buttons (Button matrix). Add manually to a group if required.

Learn more about [Keys](/overview/indev).

## Example

```eval_rst

.. include:: ../../../examples/widgets/tabview/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_tabview.h
  :project: lvgl

```
