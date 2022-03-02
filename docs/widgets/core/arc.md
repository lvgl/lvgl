```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/core/arc.md
```
# Arc (lv_arc)

## Overview

The Arc consists of a background and a foreground arc. The foreground (indicator) can be touch-adjusted.

## Parts and Styles
- `LV_PART_MAIN`  Draws a background using the typical background style properties and an arc using the arc style properties. The arc's size and position will respect the *padding* style properties.
- `LV_PART_INDICATOR` Draws another arc using the *arc* style properties. Its padding values are interpreted relative to the background arc. 
- `LV_PART_KNOB` Draws a handle on the end of the indicator using all background properties and padding values. With zero padding the knob size is the same as the indicator's width. 
Larger padding makes it larger, smaller padding makes it smaller. 

## Usage

### Value and range

A new value can be set using `lv_arc_set_value(arc, new_value)`. 
The value is interpreted in a range (minimum and maximum values) which can be modified with `lv_arc_set_range(arc, min, max)`.
The default range is 0..100.

The indicator arc is drawn on the main part's arc. This if the value is set to maximum the indicator arc will cover the entire "background" arc.
To set the start and end angle of the background arc use the `lv_arc_set_bg_angles(arc, start_angle, end_angle)` functions or `lv_arc_set_bg_start/end_angle(arc, angle)`. 

Zero degrees is at the middle right (3 o'clock) of the object and the degrees are increasing in clockwise direction.
The angles should be in the [0;360] range.

### Rotation

An offset to the 0 degree position can be added with `lv_arc_set_rotation(arc, deg)`.

### Mode

The arc can be one of the following modes:
- `LV_ARC_MODE_NORMAL` The indicator arc is drawn from the minimum value to the current.
- `LV_ARC_MODE_REVERSE` The indicator arc is drawn counter-clockwise from the maximum value to the current.
- `LV_ARC_MODE_SYMMETRICAL` The indicator arc is drawn from the middle point to the current value.

The mode can be set by `lv_arc_set_mode(arc, LV_ARC_MODE_...)` and used only if the angle is set by `lv_arc_set_value()` or the arc is adjusted by finger.

### Change rate
If the arc is pressed the current value will set with a limited speed according to the set *change rate*. 
The change rate is defined in degree/second unit and can be set with `lv_arc_set_change_rage(arc, rate)`


### Setting the indicator manually
It's also possible to set the angles of the indicator arc directly with `lv_arc_set_angles(arc, start_angle, end_angle)` function or `lv_arc_set_start/end_angle(arc, start_angle)`.
In this case the set "value" and "mode" are ignored.

In other words, the angle and value settings are independent. You should exclusively use one or the other. Mixing the two might result in unintended behavior.

To make the arc non-adjustable, remove the style of the knob and make the object non-clickable:
```c
lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
lv_obj_clear_flag(arc, LV_OBJ_FLAG_CLICKABLE);
```

### Advanced hit test

If the `LV_OBJ_FLAG_ADV_HITTEST` flag is enabled the arc can be clicked through in the middle. Clicks are recognized only on the ring of the background arc. `lv_obj_set_ext_click_size()` makes the sensitive area larger inside and outside with the given number of pixels. 




## Events
- `LV_EVENT_VALUE_CHANGED` sent when the arc is pressed/dragged to set a new value.
- `LV_EVENT_DRAW_PART_BEGIN` and `LV_EVENT_DRAW_PART_END` are sent with the following types:
    - `LV_ARC_DRAW_PART_BACKGROUND` The background arc. 
        - `part`: `LV_PART_MAIN`
        - `p1`: center of the arc
        - `radius`: radius of the arc
        - `arc_dsc`
    - `LV_ARC_DRAW_PART_FOREGROUND` The foreground arc.  
        - `part`: `LV_PART_INDICATOR`
        - `p1`: center of the arc
        - `radius`: radius of the arc
        - `arc_dsc`
    - LV_ARC_DRAW_PART_KNOB The knob
        - `part`: `LV_PART_KNOB`
        - `draw_area`: the area of the knob
        - `rect_dsc`:
    
See the events of the [Base object](/widgets/obj) too.
    
Learn more about [Events](/overview/event).

## Keys
- `LV_KEY_RIGHT/UP` Increases the value by one.
- `LV_KEY_LEFT/DOWN` Decreases the value by one.


Learn more about [Keys](/overview/indev).


## Example

```eval_rst

.. include:: ../../../examples/widgets/arc/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_arc.h
  :project: lvgl

```
