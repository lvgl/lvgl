```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/page.md
```
# Page (lv_page)

## Overview

The Page consist of two [Containers](/widgets/cont) on each other: 
- a **background**
- a top which is **scrollable**. 

## Parts and Styles

The Page's main part is called `LV_PAGE_PART_BG` which is the background of the Page. It uses all the typical background style properties. Using padding will add space on the sides.

The scrollable object can be referenced via the `LV_PAGE_PART_SCRL` part. It also uses all the typical background style properties and padding to add space on the sides.


`LV_LIST_PART_SCROLLBAR` is a virtual part of the background to draw the scroll bars. Uses all the typical background style properties, *size* to set the width of the scroll bars, and *pad_right* and *pad_bottom* to set the spacing.

`LV_LIST_PART_EDGE_FLASH`is also a virtual part of the background to draw a semicircle on the sides when the list can not be scrolled in that direction further. Uses all the typical background properties.

## Usage

The background object can be referenced as the page itself like. E.g. to set the page's width: `lv_obj_set_width(page, 100)`.

If a child is created on the page it will be automatically moved to the scrollable container. 
If the scrollable container becomes larger then the background it can be scrolled by dragging (like the lists on smartphones).

By default, the scrollable's has `LV_FIT_MAX` fit in all directions. 
It means the scrollable size will be the same as the background's size (minus the padding) while the children are in the background. 
But when an object is positioned out of the background the scrollable size will be increased to involve it.
  
### Scrollbars
 
Scrollbars can be shown according to four policies:
- `LV_SCRLBAR_MODE_OFF` Never show scroll bars
- `LV_SCRLBAR_MODE_ON` Always show scroll bars
- `LV_SCRLBAR_MODE_DRAG` Show scroll bars when the page is being dragged
- `LV_SCRLBAR_MODE_AUTO` Show scroll bars when the scrollable container is large enough to be scrolled
- `LV_SCRLBAR_MODE_HIDE` Hide the scroll bar temporally
- `LV_SCRLBAR_MODE_UNHIDE`  Unhide the previously hidden scrollbar. Recover the original mode too

The scroll bar show policy can be changed by: `lv_page_set_scrlbar_mode(page, SB_MODE)`. The default value is `LV_SCRLBAR_MODE_AUTO`.


### Glue object
A children can be "glued" to the page. In this case, if the page can be scrolled by dragging that object. 
It can be enabled by the `lv_page_glue_obj(child, true)`.

### Focus object
An object on a page can be focused with `lv_page_focus(page, child, LV_ANIM_ONO/FF)`.
It will move the scrollable container to show a child. The time of the animation can be set by `lv_page_set_anim_time(page, anim_time)` in milliseconds.
`child` doesn't have to be a direct child of the page. This is it works if the scrollable object is the grandparent of the object too.

### Manual navigation
You can move the scrollable object manually using `lv_page_scroll_hor(page, dist)` and `lv_page_scroll_ver(page, dist)`

### Edge flash
A circle-like effect can be shown if the list reached the most top/bottom/left/right position. `lv_page_set_edge_flash(list, en)` enables this feature.

### Scroll propagation

If the list is created on an other scrollable element (like an other page) and the Page can't be scrolled further the scrolling can be propagated to the parent to continue the scrolling on the parent. 
It can be enabled with `lv_page_set_scroll_propagation(list, true)`

## Clean the page
All the object created on the page can be clean with `lv_page_clean(page)`. Note that `lv_obj_clean(page)` doesn't work here because it would delete the scrollable object too.

### Scrollable API
There are functions to directly set/get the scrollable's attributes: 
- `lv_page_get_scrl()`
- `lv_page_set_scrl_fit/fint2/fit4()`
- `lv_page_set_scrl_width()`
- `lv_page_set_scrl_height()`
- `lv_page_set_scrl_fit_width()`
- `lv_page_set_scrl_fit_height()`
- `lv_page_set_scrl_layout()`

## Events
Only the [Generic events](../overview/event.html#generic-events) are sent by the object type.

The scrollable object has a default event callback which propagates the following events to the background object:
`LV_EVENT_PRESSED`, `LV_EVENT_PRESSING`, `LV_EVENT_PRESS_LOST`,`LV_EVENT_RELEASED`, `LV_EVENT_SHORT_CLICKED`, `LV_EVENT_CLICKED`, `LV_EVENT_LONG_PRESSED`, `LV_EVENT_LONG_PRESSED_REPEAT` 

Learn more about [Events](/overview/event).

##Keys

The following *Keys* are processed by the Page:
- **LV_KEY_RIGHT/LEFT/UP/DOWN** Scroll the page

Learn more about [Keys](/overview/indev).

## Example

```eval_rst

.. include:: /lv_examples/src/lv_ex_widgets/lv_ex_page/index.rst

```

## API 

```eval_rst

.. doxygenfile:: lv_page.h
  :project: lvgl
        
```
