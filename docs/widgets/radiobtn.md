# Radio button (lv_radiobtn)

## Overview
The Radio button is basically a rectangle with vertical layout to which buttons(modified from [Checkbox](/widgets/core/checkbox)) can be added, and at same time only one button can be selected.

## Parts and Styles

**Background**
- `LV_PART_MAIN` The main part of the radio button that uses all the typical background properties

**Buttons**
- `LV_PART_MAIN` The main part of the single button that uses all the typical background properties
- `LV_PART_INDICATOR` The "tick box" is a circle that uses all the typical background style properties.
By default, its size is equal to the height of the main part's font. Padding properties make the tick box larger in the respective directions.
- `LV_PART_CUSTOM_FIRST` the inner box inside "tick box" that uses all the typical background style properties, use to show the button being selected.

## Usage

### Buttons
`lv_radiobtn_add_item(radiobtn, text)` adds an button with text.

The text of button can be modified with the `lv_radiobtn_set_item_text(rb_item, "New text")` function and will be dynamically allocated.

To set a static text,
use `lv_radiobtn_set_item_static_text(rb_item, txt)`. This way, only a pointer to `txt` will be stored. The text then shouldn't be deallocated while the button exists.

#### Check, uncheck, disable
You can manually check, un-check, and disable the single button by using the common state add/clear function:
```c
lv_obj_add_state(rb_item, LV_STATE_CHECKED);   /*Make the button checked*/
lv_obj_clear_state(rb_item, LV_STATE_CHECKED); /*Make the button unchecked*/
lv_obj_add_state(rb_item, LV_STATE_CHECKED | LV_STATE_DISABLED); /*Make the button checked and disabled*/
```

## Events
- `LV_EVENT_VALUE_CHANGED` Sent when the button is toggled.
- `LV_EVENT_DRAW_PART_BEGIN` and `LV_EVENT_DRAW_PART_END` are sent for the following types:
    - `LV_RADIOBTN_DRAW_PART_BOX` The tickbox of the button
        - `part`: `LV_PART_INDICATOR`
        - `draw_area`: the area of the tickbox
        - `rect_dsc`
    - `LV_RADIOBTN_DRAW_PART_BOX_INNER` The inner box of the button
        - `part`: `LV_PART_CUSTOM_FIRST`
        - `draw_area`: the area of the inner box
        - `rect_dsc`

See the events of the [Base object](/widgets/obj) too.

Learn more about [Events](/overview/event).

## Keys
No *Keys* are processed by the object type.

Learn more about [Keys](/overview/indev).

## Example

```eval_rst

.. include:: ../../../examples/widgets/radiobtn/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_radiobtn.h
  :project: lvgl

```
