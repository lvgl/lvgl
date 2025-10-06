.. _lv_table:

================
Table (lv_table)
================


Overview
********

Tables are built from rows, columns, and cells containing text.

The Table Widget is very lightweight because only the text strings are stored.
No real Widgets are created for cells --- they are just drawn on the fly.

The Table is added to the default group (if one is set).
Table is an editable Widget, allowing selection of a cell with encoder and keyboard
navigation as well.



.. _lv_table_parts_and_styles:

Parts and Styles
****************

-  :cpp:enumerator:`LV_PART_MAIN` The background of the Table; uses the :ref:`typical
   background style properties <typical bg props>`.
-  :cpp:enumerator:`LV_PART_ITEMS` The cells of the Table also use the
   :ref:`typical background style properties <typical bg props>` as well as text
   style properties.



.. _lv_table_usage:

Usage
*****

Set cell value
--------------

Cells can store only text so numbers need to be converted to text
before displaying them in a Table.

:cpp:expr:`lv_table_set_cell_value(table, row, col, "Content")`. The text is
saved by the Table so the buffer containing the string can be a local variable.

Line breaks can be used in the text like ``"Value\n60.3"``.

New rows and columns are automatically added as required.

Rows and Columns
----------------

To explicitly set number of rows and columns use
:cpp:expr:`lv_table_set_row_count(table, row_cnt)` and
:cpp:expr:`lv_table_set_column_count(table, col_cnt)`.

Width and Height
----------------

Column width can be set with
:cpp:expr:`lv_table_set_column_width(table, col_id, width)`. The overall width of
the Table Widget will be set to the sum of all column widths.

Height is calculated automatically from the cell styles (font,
padding etc) and the number of rows.

Merge cells
-----------

Cells can be merged horizontally with
:cpp:expr:`lv_table_set_cell_ctrl(table, row, col, LV_TABLE_CELL_CTRL_MERGE_RIGHT)`.
To merge more adjacent cells, call this function for each cell.

Scrolling
---------

If a Table's width or height is set to :c:macro:`LV_SIZE_CONTENT` that size
will be used to show the whole Table in the respective direction. E.g.
:cpp:expr:`lv_obj_set_size(table, LV_SIZE_CONTENT, LV_SIZE_CONTENT)`
automatically sets the Table size to show all columns and rows.

If the width or height is set to a smaller number than its "intrinsic"
size then the Table becomes scrollable.


Set cell user data
------------------

Custom data can be bound to a table cell. The data lifetime must be managed by the user. If the data is dynamically allocated,
the user must free it to prevent memory leaks. This can be done by binding to the :cpp:enumerator:`LV_EVENT_DELETE` event.

.. code-block:: c 

    static void table_delete_event_cb(lv_event_t * e) {
        lv_obj_t * my_table = lv_event_get_target_obj(e);
        const uint32_t row_count = lv_table_get_row_count(table);
        const uint32_t col_count = lv_table_get_column_count(table);

        /* Assuming every cell has custom data associated with it*/
        for(uint32_t i = 0; i < row_count; ++i) {
            for(uint32_t j = 0; j < col_count; ++j) {
                void * cell_user_data = lv_table_get_cell_user_data(table, i, j);
                lv_free(cell_user_data);
            }
        }
    }

    lv_obj_add_event_cb(table, table_delete_event_cb, LV_EVENT_DELETE, NULL);

.. _lv_table_events:

Events
******

-  :cpp:enumerator:`LV_EVENT_VALUE_CHANGED` Sent when a new cell is selected with
   keys.

.. admonition::  Further Reading

    Learn more about :ref:`lv_obj_events` emitted by all Widgets.

    Learn more about :ref:`events`.



.. _lv_table_keys:

Keys
****

The following *Keys* are processed by Table Widgets:

- ``LV_KEY_RIGHT/LEFT/UP/DOWN/`` Select a cell.

Note that, as usual, the state of :cpp:enumerator:`LV_KEY_ENTER` is translated to
``LV_EVENT_PRESSED/PRESSING/RELEASED`` etc.

:cpp:expr:`lv_table_get_selected_cell(table, &row, &col)` can be used to get the
currently selected cell. Row and column will be set to
:c:macro:`LV_TABLE_CELL_NONE` if no cell is selected.

.. admonition::  Further Reading

    Learn more about :ref:`indev_keys`.



.. _lv_table_example:

Example
*******

.. include:: ../../examples/widgets/table/index.rst

MicroPython
-----------

No examples yet.



.. _lv_table_api:

API
***
