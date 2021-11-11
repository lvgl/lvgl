```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/table.md
```
# Table (lv_table)

## Overview

Tables, as usual, are built from rows, columns, and cells containing texts.

The Table object is very lightweight because only the texts are stored. No real objects are created for cells but they are just drawn on the fly.

The Table is added to the default group (if it is set). Besides the Table is an editable object to allow selecting a cell with encoder navigation too.

## Parts and Styles
- `LV_PART_MAIN` The background of the table uses all the typical background style properties.
- `LV_PART_ITEMS` The cells of the table also use all the typical background style properties and the text properties.


## Usage

### Set cell value

The cells can store only text so numbers need to be converted to text before displaying them in a table.

`lv_table_set_cell_value(table, row, col, "Content")`. The text is saved by the table so it can be even a local variable.

Line breaks can be used in the text like `"Value\n60.3"`.

New rows and columns are automatically added is required 

### Rows and Columns

To explicitly set number of rows and columns use `lv_table_set_row_cnt(table, row_cnt)` and `lv_table_set_col_cnt(table, col_cnt)`

### Width and Height

The width of the columns can be set with `lv_table_set_col_width(table, col_id, width)`. The overall width of the Table object will be set to the sum of columns widths.

The height is calculated automatically from the cell styles (font, padding etc) and the number of rows.

### Merge cells

Cells can be merged horizontally with `lv_table_add_cell_ctrl(table, row, col, LV_TABLE_CELL_CTRL_MERGE_RIGHT)`. To merge more adjacent cells call this function for each cell.

### Scroll
If the label's width or height is set to `LV_SIZE_CONTENT` that size will be used to show the whole table in the respective direction. 
E.g. `lv_obj_set_size(table, LV_SIZE_CONTENT, LV_SIZE_CONTENT)` automatically sets the table size to show all the columns and rows.

If the width or height is set to a smaller number than the "intrinsic" size then the table becomes scrollable.

## Events
- `LV_EVENT_VALUE_CHANGED` Sent when a new cell is selected with keys.
- `LV_EVENT_DRAW_PART_BEGIN` and `LV_EVENT_DRAW_PART_END` are sent for the following types:
    - `LV_TABLE_DRAW_PART_CELL` The individual cells of the table
        - `part`: `LV_PART_ITEMS` 
        - `draw_area`: area of the indicator
        - `rect_dsc`
        - `label_dsc`
        - `id`: current row &times; col count + current column 

See the events of the [Base object](/widgets/obj) too.

Learn more about [Events](/overview/event).

## Keys

The following *Keys* are processed by the Tables:
- `LV_KEY_RIGHT/LEFT/UP/DOWN/` Select a cell.

Note that, as usual, the state of `LV_KEY_ENTER` is translated to `LV_EVENT_PRESSED/PRESSING/RELEASED` etc.

`lv_table_get_selected_cell(table, &row, &col)` can be sued the get currentéy selected cell. Row and column eill be set to `LV_TABLE_CELL_NONE` no cell is celected.

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
