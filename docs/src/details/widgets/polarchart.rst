.. _lv_polarchart:

==========================
Polarchart (lv_polarchart)
==========================


Overview
********

Polarchart Widgets are used to visualize data.

Polarcharts have:

- 0 or more background circular division circles,
- 0 or more background radial division lines,
- 2 internal axes (angle and radial), that must be used to specify scale for a data series,
- a ``point_count`` (default 10) that applies to all data series added,
- 0 or more data series (you can add or remove them at any time),
- 0 or more cursors (you can add or remove them at any time),
- update mode (modifies behavior of :cpp:func:`lv_polarchart_set_next_value` if you use it).

Polarcharts can:

- display flexible axes with ticks and text
- show precise locations of points or other locations on polarchart with cursors
- show or hide individual data series
- show or hide individual data points
- show or hide cursors
- scroll and zoom



Polarchart Basics
*****************

Each polarchart has the following attributes (over and above attributes found in
:ref:`all Widgets <base_widget>`):

Type (governs how a polarchart's data series are drawn)

- Can be LINE (default), SCATTER, or NONE.
- You can change the polarchart's type at any point during its life.

Circular and Radial division lines

- default 3 and 4 respectively
- can be any non-negative value including 0

1 Radial axis and 1 Angle axis (the latter is used with SCATTER polarcharts)

- All 2 axes come with each polarchart automatically (they do not have to be created).
- Their default ranges are [0..100] and [0..360) respectively.
  If you need a different range, set it before polarchart is drawn.
- You "use" an axis by associating it with a data series, which happens when the
  data series is created (more on this below).  More than one data series can be
  associated with each axis.

Point count (number of data points in all data series added to the polarchart)

- default 10
- If you provide your own data-value arrays, each array so provided must contain
  at least this number of values.
- For LINE-charts, this is the number of points on the 'angle' axis.
- LINE-charts require only one data-value array to supply Radial-values for each data point.
- For SCATTER polarcharts, this is the number of scatter-points in the data series.
- SCATTER polarcharts have separate data-value arrays for both Angle-values and Radial-values.

Any number of data series

- After a polarchart is created, it initially contains no data series.  You have to add them.
- You can add and remove data series at any time during a polarchart's life.
- When a data series is created, it comes with pre-allocated values array(s)
  based on its polarchart type and ``point_count``.  (All polarchart types use an array of
  Radial-values.  SCATTER-type polarcharts also use an array of Angle-values.).  All Radial-values so
  allocated are set to :c:macro:`LV_POLARCHART_POINT_NONE`, which causes that point to be hidden.
- To get points to be drawn on the polarchart, you must set their Radial-values to something
  other than :c:macro:`LV_POLARCHART_POINT_NONE`.
- You can hide a point by setting its Radial-value to :c:macro:`LV_POLARCHART_POINT_NONE`.
- If desired, you can tell a data series to instead use a value array you
  provide.  If you do:

  - Pre-allocated value arrays are automatically freed.
  - That data series will continue to use *your* array from that time onward.
  - The values in your array must remain available through the life of that data series.
  - You must ensure each array provided contains at least ``point_count`` ``int32_t`` elements.
  - Management of the life any value arrays you provide is up to you.

Any number of cursors

- After a polarchart is created, it initially contains no cursors.  You have to add them
  if you want to use them.
- You can add, show, hide or remove cursors at any time during a polarchart's life.

Update mode

- :ref:`See below <polarchart_update_modes>`


Polarchart layers
-----------------
When a polarchart is drawn, certain things appear on top of other things, in this
order, from back to front:

- The polarchart's background (with optional division lines)
- Each data series:

  - Earliest data series added appears on top.
  - For a SCATTER polarchart, within each series, points later in the sequence will appear
    on top of points earlier in the sequence when there is overlap.

- Each cursor (if there are any):

  - The most recent cursor added appears on top.



.. _lv_polarchart_parts_and_styles:

Parts and Styles
****************

-  :cpp:enumerator:`LV_PART_MAIN` The background of the polarchart. Uses the :ref:`typical
   background <typical bg props>` and line style properties (for division lines).
   *Padding* makes the series area smaller.
-  :cpp:enumerator:`LV_PART_SCROLLBAR` A scrollbar used if the polarchart is zoomed. See
   :ref:`base_widget`'s documentation for details.
-  :cpp:enumerator:`LV_PART_ITEMS` Refers to the LINE or BAR data series.

   -  LINE polarchart: *line* properties are used by lines.
      ``width``, ``height``, ``bg_color`` and ``radius`` are used to set
      the appearance of points on the line.
   -  Bar polarchart: The typical background properties are used to style the
      bars. ``pad_column`` sets the space between columns in the same data series.

-  :cpp:enumerator:`LV_PART_INDICATOR` Refers to points on LINE- and SCATTER-charts
   (small circles or squares [with possibly-rounded corners]).
-  :cpp:enumerator:`LV_PART_CURSOR` *Line* properties are used to style cursors.
   ``width``, ``height``, ``bg_color`` and ``radius`` are used to set
   the appearance of the cursor's "point" showing its location.  If either ``width``
   or ``height`` are set to 0, only the cursor's lines are drawn.



.. _lv_polarchart_details:

Details
*******

Polarchart type
---------------

A polarchart can be one of the following types:

- :cpp:enumerator:`LV_POLARCHART_TYPE_NONE`: Do not display any data. Can be used to hide polarchart's data.
- :cpp:enumerator:`LV_POLARCHART_TYPE_LINE`: Draw lines between data points.  Data points
  can also be illustrated if their ``width``, ``height``, ``bg_color`` and ``radius``
  styles (for :cpp:enumerator:`LV_PART_ITEMS`) are set and both ``width`` and
  ``height`` have non-zero values.
- :cpp:enumerator:`LV_POLARCHART_TYPE_SCATTER`: Angle/Radial polarchart drawing point's and optionally
  lines between the points if line-width style values for
  :cpp:enumerator:`LV_PART_ITEMS` is a non-zero value, and the point's Radial-value is
  something other than :c:macro:`LV_POLARCHART_POINT_NONE`.  (Drawing of individual points on a
  SCATTER polarchart can be suppressed if their Radial-values are set to :c:macro:`LV_POLARCHART_POINT_NONE`.)

Polarcharts start their life as LINE polarcharts.  You can change a polarchart's type with
:cpp:expr:`lv_polarchart_set_type(chart, LV_POLARCHART_TYPE_...)`.

Data series
-----------

You can add any number of data series to a polarchart by using

    :cpp:expr:`lv_polarchart_add_series(chart, color, axis)`.

This allocates (and returns a pointer to) an :cpp:expr:`lv_polarchart_series_t` structure
which remembers the ``color`` and ``axis`` you specified, and comes pre-allocated
with an array of ``chart->point_cnt`` ``int32_t`` Radial-values, all set to
:c:macro:`LV_POLARCHART_POINT_NONE`. (A SCATTER polarchart also comes with a pre-allocated array of
the same number of Angle-values.)

``axis`` specifies which axis is used to scale its values, and may be one of the following:

- :cpp:enumerator:`LV_POLARCHART_AXIS_RADIAL`: Radial axis
- :cpp:enumerator:`LV_POLARCHART_AXIS_ANGLE`: Angle axis

When adding a data series to a SCATTER polarchart, bit-wise OR the Radial axis
with the Angle-axis values.

If you wish to have the polarchart use your own Radial-value array instead of the one provided,
you can do so with

    :cpp:expr:`lv_polarchart_set_series_ext_radial_array(chart, series, value_array)`.

You are responsible for ensuring the array you provide contains at least
``chart->point_cnt`` elements in it.

``value_array`` should look like this: ``int32_t * value_array[num_points]``.  Only
the array's pointer is saved in the series so its contents need to remain available
for the life of the series, i.e. the array needs to be global, static or dynamically
allocated.

.. note::
    Call :cpp:expr:`lv_polarchart_refresh(chart)` when a polarchart's data has changed to
    signal that the polarchart should be re-rendered next time a display refresh occurs.
    You do not need to do this if you are using the provided value array(s) and
    setting values with ``lv_polarchart_set_...value_...()`` functions.  See below
    for more information about these functions.

A pointer to the Radial-value array of a series can be obtained with
:cpp:expr:`lv_polarchart_get_series_radial_array(chart, series)`.  This is true whether you are using
the provided Radial-value array or provided your own.

For SCATTER-type polarcharts,

- :cpp:expr:`lv_polarchart_set_series_ext_angle_array(chart, series, value_array)` and
- :cpp:expr:`lv_polarchart_get_series_angle_array(chart, series)`

can be used as well.

Modifying data
--------------

You have several options to set the Radial-values for a data series:

1. Set the values programmatically in the array like ``ser1->points[3] = 7`` and refresh the
   polarchart with :cpp:expr:`lv_polarchart_refresh(chart)`.
2. Use :cpp:expr:`lv_polarchart_set_series_value_by_id(chart, series, id, value)` where ``id`` is
   the zero-based index of the point you wish to update.
3. Use :cpp:expr:`lv_polarchart_set_next_value(chart, series, value)`.
   (See `Update modes`_ below.)
4. Set all points to a single Radial-Value with :cpp:expr:`lv_polarchart_set_all_values(chart, series, value)`.

Use :c:macro:`LV_POLARCHART_POINT_NONE` as value to make the library skip drawing
that point, column, or scatter-point.

For SCATTER-type polarcharts,

- :cpp:expr:`lv_polarchart_set_series_value_by_id2(chart, series, id, x_value, y_value)` and
- :cpp:expr:`lv_polarchart_set_next_value2(chart, series, x_value, y_value)`

can be used as well.

.. _polarchart_update_modes:

Update modes
------------

:cpp:func:`lv_polarchart_set_next_value` can behave in two ways depending on *update
mode*:

- :cpp:enumerator:`LV_POLARCHART_UPDATE_MODE_SHIFT`: Shift old data to the left and add the new one to the right.
- :cpp:enumerator:`LV_POLARCHART_UPDATE_MODE_CIRCULAR`: Add the new data in circular fashion, like an ECG diagram.

The update mode can be changed with
:cpp:expr:`lv_polarchart_set_update_mode(chart, LV_POLARCHART_UPDATE_MODE_...)`.

Number of points
----------------

The number of points in the series can be modified by
:cpp:expr:`lv_polarchart_set_point_count(chart, point_num)`. The default value is 10.
Note: this affects the number of points processed when an external
value array is assigned to a series, so you also need to be sure any external
array so provided contains at least ``point_num`` elements.

Handling large numbers of points
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

????

Vertical range
--------------

You can specify the minimum and maximum values in Radial-direction with
:cpp:expr:`lv_polarchart_set_axis_range(chart, axis, min, max)`. ``axis`` can be
:cpp:enumerator:`LV_POLARCHART_AXIS_RADIAL`

The value of the points will be scaled proportionally. The default range
is: 0..100.

Division lines
--------------

The number of radial and circular division lines can be modified by
:cpp:expr:`lv_polarchart_set_div_line_count(chart, angle_div_num, radial_div_num)`. The default
settings are 4 radial and 5 circular division lines.

Override default start point for series
---------------------------------------

If you want a plot to start from a point other than the default which is
``point[0]`` of the series, you can set an alternative index with the
function :cpp:expr:`lv_polarchart_set_angle_start_point(chart, series, id)` where ``id`` is
the new zero-based index position to start plotting from.

Note that :cpp:enumerator:`LV_POLARCHART_UPDATE_MODE_SHIFT` also changes the
``start_point``.

Tick marks and labels
---------------------

With the help of :ref:`Scale <lv_scale>`, radial and angular scales can be added
in a very flexible way.  See the `examples 2 <#axis-ticks-and-labels-with-scrolling>`_
below to learn more.

Zoom
----

To zoom the polarchart all you need to do is wrap it in a parent container and set the
chart's width and/or height to a larger value.  Doing this will cause the polarchart
to be scrollable in its parent --- the parent container provides the scrollable "view
window".

Cursor
------

A new cursor is initially given position :c:macro:`LV_POLARCHART_POINT_NONE` which causes
it to be hidden.  To show the cursor, its location must be set by you
programmatically using one of the functions below.

You can hide a cursor without removing it from the polarchart by using
:cpp:func:`lv_polarchart_set_cursor_point` by passing :c:macro:`LV_POLARCHART_POINT_NONE` as
the point id.

A cursor can be added with ``lv_polarchart_cursor_t * c1 = lv_polarchart_add_cursor(chart, color, dir);``.
The possible values of ``dir`` are the enumeration values of :cpp:type:`lv_dir_t`:
``LV_DIR_NONE/RIGHT/UP/LEFT/DOWN/HOR/VER/ALL`` or their bit-wise OR-ed values to tell
the polarchart which direction(s) to draw its lines.

:cpp:expr:`lv_polarchart_set_cursor_pos(chart, cursor, &point)` sets the position of
the cursor to an arbitrary point on the polarchart. ``&point`` is a pointer to an
:cpp:struct:`lv_point_t` variable. E.g. ``lv_point_t point = {10, 20}``. If the polarchart
is scrolled, the cursor moves with it.

:cpp:expr:`lv_polarchart_get_point_pos_by_id(chart, series, id, &point_out)` gets the
coordinate of a given point on the polarchart.  This is useful to place the cursor on
that data point.

:cpp:expr:`lv_polarchart_set_cursor_point(chart, cursor, series, point_id)` places the
cursor on the specified data point on the polarchart.  If the point's position changes
(via a new value or via scrolling), the cursor moves with the point.
See an example of using this function `here <#show-cursor-on-the-clicked-point>`_.



.. _lv_polarchart_events:

Events
******

-  :cpp:enumerator:`LV_EVENT_VALUE_CHANGED` Sent when a new point on the polarchart is pressed.
   :cpp:expr:`lv_polarchart_get_pressed_point(chart)` returns the zero-based index of
   the pressed point.

.. admonition::  Further Reading

    Learn more about :ref:`lv_obj_events` emitted by all Widgets.

    Learn more about :ref:`events`.



.. _lv_polarchart_keys:

Keys
****

No *Keys* are processed by Chart Widgets.

.. admonition::  Further Reading

    Learn more about :ref:`indev_keys`.



.. _lv_polarchart_example:

Example
*******

.. include:: ../../examples/widgets/polarchart/index.rst



.. _lv_polarchart_api:

API
***
