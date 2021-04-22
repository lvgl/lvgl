```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/cont.md
```
# Container (lv_cont)

## Overview

The containers are essentially a **basic object** with layout and automatic sizing features features.


## Parts and Styles
The containers has only a main style called `LV_CONT_PART_MAIN` and it can use all the typicaly bacground properties properties and padding for layout auto sizing.

## Usage


### Layout
You can apply a layout on the containers to automatically order their children. The layout spacing comes from the style's `pad` properties. The possible layout options:

- **LV_LAYOUT_OFF** - Do not align the children.
- **LV_LAYOUT_CENTER** - Align children to the center in column and keep `pad_inner` space between them.
- **LV_LAYOUT_COLUMN_LEFT** - Align children in a left-justified column. Keep `pad_left` space on the left, `pad_top` space on the top and `pad_inner` space between the children.
- **LV_LAYOUT_COLUMN_MID** - Align children in centered column. Keep `pad_top` space on the top and `pad_inner` space between the children.
- **LV_LAYOUT_COLUMN_RIGHT** - Align children in a right-justified column. Keep `pad_right` space on the right, `pad_top` space on the top and `pad_inner` space between the children.
- **LV_LAYOUT_ROW_TOP** - Align children in a top justified row. Keep `pad_left` space on the left, `pad_top` space on the top and `pad_inner` space between the children.
- **LV_LAYOUT_ROW_MID** - Align children in centered row. Keep `pad_left` space on the left and `pad_inner` space between the children.
- **LV_LAYOUT_ROW_BOTTOM** - Align children in a bottom justified row. Keep `pad_left` space on the left, `pad_bottom` space on the bottom and `pad_inner` space between the children.
- **LV_LAYOUT_PRETTY_TOP** - Put as many objects as possible in a row (with at least `pad_inner` space and `pad_left/right` space on the sides). Divide the space in each line equally between the children. 
If here are children with different height in a row align their top edge.
- **LV_LAYOUT_PRETTY_MID** - Same as `LV_LAYOUT_PRETTY_TOP` but if here are children with different height in a row align their middle line.
- **LV_LAYOUT_PRETTY_BOTTOM** -  Same as `LV_LAYOUT_PRETTY_TOP` but if here are children with different height in a row align their bottom line.
- **LV_LAYOUT_GRID** - Similar to `LV_LAYOUT_PRETTY` but not divide horizontal space equally just let `pad_left/right` on the edges and `pad_inner` space between the elements.

### Autofit
Container have an autofit feature which can automatically change the size of the container according to its children and/or its parent. The following options exist:
- **LV_FIT_NONE** - Do not change the size automatically.
- **LV_FIT_TIGHT** - Shrink-wrap the container around all of its children, while keeping `pad_top/bottom/left/right` space on the edges.
- **LV_FIT_PARENT** - Set the size to the parent's size minus `pad_top/bottom/left/right` (from the parent's style) space.
- **LV_FIT_MAX** - Use `LV_FIT_PARENT` while smaller than the parent and `LV_FIT_TIGHT` when larger. It will ensure that the container is, at minimum, the size of its parent.

To set the auto fit mode for all directions, use `lv_cont_set_fit(cont, LV_FIT_...)`.
To use different auto fit horizontally and vertically, use `lv_cont_set_fit2(cont, hor_fit_type, ver_fit_type)`.
To use different auto fit in all 4 directions, use `lv_cont_set_fit4(cont, left_fit_type, right_fit_type, top_fit_type, bottom_fit_type)`.

## Events
Only the [Generic events](../overview/event.html#generic-events) are sent by the object type.

Learn more about [Events](/overview/event).

## Keys
No *Keys* are processed by the object type.

Learn more about [Keys](/overview/indev).


## Example

```eval_rst

.. include:: /lv_examples/src/lv_ex_widgets/lv_ex_cont/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_cont.h
  :project: lvgl

```
