```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/linemeter.md
```
# Line meter (lv_lmeter)


## Overview

The Line meter object consists of some radial lines which draw a scale. Setting a value for the Line meter will change the color of the scale lines proportionally. 

## Parts and Styles
The Line meter has only a main part, called `LV_LINEMETER_PART_MAIN`. It uses all the typical background properties the draw a rectangle-like or circle background and the *line* and *scale* properties to draw the scale lines. 
The active lines (which are related to smaller values the the current value) are colored from *line_color* to *scale_grad_color*. The lines in the end (after the current value) are set to *scale_end_color* color.

## Usage

### Set value
When setting a new value with `lv_linemeter_set_value(linemeter, new_value)` the proportional part of the scale will be recolored. 

### Range and Angles
The `lv_linemeter_set_range(linemeter, min, max)` function sets the range of the line meter. 

You can set the angle of the scale and the number of the lines by: `lv_linemeter_set_scale(linemeter, angle, line_num)`. 
The default angle is 240 and the default line number is 31.

### Angle offset
By default the scale angle is interpreted symmetrically to the y axis. It results in "standing" line meter. With `lv_linemeter_set_angle_offset` an offset can be added the scale angle. 
It can used e.g to put a quarter line meter into a corner or a half line meter to the right or left side. 

### Mirror

By default the Line meter's lines are activated clock-wise. It can be changed using `lv_linemeter_set_mirror(linemeter, true/false)`.

## Events
Only the [Generic events](../overview/event.html#generic-events) are sent by the object type.

Learn more about [Events](/overview/event).

## Keys
No *Keys* are processed by the object type.

Learn more about [Keys](/overview/indev).

## Example

```eval_rst

.. include:: /lv_examples/src/lv_ex_widgets/lv_ex_linemeter/index.rst

```

## API 

```eval_rst

.. doxygenfile:: lv_linemeter.h
  :project: lvgl
        
```

