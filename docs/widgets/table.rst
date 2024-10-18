.. _lv_table:

================
Table (lv_table)
================

Overview
********

Tables, as usual, are built from rows, columns, and cells containing
texts.

The Table object is very lightweight because only the texts are stored.
No real objects are created for cells but they are just drawn on the
fly.

The Table is added to the default group (if it is set). Besides the
Table is an editable object to allow selecting a cell with encoder
navigation too.

.. _lv_table_parts_and_styles:

Parts and Styles
****************

-  :cpp:enumerator:`LV_PART_MAIN` The background of the table uses all the typical
   background style properties.
-  :cpp:enumerator:`LV_PART_ITEMS` The cells of the table also use all the typical
   background style properties and the text properties.

.. _lv_table_usage:

Usage
*****

Set cell value
--------------

The cells can store only text so numbers need to be converted to text
before displaying them in a table.

:cpp:expr:`lv_table_set_cell_value(table, row, col, "Content")`. The text is
saved by the table so it can be even a local variable.

Line breaks can be used in the text like ``"Value\n60.3"``.

New rows and columns are automatically added is required

Rows and Columns
----------------

To explicitly set number of rows and columns use
:cpp:expr:`lv_table_set_row_count(table, row_cnt)` and
:cpp:expr:`lv_table_set_column_count(table, col_cnt)`

Width and Height
----------------

The width of the columns can be set with
:cpp:expr:`lv_table_set_column_width(table, col_id, width)`. The overall width of
the Table object will be set to the sum of columns widths.

The height is calculated automatically from the cell styles (font,
padding etc) and the number of rows.

Merge cells
-----------

Cells can be merged horizontally with
:cpp:expr:`lv_table_add_cell_ctrl(table, row, col, LV_TABLE_CELL_CTRL_MERGE_RIGHT)`.
To merge more adjacent cells call this function for each cell.

Scroll
------

If the label's width or height is set to :c:macro:`LV_SIZE_CONTENT` that size
will be used to show the whole table in the respective direction. E.g.
:cpp:expr:`lv_obj_set_size(table, LV_SIZE_CONTENT, LV_SIZE_CONTENT)`
automatically sets the table size to show all the columns and rows.

If the width or height is set to a smaller number than the "intrinsic"
size then the table becomes scrollable.

.. _lv_table_events:

Events
******

-  :cpp:enumerator:`LV_EVENT_VALUE_CHANGED` Sent when a new cell is selected with
   keys.

See the events of the :ref:`Base object <lv_obj>` too.

Learn more about :ref:`events`.

.. _lv_table_keys:

Keys
****

The following *Keys* are processed by the Tables: -
``LV_KEY_RIGHT/LEFT/UP/DOWN/`` Select a cell.

Note that, as usual, the state of :cpp:enumerator:`LV_KEY_ENTER` is translated to
``LV_EVENT_PRESSED/PRESSING/RELEASED`` etc.

:cpp:expr:`lv_table_get_selected_cell(table, &row, &col)` can be used to get the
currently selected cell. Row and column will be set to
:c:macro:`LV_TABLE_CELL_NONE` no cell is selected.

Learn more about :ref:`indev_keys`.

.. _lv_table_example:

Example
*******

.. include:: ../examples/widgets/table/index.rst

MicroPython
-----------

No examples yet.

.. _lv_table_api:

API
***
