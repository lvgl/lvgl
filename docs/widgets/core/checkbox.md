# Checkbox (lv_checkbox)


## Overview

The Checkbox object is created from a "tick box" and a label. When the Checkbox is clicked the tick box is toggled.

## Parts and Styles
- `LV_PART_MAIN` The is the background of the Checkbox and it uses the text and all the typical background style properties.
`pad_column` adjusts the spacing between the tickbox and the label
- `LV_PART_INDICATOR` The "tick box" is a square that uses all the typical background style properties.
By default, its size is equal to the height of the main part's font. Padding properties make the tick box larger in the respective directions.

The Checkbox is added to the default group (if it is set).

## Usage


### Text
The text can be modified with the `lv_checkbox_set_text(cb, "New text")` function and will be dynamically allocated.

To set a static text,
use `lv_checkbox_set_static_text(cb, txt)`. This way, only a pointer to `txt` will be stored. The text then shouldn't be deallocated while the checkbox exists.

### Check, uncheck, disable
You can manually check, un-check, and disable the Checkbox by using the common state add/clear function:
```c
lv_obj_add_state(cb, LV_STATE_CHECKED);   /*Make the chekbox checked*/
lv_obj_clear_state(cb, LV_STATE_CHECKED); /*MAke the checkbox unchecked*/
lv_obj_add_state(cb, LV_STATE_CHECKED | LV_STATE_DISABLED); /*Make the checkbox checked and disabled*/
```

## Events
- `LV_EVENT_VALUE_CHANGED` Sent when the checkbox is toggled.
- `LV_EVENT_DRAW_PART_BEGIN` and `LV_EVENT_DRAW_PART_END` are sent for the following types:
    - `LV_CHECKBOX_DRAW_PART_BOX` The tickbox of the checkbox
        - `part`: `LV_PART_INDICATOR`
        - `draw_area`: the area of the tickbox
        - `rect_dsc`

See the events of the [Base object](/widgets/obj) too.

Learn more about [Events](/overview/event).


## Keys
The following *Keys* are processed by the 'Buttons':
- `LV_KEY_RIGHT/UP` Go to toggled state if toggling is enabled
- `LV_KEY_LEFT/DOWN` Go to non-toggled state if toggling is  enabled
- `LV_KEY_ENTER` Clicks the checkbox and toggles it

Note that, as usual, the state of `LV_KEY_ENTER` is translated to `LV_EVENT_PRESSED/PRESSING/RELEASED` etc.

Learn more about [Keys](/overview/indev).


## Example

```eval_rst

.. include:: ../../../examples/widgets/checkbox/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_checkbox.h
  :project: lvgl

```
