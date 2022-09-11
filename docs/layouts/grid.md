
# Grid

## Overview

The Grid layout is a subset of [CSS Flexbox](https://css-tricks.com/snippets/css/complete-guide-grid/).

It can arrange items into a 2D "table" that has rows or columns (tracks). The item can span through multiple columns or rows.
The track's size can be set in pixel, to the largest item (`LV_GRID_CONTENT`) or in "Free unit" (FR) to distribute the free space proportionally.

To make an object a grid container call `lv_obj_set_layout(obj, LV_LAYOUT_GRID)`.

Note that the grid layout feature of LVGL needs to be globally enabled with `LV_USE_GRID` in `lv_conf.h`.

## Terms
- tracks: the rows or columns
- free unit (FR): if set on track's size is set in `FR` it will grow to fill the remaining space on the parent.
- gap: the space between the rows and columns or the items on a track

## Simple interface

With the following functions you can easily set a Grid layout on any parent.

### Grid descriptors

First you need to describe the size of rows and columns. It can be done by declaring 2 arrays and the track sizes in them. The last element must be `LV_GRID_TEMPLATE_LAST`.

For example:
```
static lv_coord_t column_dsc[] = {100, 400, LV_GRID_TEMPLATE_LAST};   /*2 columns with 100 and 400 ps width*/
static lv_coord_t row_dsc[] = {100, 100, 100, LV_GRID_TEMPLATE_LAST}; /*3 100 px tall rows*/
```

To set the descriptors on a parent use `lv_obj_set_grid_dsc_array(obj, col_dsc, row_dsc)`.

Besides simple settings the size in pixel you can use two special values:
- `LV_GRID_CONTENT` set the width to the largest children on this track
- `LV_GRID_FR(X)` tell what portion of the remaining space should be used by this track. Larger value means larger space.

### Grid items
By default, the children are not added to the grid. They need to be added manually to a cell.

To do this call `lv_obj_set_grid_cell(child, column_align, column_pos, column_span, row_align, row_pos, row_span)`.

`column_align` and `row_align` determine how to align the children in its cell. The possible values are:
- `LV_GRID_ALIGN_START` means left on a horizontally and top vertically. (default)
- `LV_GRID_ALIGN_END` means right on a horizontally and bottom vertically
- `LV_GRID_ALIGN_CENTER` simply center

`colum_pos` and `row_pos` means the zero based index of the cell into the item should be placed.

`colum_span` and `row_span` means how many tracks should the item involve from the start cell. Must be &gt; 1.

### Grid align

If there are some empty space the track can be aligned several ways:
- `LV_GRID_ALIGN_START` means left on a horizontally and top vertically. (default)
- `LV_GRID_ALIGN_END` means right on a horizontally and bottom vertically
- `LV_GRID_ALIGN_CENTER` simply center
- `LV_GRID_ALIGN_SPACE_EVENLY` items are distributed so that the spacing between any two items (and the space to the edges) is equal. Not applies to `track_cross_place`.
- `LV_GRID_ALIGN_SPACE_AROUND` items are evenly distributed in the track with equal space around them.
Note that visually the spaces aren’t equal, since all the items have equal space on both sides.
The first item will have one unit of space against the container edge, but two units of space between the next item because that next item has its own spacing that applies. Not applies to `track_cross_place`.
- `LV_GRID_ALIGN_SPACE_BETWEEN` items are evenly distributed in the track: first item is on the start line, last item on the end line. Not applies to `track_cross_place`.

To set the track's alignment use `lv_obj_set_grid_align(obj, column_align, row_align)`.

## Style interface

All the Grid related values are style properties under the hood and you can use them similarly to any other style properties. The following Grid related style properties exist:

- `GRID_COLUMN_DSC_ARRAY`
- `GRID_ROW_DSC_ARRAY`
- `GRID_COLUMN_ALIGN`
- `GRID_ROW_ALIGN`
- `GRID_CELL_X_ALIGN`
- `GRID_CELL_COLUMN_POS`
- `GRID_CELL_COLUMN_SPAN`
- `GRID_CELL_Y_ALIGN`
- `GRID_CELL_ROW_POS`
- `GRID_CELL_ROW_SPAN`

### Internal padding

To modify the minimum space Grid inserts between objects, the following properties can be set on the Grid container style:

- `pad_row` Sets the padding between the rows.
- `pad_column` Sets the padding between the columns.

## Other features

### RTL
If the base direction of the container is set to `LV_BASE_DIR_RTL`, the meaning of `LV_GRID_ALIGN_START` and `LV_GRID_ALIGN_END` is swapped. I.e. `START` will mean right-most.

The columns will be placed from right to left.


## Example

```eval_rst

.. include:: ../../examples/layouts/grid/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_grid.h
  :project: lvgl

```
