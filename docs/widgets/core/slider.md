# Slider (lv_slider)

## Overview

The Slider object looks like a [Bar](/widgets/core/bar) supplemented with a knob. The knob can be dragged to set a value. Just like Bar, Slider can be vertical or horizontal.


## Parts and Styles
- `LV_PART_MAIN` The background of the slider. Uses all the typical background style properties. `padding` makes the indicator smaller in the respective direction.
- `LV_PART_INDICATOR` The indicator that shows the current state of the slider. Also uses all the typical background style properties.
- `LV_PART_KNOB` A rectangle (or circle) drawn at the current value. Also uses all the typical background properties to describe the knob(s). By default, the knob is square (with an optional corner radius) with side length equal to the smaller side of the slider. The knob can be made larger with the `padding` values. Padding values can be asymmetric too.

## Usage

### Value and range
To set an initial value use `lv_slider_set_value(slider, new_value, LV_ANIM_ON/OFF)`.  The animation time is set by the styles' `anim_time` property.

To specify the range (min, max values), `lv_slider_set_range(slider, min , max)` can be used.

### Modes
The slider can be one of the following modes:
- `LV_SLIDER_MODE_NORMAL` A normal slider as described above
- `LV_SLIDER_SYMMETRICAL` Draw the indicator form the zero value to current value. Requires negative minimum range and positive maximum range.
- `LV_SLIDER_RANGE` Allows setting the start value too by `lv_bar_set_start_value(bar, new_value, LV_ANIM_ON/OFF)`. The start value has to be always smaller than the end value.

The mode can be changed with `lv_slider_set_mode(slider, LV_SLIDER_MODE_...)`

### Knob-only mode
Normally, the slider can be adjusted either by dragging the knob, or by clicking on the slider bar.
In the latter case the knob moves to the point clicked and slider value changes accordingly. In some cases it is desirable to set the slider to react on dragging the knob only. This feature is enabled by adding the `LV_OBJ_FLAG_ADV_HITTEST`: `lv_obj_add_flag(slider, LV_OBJ_FLAG_ADV_HITTEST)`.

The extended click area (set by `lv_obj_set_ext_click_area(slider, value)`) increases to knob's click area.

## Events
- `LV_EVENT_VALUE_CHANGED` Sent while the slider is being dragged or changed with keys. The event is sent continuously while the slider is being dragged.
- `LV_EVENT_RELEASED` Sent when the slider has just been released.
- `LV_EVENT_DRAW_PART_BEGIN` and `LV_EVENT_DRAW_PART_END` are sent for the following parts.
    - `LV_SLIDER_DRAW_PART_KNOB` The main (right) knob of the slider
        - `part`: `LV_PART_KNOB`
        - `draw_area`: area of the indicator
        - `rect_dsc`
        - `id`: 0
    - `LV_SLIDER_DRAW_PART_KNOB`  The left knob of the slider
        - `part`: `LV_PART_KNOB`
        - `draw_area`: area of the indicator
        - `rect_dsc`
        - `id`: 1

See the events of the [Bar](/widgets/core/bar) too.

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
