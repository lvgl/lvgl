# Color wheel (lv_colorwheel)

## Overview
As its name implies *Color wheel* allows the user to select a color. The Hue, Saturation and Value of the color can be selected separately.

Long pressing the object, the color wheel will change to the next parameter of the color (hue, saturation or value). A double click will reset the current parameter.

## Parts and Styles
- `LV_PART_MAIN` Only `arc_width` is used to set the width of the color wheel
- `LV_PART_KNOB` A rectangle (or circle) drawn on the current value. It uses all the rectangle like style properties and padding to make it larger than the width of the arc.

## Usage

### Create a color wheel

`lv_colorwheel_create(parent, knob_recolor)` creates a new color wheel. With `knob_recolor=true` the knob's background color will be set to the current color.

### Set color

The color can be set manually with `lv_colorwheel_set_hue/saturation/value(colorwheel, x)` or all at once with `lv_colorwheel_set_hsv(colorwheel, hsv)` or `lv_colorwheel_set_color(colorwheel, rgb)`

### Color mode

The current color mode can be manually selected with `lv_colorwheel_set_mode(colorwheel, LV_COLORWHEEL_MODE_HUE/SATURATION/VALUE)`.

The color mode can be fixed (so as to not change with long press) using `lv_colorwheel_set_mode_fixed(colorwheel, true)`

## Events
- `LV_EVENT_VALUE_CHANGED` Sent if a new color is selected.

Learn more about [Events](/overview/event).

## Keys
- `LV_KEY_UP`, `LV_KEY_RIGHT` Increment the current parameter's value by 1
- `LV_KEY_DOWN`, `LV_KEY_LEFT` Decrement the current parameter's value by 1
- `LV_KEY_ENTER` A long press will show the next mode. Double click to reset the current parameter.

Learn more about [Keys](/overview/indev).

## Example

```eval_rst

.. include:: ../../../examples/widgets/colorwheel/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_colorwheel.h
  :project: lvgl

```
