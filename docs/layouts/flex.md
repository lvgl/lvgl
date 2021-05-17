```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/layouts/flex.md
```

# Flex

## Overview

The Flexbox (or Flex for short) is a subset of [CSS Flexbox](https://css-tricks.com/snippets/css/a-guide-to-flexbox/).

It can arrange items into rows or columns (tracks), handle wrapping, adjust the spacing between the items and tracks, handle *grow* to make the item(s) will the remaining space with respect to min/max width and height.

To make an object flex container call `lv_obj_set_layout(obj, LV_LAYOUT_FLEX)`.

Note that, the flex layout needs to enabled with `LV_USE_FLEX` in `lv_conf.h`. 

## Terms
- tracks: the rows or columns
- main direction: row or column, the direction in which the items are placed
- cross direction: perpendicular to the main direction
- wrap: if there there is no more space in the track a new track is started
- grow: if set on an item it will grow to fill the remaining space on the track. 
The available space will be distributed among items respective to the their grow value (larger value means more space)
- gap: the space between the rows and columns or the items on a track

## Simple interface

With follwoing functions you can simple set a Flex layout on any parent.

### Flex flow

`lv_obj_set_flex_flow(obj, felx_flow)`

The possible values for `felx_flow` are:
- `LV_FLEX_FLOW_ROW` Place the children in a row without wrapping
- `LV_FLEX_FLOW_COLUMN` Place the children in a column without wrapping
- `LV_FLEX_FLOW_ROW_WRAP` Place the children in a row with wrapping
- `LV_FLEX_FLOW_COLUMN_WRAP` Place the children in a column with wrapping
- `LV_FLEX_FLOW_ROW_REVERSE` Place the children in a row without wrapping but in reversed order
- `LV_FLEX_FLOW_COLUMN_REVERSE` Place the children in a column without wrapping but in reversed order
- `LV_FLEX_FLOW_ROW_WRAP_REVERSE` Place the children in a row without wrapping but in reversed order
- `LV_FLEX_FLOW_COLUMN_WRAP_REVERSE` Place the children in a column without wrapping but in reversed order

### Flex align
To manage the placement of the children use `lv_obj_set_flex_align(obj,  main_place, cross_place, track_cross_place)`

- `main_place` tells how to distribute the items in their track on the main axis. E.g. flush the items to the right on `LV_FLEX_FLOW_ROW_WRAP`.  (It's called `justify-content` in CSS)
- `cross_place` tells how to distribute the items in their track on the cross axis. E.g. if the items have differetn height place them to the bottom of the track.  (It's called `align-items` in CSS)
- `track_cross_place` tells how to distribute the tracks (It's called `align-content` in CSS)

The possible values are:
- `LV_FLEX_ALIGN_START` means left on a horizontally and top vertically. (default)
- `LV_FLEX_ALIGN_END` means right on a horizontally and bottom vertically
- `LV_FLEX_ALIGN_CENTER` simply center
- `LV_FLEX_ALIGN_SPACE_EVENLY` items are distributed so that the spacing between any two items (and the space to the edges) is equal. Not applies to `track_cross_place`.
- `LV_FLEX_ALIGN_SPACE_AROUND` items are evenly distributed in the track with equal space around them. 
Note that visually the spaces aren’t equal, since all the items have equal space on both sides. 
The first item will have one unit of space against the container edge, but two units of space between the next item because that next item has its own spacing that applies. Not applies to `track_cross_place`.
- `LV_FLEX_ALIGN_SPACE_BETWEEN` items are evenly distributed in the track: first item is on the start line, last item on the end line. Not applies to `track_cross_place`.


### Flex grow

Flex grow can be used to make one or more children fill the available space on the track. If more children has grow the available space will be distributed proportionally to the grow values.
For example let's there is 400 px remaining space and 4 object with grow:
- `A` with grow = 1
- `B` with grow = 1
- `c` with grow = 2

`A` and `B` will have 100 px size, and `C` will have 200 px size.

Flex grow can be set on a child with `lv_obj_set_flex_flow(child, value)`. `value` needs to be &gt; 1 or 0 to disable grow on the child.


## Style interface

All the Flex related values are style properties under the hood and you can use them similarly to any other style property. The following flex related style properties exist:

- `FLEX_FLOW`
- `FLEX_MAIN_PLACE`
- `FLEX_CROSS_PLACE`
- `FLEX_TRACK_PLACE`
- `FLEX_GROW`

## Other features 

### RTL
If the base direction of the container is set the `LV_BASE_DIR_RTL` the meaning of `LV_FLEX_ALIGN_START` and `LV_FLEX_ALIGN_END` is swapped on `ROW` layouts. I.e. `START` will mean right.

The items on `ROW` layouts, and tracks of `COLUMN` layouts will be placed from right to left.

### New track

You can force Flex to put an item into a new line with `lv_obj_add_flag(child, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK)`.


## Example

```eval_rst

.. include:: ../../examples/layouts/flex/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_flex.h
  :project: lvgl

```
