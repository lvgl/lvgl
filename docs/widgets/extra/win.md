# Window (lv_win)

## Overview

The Window is container-like object built from a header with title and buttons and a content area.

## Parts and Styles
The Window is built from other widgets so you can check their documentation for details:
- Background: [lv_obj](/widgets/obj)
- Header on the background: [lv_obj](/widgets/obj)
- Title on the header: [lv_label](/widgets/core/label)
- Buttons on the header: [lv_btn](/widgets/core/btn)
- Content area on the background: [lv_obj](/widgets/obj)


## Usage

### Create a Window

`lv_win_create(parent, header_height)` creates a Window with an empty header.

### Title and buttons

Any number of texts (but typically only one) can be added to the header with `lv_win_add_title(win, "The title")`.

Control buttons can be added to the window's header with `lv_win_add_btn(win, icon, btn_width)`. `icon` can be any image source, and `btn_width` is the width of the button.

The title and the buttons will be added in the order the functions are called. So adding a button, a text and two other buttons will result in a button on the left, a title, and 2 buttons on the right.
The width of the title is set to take all the remaining space on the header. In other words: it pushes to the right all the buttons that are added after the title.

## Get the parts
`lv_win_get_header(win)` returns a pointer to the header, `lv_win_get_content(win)` returns a pointer to the content container to which the content of the window can be added.

## Events
No special events are sent by the windows, however events can be added manually to the return value of `lv_win_add_btn`.

Learn more about [Events](/overview/event).

## Keys
No *Keys* are handled by the window.

Learn more about [Keys](/overview/indev).


## Example

```eval_rst

.. include:: ../../../examples/widgets/win/index.rst

```


## API

```eval_rst

.. doxygenfile:: lv_win.h
  :project: lvgl

```
