```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/cpicker.md
```
# color picker (lv_cpicker)

## Overview
As its name implies *Color picker* allows to select color. The Hue, Saturation and Value of the color can be selected after each other. 

The widget has two forms: circle (disc) and rectangle.

In both forms, be long pressing the object, the color picker will change to the next parameter of the color (hue, saturation or value).
Besides, double click will reset the current parameter.

## Parts and Styles
The Color picker's main part is called `LV_CPICKER_PART_BG`. In circular form it uses *scale_width* to set the the width of the circle and *pad_inner* for padding between the circle and the inner preview circle. 
In rectangle mode *radius* can be used to apply a radius on the rectangle.

The object has  virtual part called `LV_CPICKER_PART_KNOB` which is rectangle (or circle) drawn on the current value. 
It uses all the rectangle like style properties and padding to make it larger than the width of the circle or rectangle background.

## Usage

### Type

The type of the Color picker can be changed with `lv_cpicker_set_type(cpicker, LV_CPICKER_TYPE_RECT/DISC)`


### Set color

The colro can be set manually with `lv_cpicker_set_hue/saturation/value(cpicker, x)` or all at once with `lv_cpicker_set_hsv(cpicker, hsv)` or `lv_cpicker_set_color(cpicker, rgb)`

### Color mode

The current color moed can be manually selected with `lv_cpicker_set_color_mode(cpicker, LV_CPICKER_COLOR_MODE_HUE/SATURATION/VALUE)`.

The color moe be fixed (do not change with long press) using `lv_cpicker_set_color_mode_fixed(cpicker, true)`

### Knob color
`lv_cpicker_set_knob_colored(cpicker, true)` make the knob to automatically show the selected color as background color.

## Events
Only the [Generic events](../overview/event.html#generic-events) are sent by the object type.

Learn more about [Events](/overview/event).

## Keys
- **LV_KEY_UP**, **LV_KEY_RIGHT** Increment the current parameter's value by 1
- **LV_KEY_DOWN**, **LV_KEY_LEFT** Decrement the current parameter's by 1
- **LV_KEY_ENTER** By long press the next mode will be shown. By double click the current parameter will be reset.

Learn more about [Keys](/overview/indev).

## Example

```eval_rst

.. include:: /lv_examples/src/lv_ex_widgets/lv_ex_cpicker/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_cpicker.h
  :project: lvgl

```
