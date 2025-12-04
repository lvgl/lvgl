.. _grid:

====
Grid
====



Overview
********

The Grid layout is a subset of `CSS Grid`_ layout.

It can arrange items (child Widgets) into a 2D "table" that has rows and columns
(tracks).  An item can span multiple columns or rows. The
track's size can be set in pixels, to the largest item
(:c:macro:`LV_GRID_CONTENT`), or to a fraction of the available free space
(i.e. `Free [FR] Units <fr units_>`_) to distribute free space proportionally.

To make a Widget a Grid container call :cpp:expr:`lv_obj_set_layout(widget, LV_LAYOUT_GRID)`.

Note that the Grid layout feature of LVGL needs to be globally enabled
with :c:macro:`LV_USE_GRID` in ``lv_conf.h``.



Terms
*****

-  **tracks**: rows or columns
-  **free (FR) units**: if a track's size is set in ``FR units`` it will grow
   to fill the remaining space in the parent Widget (container), in proportion with
   other tracks that have non-zero FR-unit values.
-  **gap**: the space between rows and columns or the items on a track



Simple Interface
****************

With the following functions you can cause any parent Widget to have Grid-layout behaviors.

.. note::

    As with Flex containers, the parent Widget must be a Grid container for these
    styles to take effect.  The functions below cause the parent Widget to become a
    Grid container if it is not already.


.. _grid_descriptors:

Grid descriptors
----------------

First you need to describe the size of rows and columns. It can be done
by declaring 2 arrays and the track sizes in them. The last element must
be :c:macro:`LV_GRID_TEMPLATE_LAST`.

For example:

.. code-block:: c

   static int32_t column_dsc[] = {100, 400, LV_GRID_TEMPLATE_LAST};   /* 2 columns with 100- and 400-px width */
   static int32_t row_dsc[] = {100, 100, 100, LV_GRID_TEMPLATE_LAST}; /* 3 100-px tall rows */

To set the descriptors on a parent use
:cpp:expr:`lv_obj_set_grid_dsc_array(widget, col_dsc, row_dsc)`.

Besides settings the sizes in pixels, you can use two special
values:

- :c:macro:`LV_GRID_CONTENT` sets size to fit the largest child on this track
- :cpp:expr:`LV_GRID_FR(X)` determines what portion of the remaining space
  should be used by this track. Larger values means larger space.

.. _grid_items:

Grid items
----------

By default, a Widget's children are not added to the grid. They need to be
added manually to a cell.

To do this call
:cpp:expr:`lv_obj_set_grid_cell(child, column_align, column_pos, column_span, row_align, row_pos, row_span)`.

``column_align`` and ``row_align`` determine how to align the child Widget
in its cell.  Possible values are:

- :cpp:enumerator:`LV_GRID_ALIGN_START`: means left when direction is horizontal and top when vertical (default)
- :cpp:enumerator:`LV_GRID_ALIGN_END`: means right when direction is horizontal and bottom when vertical
- :cpp:enumerator:`LV_GRID_ALIGN_CENTER`: simply center ``column_pos`` and ``row_pos``
  means the zero-based index of the cell in which the item should be placed.

``column_span`` and ``row_span`` means how many tracks should be occupied
from the start cell. Must be ``>= 1``.

.. _grid_align:

Grid align
----------

If there is some empty space, items (Widgets) in Grid tracks can be aligned in several ways:

- :cpp:enumerator:`LV_GRID_ALIGN_START`: means left when direction is horizontal and top when vertical. (default)
- :cpp:enumerator:`LV_GRID_ALIGN_END`: means right when direction is horizontal and bottom when vertical
- :cpp:enumerator:`LV_GRID_ALIGN_CENTER`: simply center
- :cpp:enumerator:`LV_GRID_ALIGN_SPACE_EVENLY`: items are distributed so that the spacing
  between any two items (and the space to the edges) is equal. Not applies to ``track_cross_place``.
- :cpp:enumerator:`LV_GRID_ALIGN_SPACE_AROUND`: items are
  evenly distributed in the track with equal space around them. Note that
  visually the spaces aren't equal, since all the items have equal space
  on both sides. This makes the space between items double the space
  between edge items and the container's edge. Does not apply to ``track_cross_place``.
- :cpp:enumerator:`LV_GRID_ALIGN_SPACE_BETWEEN`: items are
  evenly distributed in the track with first and last items next to container's edges.
  Does not apply to ``track_cross_place``.

To set the track's alignment use
:cpp:expr:`lv_obj_set_grid_align(widget, column_align, row_align)`.

.. _grid_subgrid:

Sub grid
--------

If you set the column and/or row grid descriptors of a widget to ``NULL`` it will use
the grid descriptor(s) from it's parent.

For example if you create a grid item that spans columns 2..6 columns and rows 1..3
of the grid, the grid item will occupy 5 columns and 4 rows with the corresponding
track size from the parent Grid container.

This way even if a wrapper item is used in the grid, it can be made "transparent"
from the grid's point of view.

Limitations:

- The sub-grid is resolved only to a depth of 1 level. That is, a grid can have a
  sub-grid child, but that sub-grid cannot have another sub-grid.

- ``LV_GRID_CONTENT`` tracks on the grid are not handled in the sub-grid, only in its
  own grid.

The sub-grid feature works the same as in CSS.  For further information, see
`CSS Subgrid`_.



.. _grid_style:

Style Interface
***************

All the Grid-related values are style properties under the hood so you
can use them as you would any other style property.

The following Grid-related style properties exist:

-  :cpp:enumerator:`GRID_COLUMN_DSC_ARRAY`
-  :cpp:enumerator:`GRID_ROW_DSC_ARRAY`
-  :cpp:enumerator:`GRID_COLUMN_ALIGN`
-  :cpp:enumerator:`GRID_ROW_ALIGN`
-  :cpp:enumerator:`GRID_CELL_X_ALIGN`
-  :cpp:enumerator:`GRID_CELL_COLUMN_POS`
-  :cpp:enumerator:`GRID_CELL_COLUMN_SPAN`
-  :cpp:enumerator:`GRID_CELL_Y_ALIGN`
-  :cpp:enumerator:`GRID_CELL_ROW_POS`
-  :cpp:enumerator:`GRID_CELL_ROW_SPAN`

.. _grid_padding:

Internal padding
----------------

To modify the minimum space Grid inserts between Widgets, the following
properties can be set on the Grid container style:

-  :cpp:func:`lv_style_set_pad_row` sets padding between rows.

-  :cpp:func:`lv_style_set_pad_column` sets padding between columns.



.. _grid_other:

Other features
**************

RTL
---

If the base direction of the container is set to :cpp:enumerator:`LV_BASE_DIR_RTL`,
the meaning of :cpp:enumerator:`LV_GRID_ALIGN_START` and :cpp:enumerator:`LV_GRID_ALIGN_END` is
swapped. I.e. ``START`` will mean right-most.

The columns will be placed from right to left.



.. admonition::  Further Reading

    - Learn more about `CSS Grid`_ layout.
    - Learn more about `CSS Subgrid`_ layout.



.. _grid_examples:

Examples
********

.. include:: /examples/layouts/grid/index.rst


..  Hyperlinks

.. _css grid:     https://css-tricks.com/snippets/css/complete-guide-grid/
.. _fr units:     https://css-tricks.com/introduction-fr-css-unit/
.. _css subgrid:  https://developer.mozilla.org/en-US/docs/Web/CSS/CSS_grid_layout/Subgrid



.. _grid_api:

API
***
