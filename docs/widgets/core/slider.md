```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/slider.md
```
# Slider (lv_slider)

## Overview

The Slider object looks like a [Bar](/widgets/core/bar) supplemented with a knob. The knob can be dragged to set a value. The Slider also can be vertical or horizontal.


## Parts and Styles 
- `LV_PART_MAIN` The background of the slider and it uses all the typical background style properties. `padding` makes the indicator smaller in the respective direction.
- `LV_PART_INDICATOR` The indicator the show the current state of the slider. Also uses all the typical background style properties.
- `LV_PART_KNOB` A rectangle (or circle) drawn at the current value. It also uses all the typical background properties to describe the knob(s). By default the knob is square (with a optional radius) with side length equal to the smaller side of the slider. The knob can be made larger with the `padding` values. Padding values can be asymmetric too. 

## Usage

### Value and range
To set an initial value use `lv_slider_set_value(slider, new_value, LV_ANIM_ON/OFF)`.  The animation time is set by the styles' `anim_time` property.

To specify the range (min, max values) the `lv_slider_set_range(slider, min , max)` can be used.

### Modes
The slider can be one the following modes:
- `LV_SLIDER_MODE_NORMAL` A normal slider as described above
- `LV_SLIDER_SYMMETRICAL` Draw the indicator form the zero value to current value. Requires negaitve minimum range and positive maximum range.
- `LV_SLIDER_RANGE` Allows setting the start value too by `lv_bar_set_start_value(bar, new_value, LV_ANIM_ON/OFF)`. The start value has to be always smaller than the end value.

The mode can be changed with `lv_slider_set_mode(slider, LV_SLIDER_MODE_...)`

### Knob-only mode
Normally, the slider can be adjusted either by dragging the knob, or clicking on the slider bar. 
In the latter case the knob moves to the point clicked and slider value changes accordingly. In some cases it is desirable to set the slider to react on dragging the knob only.

This feature is enabled by adding the `LV_OBJ_FLAG_ADV_HITTEST`: `lv_obj_add_flag(slider, LV_OBJ_FLAG_ADV_HITTEST)`.

## Events
- `LV_EVENT_VALUE_CHANGED` Sent while the slider is being dragged or changed with keys. 
The event is sent continuously while the slider is dragged and only when it is released. Use `lv_slider_is_dragged` to decide whether is slider is being dragged or just released.

Learn more about [Events](/overview/event).
## Keys
- `LV_KEY_UP/RIGHT` Increment the slider's value by 1
- `LV_KEY_DOWN/LEFT` Decrement the slider's value by 1

Learn more about [Keys](/overview/indev).

## Example

```eval_rst

.. include:: ../../../examples/widgets/slider/index.rst

```

## API 

```eval_rst

.. doxygenfile:: lv_slider.h
  :project: lvgl
        
```
