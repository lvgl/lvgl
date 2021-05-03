```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/arc.md
```
# Arc (lv_arc)

## Overview

The Arc are consists of a background and a foreground arc. The foregrond (indicator) arc can be adjusted by finger.

## Parts and Styles
- `LV_PART_MAIN`  It draws a background using the typical background style properties and an arc using the arc style properties. The arc's size and position will respect the *padding* style properties.
- `LV_PART_INDICATOR` It draws an other arc using the *arc* style properties. It's padding values are interpreted relative to the background arc. 
- `LV_PART_KNOB`It draws a handle on the end of the indicator. It uses all background properties and padding values. With zero padding the knob size is the same as the indicator's width. 
Larger padding makes it larger, smaller padding makes it smaller. 

## Usage

### Value and range

A new value can be set by `lv_arc_set_value(arc, new_value)`. 
The value is interpreted in a range (minimum and maximum values) which can be modified with `lv_arc_set_range(arc, min, max)`.
The default range is 1..100.

The indicator arc is drawn on the main part's arc. That is if the vale is set to maximum the indicator arc will cover the entire "background" arc.
To set the start and end angl of the background arc use  the `lv_arc_set_bg_angles(arc, start_angle, end_angle)` function or `lv_arc_set_bg_start/end_angle(arc, start_angle)`. 

Zero degree is at the middle right (3 o'clock) of the object and the degrees are increasing in clockwise direction.
The angles should be in [0;360] range.

### Rotation

An offset to the 0 degree position can added with `lv_arc_set_rotation(arc, deg)`.

### Mode

The arc can be one of the following modes:
- `LV_ARC_MODE_NORMAL` The indicator arc is drawn from the minimimum value to the current.
- `LV_ARC_MODE_REVERSE` The indicator arc is drawn counter clockwise from the maximum value to the current.
- `LV_ARC_MODE_SYMMETRICAL` The indicator arc is drawn from the middle point to the current value.

The mode can be set by `lv_arc_set_mode(arc, LV_ARC_MODE_...)` and used only if the the angle is set by `lv_arc_set_value()` or the arc is adjusted by finger.

### Change rate
If the the arc is pressed the current value will set with a limited speed according to the set *change rate*. 
The change rate is defined in degree/second unit and can be set with `lv_arc_set_change_rage(arc, rate)`


### Setting the indicator manually
It also possible to set the angles o the indicator arc directly with `lv_arc_set_angles(arc, start_angle, end_angle)` function or `lv_arc_set_start/end_angle(arc, start_angle)` sets the angles of the indicator arc. 
In this case the set "value" and "mode" is ignored.

In other words, settings angles and values are independent. You should use either value and angle settings. Mixing the two might result unintended behavior. 

To make the arc non-adjutabe remove the style of the knob and make the object non-clickable:
```c
lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
lv_obj_clear_flag(arc, LV_OBJ_FLAG_CLICKABLE);
```

## Events
- `LV_EVENT_VALUE_CHANGED` sent when the arc is pressed/dragged to set a new value.
- `LV_EVENT_DRAW_PART_BEGIN` and `LV_EVENT_DRAW_PART_END` are sent for the background rectangle, the background arc, the foreground arc and the knob to allow hooking the drawing. 
For more detail on the backround rectangle part see the [Base object](/widgets/obj#events)'s documentation. The fields of `lv_obj_draw_dsc_t` is set like the followings:
  - For both arcs: `clip_area`, `p1` (center of the arc), `radius`, `arc_dsc`, `part`. 
  - For the knob: `clip_area`, `draw_area`, `rect_dsc`, `part`.


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
