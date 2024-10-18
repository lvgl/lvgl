.. _lv_chart:

================
Chart (lv_chart)
================

Overview
********

Charts are a basic object to visualize data points. Currently *Line*
charts (connect points with lines and/or draw points on them) and *Bar*
charts are supported.

Charts can have: - division lines - 2 y axis - axis ticks and texts on
ticks - cursors - scrolling and zooming

.. _lv_chart_parts_and_styles:

Parts and Styles
****************

-  :cpp:enumerator:`LV_PART_MAIN` The background of the chart. Uses all the typical
   background and *line* (for the division lines) related style
   properties. *Padding* makes the series area smaller. For column
   charts ``pad_column`` sets the space between the columns of the
   adjacent indices.
-  :cpp:enumerator:`LV_PART_SCROLLBAR` The scrollbar used if the chart is zoomed. See
   the :ref:`Base object <lv_obj>`\ 's documentation for details.
-  :cpp:enumerator:`LV_PART_ITEMS` Refers to the line or bar series.

   -  Line chart: The *line* properties are used by the lines.
      ``width``, ``height``, ``bg_color`` and ``radius`` is used to set
      the appearance of points.
   -  Bar chart: The typical background properties are used to style the
      bars. ``pad_column`` sets the space between the columns on the
      same index.

-  :cpp:enumerator:`LV_PART_INDICATOR` Refers to the points on line and scatter chart
   (small circles or squares).
-  :cpp:enumerator:`LV_PART_CURSOR` *Line* properties are used to style the cursors.
   ``width``, ``height``, ``bg_color`` and ``radius`` are used to set
   the appearance of points.

.. _lv_chart_usage:

Usage
*****

Chart type
----------

The following data display types exist:

- :cpp:enumerator:`LV_CHART_TYPE_NONE`: Do not display any data. Can be used to hide the series.
- :cpp:enumerator:`LV_CHART_TYPE_LINE`: Draw lines between the data points and/or points (rectangles or circles) on the data points.
- :cpp:enumerator:`LV_CHART_TYPE_BAR`: Draw bars.
- :cpp:enumerator:`LV_CHART_TYPE_SCATTER`: X/Y chart drawing point's and lines between the points. .

You can specify the display type with
:cpp:expr:`lv_chart_set_type(chart, LV_CHART_TYPE_...)`.

Data series
-----------

You can add any number of series to the charts by
:cpp:expr:`lv_chart_add_series(chart, color, axis)`. This allocates an
:cpp:struct:`lv_chart_series_t` structure which contains the chosen ``color`` and
an array for the data points. ``axis`` can have the following values:

- :cpp:enumerator:`LV_CHART_AXIS_PRIMARY_Y`: Left axis
- :cpp:enumerator:`LV_CHART_AXIS_SECONDARY_Y`: Right axis
- :cpp:enumerator:`LV_CHART_AXIS_PRIMARY_X`: Bottom axis
- :cpp:enumerator:`LV_CHART_AXIS_SECONDARY_X`: Top axis

``axis`` tells which axis's range should be used to scale the values.

:cpp:expr:`lv_chart_set_ext_y_array(chart, ser, value_array)` makes the chart
use an external array for the given series. ``value_array`` should look
like this: ``int32_t * value_array[num_points]``. The array size
needs to be large enough to hold all the points of that series. The
array's pointer will be saved in the chart so it needs to be global,
static or dynamically allocated. Note: you should call
:cpp:expr:`lv_chart_refresh(chart)` after the external data source has been
updated to update the chart.

The value array of a series can be obtained with
:cpp:expr:`lv_chart_get_y_array(chart, ser)`, which can be used with
``ext_array`` or *normal array*\ s.

For :cpp:enumerator:`LV_CHART_TYPE_SCATTER` type
:cpp:expr:`lv_chart_set_ext_x_array(chart, ser, value_array)` and
:cpp:expr:`lv_chart_get_x_array(chart, ser)` can be used as well.

Modify the data
---------------

You have several options to set the data of series:

1. Set the values manually in the array like ``ser1->points[3] = 7`` and refresh the chart with :cpp:enumerator:`lv_chart_refresh(chart)`.
2. Use :cpp:expr:`lv_chart_set_value_by_id(chart, ser, id, value)` where ``id`` is the index of the point you wish to update.
3. Use the :cpp:expr:`lv_chart_set_next_value(chart, ser, value)`.
4. Initialize all points to a given value with :cpp:expr:`lv_chart_set_all_value(chart, ser, value)`.

Use :cpp:enumerator:`LV_CHART_POINT_NONE` as value to make the library skip drawing
that point, column, or line segment.

