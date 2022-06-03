# LED (lv_led)

## Overview

The LEDs are rectangle-like (or circle) object whose brightness can be adjusted. With lower brightness the colors of the LED become darker.

## Parts and Styles
The LEDs have only one main part, called `LV_LED_PART_MAIN` and it uses all the typical background style properties.

## Usage

### Color
You can set the color of the LED with `lv_led_set_color(led, lv_color_hex(0xff0080))`.
This will be used as background color, border color, and shadow color.

### Brightness
You can set their brightness with `lv_led_set_bright(led, bright)`. The brightness should be between 0 (darkest) and 255 (lightest).

### Toggle
Use `lv_led_on(led)` and `lv_led_off(led)` to set the brightness to a predefined ON or OFF value. The `lv_led_toggle(led)` toggles between the ON and OFF state.

## Events
- `LV_EVENT_DRAW_PART_BEGIN` and `LV_EVENT_DRAW_PART_END` is sent for the following types:
    - `LV_LED_DRAW_PART_RECTANGLE` The main rectangle. `LV_OBJ_DRAW_PART_RECTANGLE` is not sent by the base object.
       - `part`: `LV_PART_MAIN`
       - `rect_dsc`
       - `draw_area`: the area of the rectangle


See the events of the [Base object](/widgets/obj) too.

Learn more about [Events](/overview/event).

## Keys
No *Keys* are processed by the object type.

Learn more about [Keys](/overview/indev).

## Example

```eval_rst

.. include:: ../../../examples/widgets/led/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_led.h
  :project: lvgl

```
