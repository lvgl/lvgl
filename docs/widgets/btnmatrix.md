```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/btnmatrix.md
```
# Button matrix (lv_btnmatrix)

## Overview

The Button Matrix objects can display **multiple buttons** in rows and columns.

The main reasons for wanting to use a button matrix instead of a container and individual button objects are:
- The button matrix is simpler to use for grid-based button layouts.
- The button matrix consumes a lot less memory per button.


## Parts and Styles
The Button matrix's main part is called `LV_BTNMATRIX_PART_BG`. It draws a background using the typical background style properties.

`LV_BTNMATRIX_PART_BTN` is virtual part and it refers to the buttons on the button matrix. It also uses all the typical background properties.

The top/bottom/left/right padding values from the background are used to keep some space on the sides. Inner padding is applied between the buttons.

## Usage

### Button's text
There is a text on each button. To specify them a descriptor string array, called *map*, needs to be used.
The map can be set with `lv_btnmatrix_set_map(btnm, my_map)`.
The declaration of a map should look like `const char * map[] = {"btn1", "btn2", "btn3", ""}`.
Note that **the last element has to be an empty string**!  

Use `"\n"` in the map  to make **line break**. E.g. `{"btn1", "btn2", "\n", "btn3", ""}`. Each line's buttons have their width calculated automatically.

### Control buttons
The **buttons width** can be set relative to the other button in the same line with `lv_btnmatrix_set_btn_width(btnm, btn_id, width)`
E.g. in a line with two buttons: *btnA, width = 1* and *btnB, width = 2*, *btnA* will have 33 % width and *btnB* will have 66 % width. 
It's similar to how the [`flex-grow`](https://developer.mozilla.org/en-US/docs/Web/CSS/flex-grow) property works in CSS.

In addition to width, each button can be customized with the following parameters:
- **LV_BTNMATRIX_CTRL_HIDDEN** - make a button hidden (hidden buttons still take up space in the layout, they are just not visible or clickable)
- **LV_BTNMATRIX_CTRL_NO_REPEAT** - disable repeating when the button is long pressed
- **LV_BTNMATRIX_CTRL_DISABLED** - make a button disabled
- **LV_BTNMATRIX_CTRL_CHECKABLE** - enable toggling of a button
- **LV_BTNMATRIX_CTRL_CHECK_STATE** - set the toggle state
- **LV_BTNMATRIX_CTRL_CLICK_TRIG** - if 0, the button will react on press, if 1, will react on release

The set or clear a button's control attribute, use `lv_btnmatrix_set_btn_ctrl(btnm, btn_id, LV_BTNM_CTRL_...)` and 
`lv_btnmatrix_clear_btn_ctrl(btnm, btn_id, LV_BTNM_CTRL_...)` respectively. More `LV_BTNM_CTRL_...` values can be *Or*ed

The set/clear the same control attribute for all buttons of a button matrix, use `lv_btnmatrix_set_btn_ctrl_all(btnm, btn_id, LV_BTNM_CTRL_...)` and 
`lv_btnmatrix_clear_btn_ctrl_all(btnm, btn_id, LV_BTNM_CTRL_...)`.

The set a control map for a button matrix (similarly to the map for the text), use `lv_btnmatrix_set_ctrl_map(btnm, ctrl_map)`.
An element of `ctrl_map` should look like `ctrl_map[0] = width | LV_BTNM_CTRL_NO_REPEAT |  LV_BTNM_CTRL_TGL_ENABLE`. 
The number of elements should be equal to the number of buttons (excluding newlines characters).

### One check
The "One check" feature can be enabled with `lv_btnmatrix_set_one_check(btnm, true)` to allow only one button to be checked (toggled) at once.

### Recolor
The **texts** on the button can be **recolored** similarly to the recolor feature for [Label](/widgets/label) object. To enable it, use `lv_btnmatrix_set_recolor(btnm, true)`. 
After that a button with `#FF0000 Red#` text will be red.

### Aligning the button's text
To align the text on the buttons, use `lv_btnmatrix_set_align(roller, LV_LABEL_ALIGN_LEFT/CENTER/RIGHT)`.
All text items in the button matrix will conform to the alignment proprty as it is set.

### Notes
The Button matrix object is very light weighted because the buttons are not created just virtually drawn on the fly.
This way, 1 button use only 8 extra bytes instead of the ~100-150 byte size of a normal [Button](/widgets/btn) object (plus the size of its container and a label for each button).

The disadvantage of this setup is that the ability to style individual buttons to be different from others is limited (aside from the toggling feature). 
If you require that ability, using individual buttons is very likely to be a better approach.

## Events
Besides the [Generic events](../overview/event.html#generic-events), the following [Special events](../overview/event.html#special-events) are sent by the button matrices:
 - **LV_EVENT_VALUE_CHANGED** - sent when the button is pressed/released or repeated after long press. The event data is set to the ID of the pressed/released button.

Learn more about [Events](/overview/event).

## Keys

The following *Keys* are processed by the Buttons:
- **LV_KEY_RIGHT/UP/LEFT/RIGHT** - To navigate among the buttons to select one
- **LV_KEY_ENTER** - To press/release the selected button

Learn more about [Keys](/overview/indev).

## Example

```eval_rst

.. include:: /lv_examples/src/lv_ex_widgets/lv_ex_btnmatrix/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_btnmatrix.h
  :project: lvgl

```
