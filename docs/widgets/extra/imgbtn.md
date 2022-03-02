```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/extra/imgbtn.md
```
# Image button (lv_imgbtn)

## Overview

The Image button is very similar to the simple 'Button' object. The only difference is that it displays user-defined images in each state instead of drawing a rectangle.

You can set a left, right and center image, and the center image will be repeated to match the width of the object.



## Parts and Styles
- `LV_PART_MAIN` Refers to the image(s). If background style properties are used, a rectangle will be drawn behind the image button.

## Usage

### Image sources
To set the image in a state, use the `lv_imgbtn_set_src(imgbtn, LV_IMGBTN_STATE_..., src_left, src_center, src_right)`.
 
The image sources work the same as described in the [Image object](/widgets/core/img) except that "Symbols" are not supported by the Image button.
Any of the sources can `NULL`.

The possible states are:
- `LV_IMGBTN_STATE_RELEASED`
- `LV_IMGBTN_STATE_PRESSED`
- `LV_IMGBTN_STATE_DISABLED`
- `LV_IMGBTN_STATE_CHECKED_RELEASED`
- `LV_IMGBTN_STATE_CHECKED_PRESSED`
- `LV_IMGBTN_STATE_CHECKED_DISABLED`

If you set sources only in `LV_IMGBTN_STATE_RELEASED`, these sources will be used in other states too. 
If you set e.g. `LV_IMGBTN_STATE_PRESSED` they will be used in pressed state instead of the released images.


### States
Instead of the regular `lv_obj_add/clear_state()` functions the `lv_imgbtn_set_state(imgbtn, LV_IMGBTN_STATE_...)` functions should be used to manually set a state.


## Events
- `LV_EVENT_VALUE_CHANGED` Sent when the button is toggled.

Learn more about [Events](/overview/event).

## Keys
- `LV_KEY_RIGHT/UP`  Go to toggled state if `LV_OBJ_FLAG_CHECKABLE` is enabled.
- `LV_KEY_LEFT/DOWN`  Go to non-toggled state if `LV_OBJ_FLAG_CHECKABLE` is enabled.
- `LV_KEY_ENTER` Clicks the button


Learn more about [Keys](/overview/indev).

## Example

```eval_rst

.. include:: ../../../examples/widgets/imgbtn/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_imgbtn.h
  :project: lvgl

```
