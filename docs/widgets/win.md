```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/win.md
```
# Window (lv_win)

## Overview

The Window is container-like objects built from a header with title and button and a content area. 

 
## Parts and Styles
The main part is `LV_WIN_PART_BG` which holds the two other real parts:
1. `LV_WIN_PART_HEADER`: a header [Container](/widgets/cont) on the top with a title and control buttons
2. `LV_WIN_PART_CONTENT_SCRL` the scrollable part of a [Page](/widgets/page) for the content below the header. 


Besides these, `LV_WIN_PART_CONTENT_SCRL` has a scrollbar part called `LV_WIN_PART_CONTENT_SCRL`.
Read the documentation of [Page](/widgets/page) for more details on the scrollbars.

All parts supports the typical background properties. The title uses the *Text* properties of the header part.
 
The height of the control buttons is: *header height - header padding_top -  header padding_bottom*.
 

### Title
On the header, there is a title which can be modified by: `lv_win_set_title(win, "New title")`. 

### Control buttons
Control buttons can be added to the right of the window header with: `lv_win_add_btn_right(win, LV_SYMBOL_CLOSE)`, to add a button to the left side of the window header use `lv_win_add_btn_left(win, LV_SYMBOL_CLOSE)` instead.
The second parameter is an [Image](/widgets/img) source so it can be a symbol, a pointer to an `lv_img_dsc_t `variable or a path to file.

The width of the buttons can be set with `lv_win_set_btn_width(win, w)`. If `w == 0` the buttons will be square-shaped.

`lv_win_close_event_cb` can be used as an event callback to close the Window.

### Scrollbars

The scrollbar behavior can be set by `lv_win_set_scrlbar_mode(win, LV_SCRLBAR_MODE_...)`. 
See [Page](/widgets/page) for details.

### Manual scroll and focus
To scroll the Window directly you can use `lv_win_scroll_hor(win, dist_px)` or `lv_win_scroll_ver(win, dist_px)`.

To make the Window show an object on it use `lv_win_focus(win, child, LV_ANIM_ON/OFF)`.

The time of scroll and focus animations can  be adjusted with `lv_win_set_anim_time(win, anim_time_ms)`

### Layout
To set a layout for the content use `lv_win_set_layout(win, LV_LAYOUT_...)`. 
See [Container](/widgets/cont) for details.

## Events
Only the [Generic events](../overview/event.html#generic-events) are sent by the object type.

Learn more about [Events](/overview/event).

## Keys

The following *Keys* are processed by the Page:
- **LV_KEY_RIGHT/LEFT/UP/DOWN** Scroll the page

Learn more about [Keys](/overview/indev).


## Example

```eval_rst

.. include:: /lv_examples/src/lv_ex_widgets/lv_ex_win/index.rst

```


## API 

```eval_rst

.. doxygenfile:: lv_win.h
  :project: lvgl
        
```
