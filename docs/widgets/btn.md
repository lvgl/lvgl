```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/btn.md
```
# Button (lv_btn)

## Overview

Buttons are simple rectangle-like objects. They are derived from [Containers](/widgets/cont) so [layout](/widgets/cont#layout) and [fit](/widgets/cont#fit) are also available. 
Besides, it can be enabled to automatically go to checked state on click.


## Parts and Styles
The buttons has only a main style called `LV_BTN_PART_MAIN` and it can use all the properties from the following groups:
- background
- border
- outline
- shadow
- value
- pattern
- transitions

It also uses the *padding* properties when *layout* or *fit* is enabled.

## Usage

### States
To make buttons usage simpler the button's state can be get with `lv_btn_get_state(btn)`. It returns one of the following values:
- **LV_BTN_STATE_RELEASED**
- **LV_BTN_STATE_PRESSED**
- **LV_BTN_STATE_CHECKED_RELEASED**
- **LV_BTN_STATE_CHECKED_PRESSED**
- **LV_BTN_STATE_DISABLED**
- **LV_BTN_STATE_CHECKED_DISABLED**

With `lv_btn_set_state(btn, LV_BTN_STATE_...)` the buttons state can be changed manually.

If a more precise description of the state is required (e.g. focused) the general `lv_obj_get_state(btn)` can be used.


### Checkable
You can configure the buttons as *toggle button* with `lv_btn_set_checkable(btn, true)`. In this case, on click, the button goes to `LV_STATE_CHECKED` state automatically, or back when clicked again.


### Layout and Fit
Similarly to [Containers](/widgets/cont), buttons also have layout and fit attributes.
- `lv_btn_set_layout(btn, LV_LAYOUT_...) `set a layout. The default is `LV_LAYOUT_CENTER`.
So, if you add a label, then it will be automatically aligned to the middle and can't be moved with `lv_obj_set_pos()`.
You can disable the layout with `lv_btn_set_layout(btn, LV_LAYOUT_OFF)`.
- `lv_btn_set_fit/fit2/fit4(btn, LV_FIT_..)` enables to set the button width and/or height automatically according to the children, parent, and fit type.


## Events
Besides the [Generic events](../overview/event.html#generic-events) the following [Special events](../overview/event.html#special-events) are sent by the buttons:
 - **LV_EVENT_VALUE_CHANGED** - sent when the button is toggled.

Learn more about [Events](/overview/event).

## Keys
The following *Keys* are processed by the Buttons:
- **LV_KEY_RIGHT/UP** - Go to toggled state if toggling is enabled.
- **LV_KEY_LEFT/DOWN** - Go to non-toggled state if toggling is enabled.

Note that, the state of `LV_KEY_ENTER` is translated to `LV_EVENT_PRESSED/PRESSING/RELEASED` etc.

Learn more about [Keys](/overview/indev).

## Example
```eval_rst

.. include:: /lv_examples/src/lv_ex_widgets/lv_ex_btn/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_btn.h
  :project: lvgl

```