For :cpp:enumerator:`LV_CHART_TYPE_SCATTER` type
:cpp:expr:`lv_chart_set_value_by_id2(chart, ser, id, value)` and
:cpp:expr:`lv_chart_set_next_value2(chart, ser, x_value, y_value)` can be used
as well.

Update modes
------------

:cpp:func:`lv_chart_set_next_value` can behave in two ways depending on *update
mode*:

- :cpp:enumerator:`LV_CHART_UPDATE_MODE_SHIFT`: Shift old data to the left and add the new one to the right.
- :cpp:enumerator:`LV_CHART_UPDATE_MODE_CIRCULAR`: Add the new data in circular fashion, like an ECG diagram.

The update mode can be changed with
:cpp:expr:`lv_chart_set_update_mode(chart, LV_CHART_UPDATE_MODE_...)`.

Number of points
----------------

The number of points in the series can be modified by
:cpp:expr:`lv_chart_set_point_count(chart, point_num)`. The default value is 10.
Note: this also affects the number of points processed when an external
buffer is assigned to a series, so you need to be sure the external
array is large enough.

Handling large number of points
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

On line charts, if the number of points is greater than the pixels
horizontally, the Chart will draw only vertical lines to make the
drawing of large amount of data effective. If there are, let's say, 10
points to a pixel, LVGL searches the smallest and the largest value and
draws a vertical lines between them to ensure no peaks are missed.

Vertical range
--------------

You can specify the minimum and maximum values in y-direction with
:cpp:expr:`lv_chart_set_range(chart, axis, min, max)`. ``axis`` can be
:cpp:enumerator:`LV_CHART_AXIS_PRIMARY` (left axis) or :cpp:enumerator:`LV_CHART_AXIS_SECONDARY`
(right axis).

The value of the points will be scaled proportionally. The default range
is: 0..100.

Division lines
--------------

The number of horizontal and vertical division lines can be modified by
:cpp:expr:`lv_chart_set_div_line_count(chart, hdiv_num, vdiv_num)`. The default
settings are 3 horizontal and 5 vertical division lines. If there is a
visible border on a side and no padding on that side, the division line
would be drawn on top of the border and therefore it won't be drawn.

Override default start point for series
---------------------------------------

If you want a plot to start from a point other than the default which is
``point[0]`` of the series, you can set an alternative index with the
function :cpp:expr:`lv_chart_set_x_start_point(chart, ser, id)` where ``id`` is
the new index position to start plotting from.

Note that :cpp:enumerator:`LV_CHART_UPDATE_MODE_SHIFT` also changes the
``start_point``.

Tick marks and labels
---------------------

With the help of `Scale </widgets/scale>`__, vertical and horizontal scales can be added in a very flexible way.
See the example below to learn more.

Zoom
----

To zoom the chart all you need to is wrapping it into a parent container and set the chart's width or height
to larger value. This way the chart will be scrollable on its parent.

Cursor
------

A cursor can be added with ``lv_chart_cursor_t * c1 = lv_chart_add_cursor(chart, color, dir);``.
The possible values of ``dir`` ``LV_DIR_NONE/RIGHT/UP/LEFT/DOWN/HOR/VER/ALL`` or their OR-ed values to
tell in which direction(s) should the cursor be drawn.

:cpp:expr:`lv_chart_set_cursor_pos(chart, cursor, &point)` sets the position of
the cursor. ``pos`` is a pointer to an :cpp:struct:`lv_point_t` variable. E.g.
``lv_point_t point = {10, 20}``. If the chart is scrolled the cursor
will remain in the same place.

:cpp:expr:`lv_chart_get_point_pos_by_id(chart, series, id, &point_out)` gets the
coordinate of a given point. It's useful to place the cursor at a given
point.

:cpp:expr:`lv_chart_set_cursor_point(chart, cursor, series, point_id)` sticks
the cursor at a point. If the point's position changes (new value or
scrolling) the cursor will move with the point.

.. _lv_chart_events:

Events
******

-  :cpp:enumerator:`LV_EVENT_VALUE_CHANGED` Sent when a new point is clicked pressed.
   :cpp:expr:`lv_chart_get_pressed_point(chart)` returns the zero-based index of
   the pressed point.

See the events of the :ref:`Base object <lv_obj>` too.

Learn more about :ref:`events`.

.. _lv_chart_keys:

Keys
****

No *Keys* are processed by the object type.

Learn more about :ref:`indev_keys`.

.. _lv_chart_example:

Example
*******

.. include:: ../examples/widgets/chart/index.rst

.. _lv_chart_api:

API
***
