```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/msgbox.md
```
# Message box (lv_msgbox)

## Overview
The Message boxes act as pop-ups. 
They are built from a background [Container](/widgets/cont), a [Label](/widgets/label) and a [Button matrix](/widgets/btnmatrix) for buttons. 

The text will be broken into multiple lines automatically (has `LV_LABEL_LONG_MODE_BREAK`) and the height will be set automatically to involve the text and the buttons (`LV_FIT_TIGHT` fit vertically)-

## Parts and Styles
The Message box's main part is called `LV_MSGBOX_PART_MAIN` and it uses all the typical background style properties. Using padding will add space on the sides. *pad_inner* will add space between the text and the buttons. 
The *label* style properties affect the style of text.

The buttons parts are the same as in case of [Button matrix](/widgets/btnmatrix):
- `LV_MSGBOX_PART_BTN_BG` the background of the buttons
- `LV_MSGBOX_PART_BTN` the buttons


## Usage


### Set text
To set the text use the `lv_msgbox_set_text(msgbox, "My text")` function. Not only the pointer of the text will be saved, so the the text can be in a local variable too.

### Add buttons
 To add buttons use the `lv_msgbox_add_btns(msgbox, btn_str)` function. The button's text needs to be specified like `const char * btn_str[] = {"Apply", "Close", ""}`. 
 For more information visit the [Button matrix](/widgets/btnmatrix) documentation.

 The button matrix will be created only when `lv_msgbox_add_btns()` is called for the first time.

### Auto-close
With `lv_msgbox_start_auto_close(mbox, delay)` the message box can be closed automatically after `delay` milliseconds with an animation. The `lv_mbox_stop_auto_close(mbox)` function stops a started auto close.

The duration of the close animation can be set by `lv_mbox_set_anim_time(mbox, anim_time)`.

## Events
Besides the [Generic events](../overview/event.html#generic-events) the following [Special events](../overview/event.html#special-events) are sent by the Message boxes:
 - **LV_EVENT_VALUE_CHANGED** sent when the button is clicked. The event data is set to ID of the clicked button.

The Message box has a default event callback which closes itself when a button is clicked.

Learn more about [Events](/overview/event).

##Keys

The following *Keys* are processed by the Buttons:
- **LV_KEY_RIGHT/DOWN** Select the next button
- **LV_KEY_LEFT/TOP** Select the previous button
- **LV_KEY_ENTER** Clicks the selected button 

Learn more about [Keys](/overview/indev).


## Example

```eval_rst

.. include:: /lv_examples/src/lv_ex_widgets/lv_ex_msgbox/index.rst

```

## API 

```eval_rst

.. doxygenfile:: lv_msgbox.h
  :project: lvgl
        
```
