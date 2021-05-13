```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/table.md
```
# Table (lv_table)

## Overview

Tables, as usual, are built from rows, columns, and cells containing texts.

The Table object is very light weighted because only the texts are stored. No real objects are created for cells but they are just drawn on the fly.


## Parts and Styles
- `LV_PART_MAIN` The background of the table and uses all the typical background style properties.
- `LV_PART_ITEMS` The cells of the table and they also use all the typical background style properties and the text properties.


## Usage

### Set cell value

The cells can store only texts so numbers needs to be converted to text before displaying them in a table.

`lv_table_set_cell_value(table, row, col, "Content")`. The text is saved by the table so it can be even a local variable.

Line break can be used in the text like `"Value\n60.3"`.

The new rows and column are automatically added is required 

### Rows and Columns

To explicitly set number of rows and columns use `lv_table_set_row_cnt(table, row_cnt)` and `lv_table_set_col_cnt(table, col_cnt)`

### Width and Height

The width of the columns can be set with `lv_table_set_col_width(table, col_id, width)`. The overall width of the Table object will be set to the sum of columns widths.

The height is calculated automatically from the cell styles (font, padding etc) and the number of rows.

### Merge cells

Cells can be merged horizontally with `lv_table_set_cell_merge_right(table, col, row, true)`. To merge more adjacent cells apply this function for each cell.

### Scroll
If the label's width or height is set to `LV_SIZE_CONTENT` that size will be se to show the whole table in the respective direction. 
E.g. `lv_obj_set_size(table, LV_SIZE_CONTENT, LV_SIZE_CONTENT)` automatically sets the table size to show all the columns and rows.

If the width or height is set to smaller number than the "intrinsic" size then the table becomes scrollable.

## Events
- `LV_EVENT_DRAW_PART_BEGIN` and `LV_EVENT_DRAW_PART_END` are sent for both main and items parts to allow hooking the drawing. 
The for more detail on the main part see the [Base object](/widgets/obj#events)'s documentation.
For the items (sells) the following fields are used: `clip_area`, `draw_area`, `part`, `rect_dsc`, `label_dsc` `id` (current row &times; col count + current column). 


Learn more about [Events](/overview/event).

## Keys

No *Keys* are processed by the object type.

Learn more about [Keys](/overview/indev).

## Example

```eval_rst

.. include:: ../../../examples/widgets/table/index.rst

```

### MicroPython
No examples yet.

## API 

```eval_rst

.. doxygenfile:: lv_table.h
  :project: lvgl
        
```
