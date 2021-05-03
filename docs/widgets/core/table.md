```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/table.md
```
# Table (lv_table)

## Overview

Tables, as usual, are built from rows, columns, and cells containing texts.

The Table object is very light weighted because only the texts are stored. No real objects are created for cells but they are just drawn on the fly.


## Parts and Styles
The main part of the Table is called `LV_TABLE_PART_BG`. It's a rectangle like background and uses all the typical background style properties.

For the cells there are 4 virtual parts. Every cell has type (1, 2, ... 16) which tells which part's styles to apply on them. The cell parts can be referenced by `LV_TABLE_PART_CELL1 + x` where `x` is between `0..15`.

The number of cell types can be adjusted in `lv_conf.h` by `LV_TABLE_CELL_STYLE_CNT`. By default it's 4. The default 4 cell types' part be referenced with dedicated names too:
- `LV_TABLE_PART_CELL1`
- `LV_TABLE_PART_CELL2`
- `LV_TABLE_PART_CELL3`
- `LV_TABLE_PART_CELL4`

The cells also use all the typical background style properties. If there is a line break (`\n`) in a cell's content then a horizontal division line will drawn after the line break using the *line* style properties.

The style of texts in the cells are inherited from the cell parts or the background part.

## Usage

### Rows and Columns

To set number of rows and columns use `lv_table_set_row_cnt(table, row_cnt)` and `lv_table_set_col_cnt(table, col_cnt)`

### Width and Height

The width of the columns can be set with `lv_table_set_col_width(table, col_id, width)`. The overall width of the Table object will be set to the sum of columns widths.

The height is calculated automatically from the cell styles (font, padding etc) and the number of rows.

### Set cell value

The cells can store only texts so numbers needs to be converted to text before displaying them in a table.

`lv_table_set_cell_value(table, row, col, "Content")`. The text is saved by the table so it can be even a local variable.

Line break can be used in the text like `"Value\n60.3"`.

### Align

The text alignment in cells can be adjusted individually with `lv_table_set_cell_align(table, row, col, LV_LABEL_ALIGN_LEFT/CENTER/RIGHT)`.

### Cell type

You can use 4 different cell types. Each has its own style.

Cell types can be used to add different style for example to:
- table header
- first column
- highlight a cell
- etc

The type can be selected with `lv_table_set_cell_type(table, row, col, type)` `type` can be 1, 2, 3 or 4.

### Merge cells

Cells can be merged horizontally with `lv_table_set_cell_merge_right(table, col, row, true)`. To merge more adjacent cells apply this function for each cell.


### Crop text
By default, the texts are word-wrapped to fit into the width of the cell and the height of the cell is set automatically. 
To disable this and keep the text as it is enable `lv_table_set_cell_crop(table, row, col, true)`.

### Scroll
The make the Table scrollable place it on a [Page](/widgets/page)

## Events
Only the [Generic events](../overview/event.html#generic-events) are sent by the object type.

Learn more about [Events](/overview/event).

## Keys

No *Keys* are processed by the object type.

Learn more about [Keys](/overview/indev).

## Example

```eval_rst

.. include:: /lv_examples/src/lv_ex_widgets/lv_ex_table/index.rst

```

### MicroPython
No examples yet.

## API 

```eval_rst

.. doxygenfile:: lv_table.h
  :project: lvgl
        
```
