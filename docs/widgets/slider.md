```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/slider.md
```
# Slider (lv_slider)

## Overview

The Slider object looks like a [Bar](/widgets/bar) supplemented with a knob. The knob can be dragged to set a value. The Slider also can be vertical or horizontal.


## Parts and Styles
The Slider's main part is called `LV_SLIDER_PART_BG` and it uses the typical background style properties.

`LV_SLIDER_PART_INDIC` is a virtual part which also uses all the typical background properties. 
By default, the indicator maximal size is the same as the background's size but setting positive padding values in `LV_SLIDER_PART_BG` will make the indicator smaller. (negative values will make it larger)
If the *value* style property is used on the indicator the alignment will be calculated based on the current size of the indicator. 
For example a center aligned value is always shown in the middle of the indicator regardless it's current size.

`LV_SLIDER_PART_KNOB` is a virtual part using all the typical background properties to describe the knob(s). Similarly to the *indicator* the *value* text is also aligned to the current position and size of the knob.
By default the knob is square (with a radius) with side length equal to the smaller side of the slider. The knob can be made larger with the *padding* values. Padding values can be asymmetric too. 

## Usage

### Value and range
To set an initial value use `lv_slider_set_value(slider, new_value, LV_ANIM_ON/OFF)`. 
`lv_slider_set_anim_time(slider, anim_time)` sets the animation time in milliseconds.

To specify the range (min, max values) the `lv_slider_set_range(slider, min , max)` can be used.

### Symmetrical and Range
Besides the normal type the Slider can be configured in two additional types:
- `LV_SLIDER_TYPE_NORMAL` normal type
- `LV_SLIDER_TYPE_SYMMETRICAL` draw the indicator symmetrical to zero (drawn from zero, left to right)
- `LV_SLIDER_TYPE_RANGE` allow the use of an additional knob for the left (start) value. (Can be used with `lv_slider_set/get_left_value()`)

The type can be changed with `lv_slider_set_type(slider, LV_SLIDER_TYPE_...)`

### Knob-only mode
Normally, the slider can be adjusted either by dragging the knob, or clicking on the slider bar. 
In the latter case the knob moves to the point clicked and slider value changes accordingly. In some cases it is desirable to set the slider to react on dragging the knob only.

This feature is enabled by calling `lv_obj_set_adv_hittest(slider, true);`.

## Events
Besides the [Generic events](../overview/event.html#generic-events) the following [Special events](../overview/event.html#special-events) are sent by the Slider:
- **LV_EVENT_VALUE_CHANGED** Sent while the slider is being dragged or changed with keys. The event is sent continuously while the slider is dragged and only when it is released. Use `lv_slider_is_dragged` to decide whether is slider is being dragged or just released.

## Keys
- **LV_KEY_UP**, **LV_KEY_RIGHT** Increment the slider's value by 1
- **LV_KEY_DOWN**, **LV_KEY_LEFT** Decrement the slider's value by 1

Learn more about [Keys](/overview/indev).

## Example

```eval_rst

.. include:: /lv_examples/src/lv_ex_widgets/lv_ex_slider/index.rst

```


## API 

```eval_rst

.. doxygenfile:: lv_slider.h
  :project: lvgl
        
```
