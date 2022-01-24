```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/bar.md
```
# Bar (lv_bar)

## Overview

The bar object has a background and an indicator on it. The width of the indicator is set according to the current value of the bar. 

Vertical bars can be created if the width of the object is smaller than its height.

Not only the end, but also the start value of the bar can be set, which changes the start position of the indicator.


## Parts and Styles
- `LV_PART_MAIN` The background of the bar and it uses the typical background style properties. Adding padding makes the indicator smaller or larger. The `anim_time` style property sets the animation time if the values set with `LV_ANIM_ON`.
- `LV_PART_INDICATOR` The indicator itself; also uses all the typical background properties.
 
## Usage

### Value and range
A new value can be set by `lv_bar_set_value(bar, new_value, LV_ANIM_ON/OFF)`.
The value is interpreted in a range (minimum and maximum values) which can be modified with `lv_bar_set_range(bar, min, max)`.
The default range is 1..100.

The new value in `lv_bar_set_value` can be set with or without an animation depending on the last parameter (`LV_ANIM_ON/OFF`).

### Modes
The bar can be one of the following modes:
- `LV_BAR_MODE_NORMAL` A normal bar as described above
- `LV_BAR_SYMMETRICAL` Draw the indicator from the zero value to current value. Requires a negative minimum range and positive maximum range.
- `LV_BAR_RANGE` Allows setting the start value too by `lv_bar_set_start_value(bar, new_value, LV_ANIM_ON/OFF)`. The start value always has to be smaller than the end value.

## Events
- `LV_EVENT_DRAW_PART_BEGIN` and `LV_EVENT_DRAW_PART_END` are sent for the following parts:
    - `LV_BAR_DRAW_PART_INDICATOR` The indicator of the bar
        - `part`: `LV_PART_INDICATOR` 
        - `draw_area`: area of the indicator
        - `rect_dsc`
        
See the events of the [Base object](/widgets/obj) too.

Learn more about [Events](/overview/event).

## Keys
No *Keys* are processed by the object type.

Learn more about [Keys](/overview/indev).

## Example

```eval_rst

.. include:: ../../../examples/widgets/bar/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_bar.h
  :project: lvgl

```
