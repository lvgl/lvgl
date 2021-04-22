```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/arc.md
```
# Arc (lv_arc)

## Overview

The Arc are consists of a background and a foreground arc. Both can have start and end angles and thickness.

## Parts and Styles
The Arc's main part is called `LV_ARC_PART_MAIN`. It draws a background using the typical background style properties and an arc using the *line* style properties.
The arc's size and position will respect the *padding* style properties.

`LV_ARC_PART_INDIC` is virtual part and it draws an other arc using the *line* style properties. It's padding values are interpreted relative to the background arc. 
The radius of the indicator arc will be modified according to the greatest padding value.

`LV_ARC_PART_KNOB` is virtual part and it draws on the end of the arc indicator. It uses all background properties and padding values. With zero padding the knob size is the same as the indicator's width. 
Larger padding makes it larger, smaller padding makes it smaller. 

## Usage

### Angles

To set the angles of the background, use the `lv_arc_set_bg_angles(arc, start_angle, end_angle)` function or `lv_arc_set_bg_start/end_angle(arc, start_angle)`. 
Zero degree is at the middle right (3 o'clock) of the object and the degrees are increasing in a clockwise direction.
The angles should be in [0;360] range.

Similarly, `lv_arc_set_angles(arc, start_angle, end_angle)` function or `lv_arc_set_start/end_angle(arc, start_angle)` sets the angles of the indicator arc. 

### Rotation

An offset to the 0 degree position can added with `lv_arc_set_rotation(arc, deg)`.


### Range and values

Besides setting angles manually the arc can have a range and a value. To set the range use `lv_arc_set_range(arc, min, max)` and to set a value use `lv_arc_set_value(arc, value)`.
Using range and value the angle of the indicator will be mapped between background angles.

Note that, settings angles and values are independent. You should use either value and angle settings. Mixing the two might result unintended behavior. 

### Type

The arc can have the different "types". They are set with `lv_arc_set_type`.
The following types exist:
- `LV_ARC_TYPE_NORMAL` indicator arc drawn clockwise (min to current)
- `LV_ARC_TYPE_REVERSE` indicator arc drawn counter clockwise (max to current)
- `LV_ARC_TYPE_SYMMETRIC` indicator arc drawn from the middle point to the current value.  


## Events
Besides the [Generic events](../overview/event.html#generic-events) the following [Special events](../overview/event.html#special-events) are sent by the arcs:
 - **LV_EVENT_VALUE_CHANGED** sent when the arc is pressed/dragged to set a new value.

Learn more about [Events](/overview/event).

## Keys
No *Keys* are processed by the object type.

Learn more about [Keys](/overview/indev).


## Example

```eval_rst

.. include:: /lv_examples/src/lv_ex_widgets/lv_ex_arc/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_arc.h
  :project: lvgl

```
