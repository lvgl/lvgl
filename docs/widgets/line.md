# Line (lv_line)

## Overview
The Line object is capable of drawing straight lines between a set of points.

## Parts and Styles
- `LV_PART_MAIN` uses all the typical background properties and line style properties.

## Usage

### Set points
The points have to be stored in an `lv_point_t` array and passed to the object by the `lv_line_set_points(lines, point_array, point_cnt)` function.

### Auto-size
By default, the Line's width and height are set to `LV_SIZE_CONTENT`. This means it will automatically set its size to fit all the points. If the size is set explicitly, parts on the line may not be visible.

### Invert y
By default, the *y == 0* point is in the top of the object. It might be counter-intuitive in some cases so the y coordinates can be inverted with `lv_line_set_y_invert(line, true)`. In this case,  *y == 0* will be the bottom of the object.
*y invert* is disabled by default.

## Events
Only the [Generic events](../overview/event.html#generic-events) are sent by the object type.

See the events of the [Base object](/widgets/obj) too.

Learn more about [Events](/overview/event).

## Keys
No *Keys* are processed by the object type.

Learn more about [Keys](/overview/indev).

## Example

```eval_rst

.. include:: ../../../examples/widgets/line/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_line.h
  :project: lvgl

```
