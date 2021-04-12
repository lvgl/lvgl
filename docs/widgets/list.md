```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/list.md
```
# List (lv_list)

## Overview
The Lists are built from a background [Page](/widgets/page) and [Buttons](/widgets/btn) on it. 
The Buttons contain an optional icon-like [Image](/widgets/img) (which can be a symbol too) and a [Label](/widgets/label). 
When the list becomes long enough it can be scrolled. 

## Parts and Styles
The List has the same parts as the [Page](/widgets/page)
- `LV_LIST_PART_BG`
- `LV_LIST_PART_SCRL`
- `LV_LIST_PART_SCRLBAR`
- `LV_LIST_PART_EDGE_FLASH`

Refer to the [Page](/widgets/page) documentation for details.


The buttons on the list are treated as normal buttons and they only have a main part called `LV_BTN_PART_MAIN`.

## Usage

### Add buttons
You can add new list elements (button) with `lv_list_add_btn(list, &icon_img, "Text")` or with symbol `lv_list_add_btn(list, SYMBOL_EDIT, "Edit text")`. 
If you do not want to add image use `NULL` as image source. The function returns with a pointer to the created button to allow further configurations.

The width of the buttons is set to maximum according to the object width. 
The height of the buttons are adjusted automatically according to the content. (*content height* + *padding_top* + *padding_bottom*).

The labels are created with `LV_LABEL_LONG_SROLL_CIRC` long mode to automatically scroll the long labels circularly.

`lv_list_get_btn_label(list_btn)` and `lv_list_get_btn_img(list_btn)` can be used to get the label and the image of a list button. 
The text can be et directly with `lv_list_get_btn_text(list_btn)`.

### Delete buttons
To delete a list element use `lv_list_remove(list, btn_index)`. btn_index can be obtained by `lv_list_get_btn_index(list, btn)` where btn is the return value of `lv_list_add_btn()`.

To clean the list (remove all buttons) use `lv_list_clean(list)`

### Manual navigation
You can navigate manually in the list with `lv_list_up(list)` and `lv_list_down(list)`.

You can focus on a button directly using `lv_list_focus(btn, LV_ANIM_ON/OFF)`.

The **animation time** of up/down/focus movements can be set via: `lv_list_set_anim_time(list, anim_time)`. Zero animation time means not animations. 

### Layout
By default the list is vertical. To get a horizontal list use `lv_list_set_layout(list, LV_LAYOUT_ROW_MID)`.
 
### Edge flash
A circle-like effect can be shown when the list reaches the most top or bottom position. 
`lv_list_set_edge_flash(list, true)` enables this feature.

### Scroll propagation
If the list is created on an other scrollable element (like a [Page](/widgets/page)) and the list can't be scrolled further the scrolling can be propagated to the parent. 
This way the scroll will be continued on the parent. It can be enabled with `lv_list_set_scroll_propagation(list, true)`

## Events
Only the [Generic events](../overview/event.html#generic-events) are sent by the object type.

Learn more about [Events](/overview/event).

## Keys
The following *Keys* are processed by the Lists:
- **LV_KEY_RIGHT/DOWN** Select the next button
- **LV_KEY_LEFT/UP** Select the previous button

Note that, as usual, the state of `LV_KEY_ENTER` is translated to `LV_EVENT_PRESSED/PRESSING/RELEASED` etc.

The Selected buttons are in `LV_BTN_STATE_PR/TG_PR` state.

To manually select a button use `lv_list_set_btn_selected(list, btn)`. When the list is defocused and focused again it will restore the last selected button.

Learn more about [Keys](/overview/indev).


## Example

```eval_rst

.. include:: /lv_examples/src/lv_ex_widgets/lv_ex_list/index.rst

```

## API 

```eval_rst

.. doxygenfile:: lv_list.h
  :project: lvgl
        
```
