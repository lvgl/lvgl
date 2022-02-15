```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/extra/msgbox.md
```
# Message box (lv_msgbox)

## Overview
The Message boxes act as pop-ups. 
They are built from a background container, a title, an optional close button, a text and optional buttons.

The text will be broken into multiple lines automatically and the height will be set automatically to include the text and the buttons.

The message box can be modal (blocking clicks on the rest of the screen) or not modal.

## Parts and Styles
The message box is built from other widgets, so you can check these widgets' documentation for details.
- Background: [lv_obj](/widgets/obj)
- Close button: [lv_btn](/widgets/core/btn)
- Title and text: [lv_label](/widgets/core/label)
- Buttons: [lv_btnmatrix](/widgets/core/btnmatrix)

## Usage

### Create a message box

`lv_msgbox_create(parent, title, txt, btn_txts[], add_close_btn)` creates a message box.

If `parent` is `NULL` the message box will be modal. `title` and `txt` are strings for the title and the text. 
`btn_txts[]` is an array with the buttons' text. E.g. `const char * btn_txts[] = {"Ok", "Cancel", NULL}`.
`add_close_btn` can be `true` or `false` to add/don't add a close button.

### Get the parts
The building blocks of the message box can be obtained using the following functions:
```c 
lv_obj_t * lv_msgbox_get_title(lv_obj_t * mbox);
lv_obj_t * lv_msgbox_get_close_btn(lv_obj_t * mbox);
lv_obj_t * lv_msgbox_get_text(lv_obj_t * mbox);
lv_obj_t * lv_msgbox_get_btns(lv_obj_t * mbox);
```

### Close the message box
`lv_msgbox_close(msgbox)` closes (deletes) the message box.

## Events
- `LV_EVENT_VALUE_CHANGED` is sent by the buttons if one of them is clicked. `LV_OBJ_FLAG_EVENT_BUBBLE` is enabled on the buttons so you can add events to the message box itself. 
In the event handler, `lv_event_get_target(e)` will return the button matrix and `lv_event_get_current_target(e)` will return the message box. `lv_msgbox_get_active_btn(msgbox)` and `lv_msgbox_get_active_btn_text(msgbox)` can be used to get the index and text of the clicked button.

Learn more about [Events](/overview/event).

## Keys
Keys have effect on the close button and button matrix. You can add them manually to a group if required.

Learn more about [Keys](/overview/indev).


## Example

```eval_rst

.. include:: ../../../examples/widgets/msgbox/index.rst

```

## API 

```eval_rst

.. doxygenfile:: lv_msgbox.h
  :project: lvgl
        
```
