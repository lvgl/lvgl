```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/imgbtn.md
```
# Image button (lv_imgbtn)

## Overview

The Image button is very similar to the simple 'Button' object. The only difference is that, it displays user-defined images in each state instead of drawing a rectangle.
Before reading this section, please read the [Button](/widgets/btn) section for better understanding.

## Parts and Styles
The Image button object has only a main part called `LV_IMG_BTN_PART_MAIN` from where all *image* style properties are used. 
It's possible to recolor the image in each state with *image_recolor* and *image_recolor_opa* proeprties. For example, to make the image darker if it is pressed.

## Usage

### Image sources
To set the image in a state, use the `lv_imgbtn_set_src(imgbtn, LV_BTN_STATE_..., &img_src)`. 
The image sources works the same as described in the [Image object](/widgets/img) except that, "Symbols" are not supported by the Image button.

If `LV_IMGBTN_TILED` is enabled in *lv_conf.h*, then `lv_imgbtn_set_src_tiled(imgbtn, LV_BTN_STATE_..., &img_src_left, &img_src_mid, &img_src_right)` becomes available. 
Using the tiled feature the *middle* image will be repeated to fill the width of the object. 
Therefore with `LV_IMGBTN_TILED`, you can set the width of the Image button using `lv_obj_set_width()`. However, without this option, the width will be always the same as the image source's width.

### Button features

Similarly to normal Buttons `lv_imgbtn_set_checkable(imgbtn, true/false)`, `lv_imgbtn_toggle(imgbtn)` and `lv_imgbtn_set_state(imgbtn, LV_BTN_STATE_...)` also works.

## Events
Beside the [Generic events](../overview/event.html#generic-events), the following [Special events](../overview/event.html#special-events) are sent by the buttons:
 - **LV_EVENT_VALUE_CHANGED** - Sent when the button is toggled.

Note that, the generic input device related events (like `LV_EVENT_PRESSED`) are sent in the inactive state too. You need to check the state with `lv_btn_get_state(btn)` to ignore the events from inactive buttons.

Learn more about [Events](/overview/event).

## Keys
The following *Keys* are processed by the Buttons:
- **LV_KEY_RIGHT/UP** - Go to toggled state if toggling is enabled.
- **LV_KEY_LEFT/DOWN** - Go to non-toggled state if toggling is  enabled.

Note that, as usual, the state of `LV_KEY_ENTER` is translated to `LV_EVENT_PRESSED/PRESSING/RELEASED` etc.

Learn more about [Keys](/overview/indev).

## Example

```eval_rst

.. include:: /lv_examples/src/lv_ex_widgets/lv_ex_imgbtn/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_imgbtn.h
  :project: lvgl

```
