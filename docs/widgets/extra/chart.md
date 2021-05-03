```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/chart.md
```
# Chart (lv_chart)

## Overview


Charts are a basic object to visualize data points. 
They support *Line* charts (connect points with lines and/or draw points on them) and *Column* charts.

Charts also support division lines, 2 y axis, axis ticks, and texts on ticks.

## Parts and Styles
The Chart's main part is called `LV_CHART_PART_BG` and it uses all the typical background properties. 
The *text* style properties determine the style of the axis texts and the *line* properties determine ticks' style.
*Padding* values add some space on the sides thus it makes the *series area* smaller. Padding also can be used to make space for axis texts and ticks.

The background of the series is called `LV_CHART_PART_SERIES_BG` and it's placed on the main background. 
The division lines, and series data is drawn on this part. Besides the typical background style properties
the *line* style properties are used by the division lines. The *padding* values tells the space between the this part and the axis texts.  


The style of the series can be referenced by `LV_CHART_PART_SERIES`. In case of column type the following properties are used:
- *radius*: radius of the bars
- *padding_inner*: space between the columns of the same x coordinate

In case of Line type these properties are used:
- *line properties* to describe the lines
- *size* radius of the points
- *bg_opa*: the overall opacity of the area below the lines
- *bg_main_stop*: % of *bg_opa* at the top to create an alpha fade (0: transparent at the top, 255: *bg_opa* at the top)
- *bg_grad_stop*: %  of *bg_opa* at the bottom to create an alpha fade  (0: transparent at the bottom, 255: *bg_opa* at the top)
- *bg_drag_dir*: should be `LV_GRAD_DIR_VER` to allow alpha fading with *bg_main_stop* and *bg_grad_stop*

`LV_CHART_PART_CURSOR` refres to the cursors. Any number of cursor can be added and their appearence can be set by the line related style properties. The color of the cursors are set when the cursor is created and `line_color` fro mteh style is overwritten by this value.

## Usage

### Data series
You can add any number of series to the charts by `lv_chart_add_series(chart, color)`.
It allocates data for a `lv_chart_series_t` structure which contains the chosen `color` and an array for the data points if
not using an external array, if an external array is assigned any internal points associated with the series are deallocated
and the series points to the external array instead.

### Series' type
The following **data display types** exist:

- **LV_CHART_TYPE_NONE** - Do not display any data. It can be used to hide the series.
- **LV_CHART_TYPE_LINE** - Draw lines between the points.
- **LV_CHART_TYPE_COLUMN** - Draw columns.

You can specify the display type with `lv_chart_set_type(chart, LV_CHART_TYPE_...)`. The types can be 'OR'ed (like `LV_CHART_TYPE_LINE`).

### Modify the data
You have several options to set the data of series:
1. Set the values manually in the array like `ser1->points[3] = 7` and refresh the chart with `lv_chart_refresh(chart)`.
2. Use `lv_chart_set_point_id(chart, ser, value, id)` where id is the index of the point you wish to update.
3. Use the `lv_chart_set_next(chart, ser, value)`.
4. Initialize all points to a given value with: `lv_chart_init_points(chart, ser, value)`.
5. Set all points from an array with: `lv_chart_set_points(chart, ser, value_array)`.

Use `LV_CHART_POINT_DEF` as value to make the library skip drawing that point, column, or line segment.

### Override default start point for series
If you wish a plot to start from a point other than the default which is point[0] of the series, you can set an alternative
index with the function `lv_chart_set_x_start_point(chart, ser, id)` where id is the new index position to start plotting from.

### Set an external data source
You can make the chart series update from an external data source by assigning it with the function:
`lv_chart_set_ext_array(chart, ser, array, point_cnt )` where array is an external array of lv_coord_t with point_cnt elements.
Note: you should call `lv_chart_refresh(chart)` after the external data source has been updated, to update the chart.

### Get current chart information
There are four functions to get information about a chart:
1. `lv_chart_get_type(chart)` returns the current chart type.
2. `lv_chart_get_point_count(chart)` returns the current chart point count. 
3. `lv_chart_get_x_start_point(ser)` returns the current plotting index for the specified series.
4. `lv_chart_get_point_id(chart, ser, id)` returns the value of the data at a particular index(id) for the specified series.

### Update modes
`lv_chart_set_next` can behave in two ways depending on *update mode*:
- **LV_CHART_UPDATE_MODE_SHIFT** - Shift old data to the left and add the new one o the right.
- **LV_CHART_UPDATE_MODE_CIRCULAR** - Circularly add the new data (Like an ECG diagram).

The update mode can be changed with `lv_chart_set_update_mode(chart, LV_CHART_UPDATE_MODE_...)`.

### Number of points
The number of points in the series can be modified by `lv_chart_set_point_count(chart, point_num)`. The default value is 10.
Note: this also affects the number of points processed when an external buffer is assigned to a series. 

### Vertical range
You can specify the minimum and maximum values in y-direction with `lv_chart_set_range(chart, y_min, y_max)`. The value of the points will be scaled proportionally. The default range is: 0..100.

### Division lines
The number of horizontal and vertical division lines can be modified by `lv_chart_set_div_line_count(chart, hdiv_num, vdiv_num)`. The default settings are 3 horizontal and 5 vertical division lines.

### Tick marks and labels
Ticks and labels can be added to the axis.

`lv_chart_set_x_tick_text(chart, list_of_values, num_tick_marks, LV_CHART_AXIS_...)` set the ticks and texts on x axis.
`list_of_values` is a string with `'\n'` terminated text (expect the last) with text for the ticks. E.g. `const char * list_of_values = "first\nsec\nthird"`. `list_of_values` can be `NULL`.
If `list_of_values` is set then `num_tick_marks` tells the number of ticks between two labels.  If `list_of_values` is `NULL` then it specifies the total number of ticks.

*Major tick lines* are drawn where text is placed, and *minor tick lines* are drawn elsewhere. `lv_chart_set_x_tick_length(chart, major_tick_len, minor_tick_len)` sets the length of tick lines on the x-axis.

The same functions exists for the y axis too: `lv_chart_set_y_tick_text` and `lv_chart_set_y_tick_length`.

### Cursor

A cursor can be added with `lv_chart_cursor_t * c1 = lv_chart_add_cursor(chart, color, dir);`. The possible values of `dir`  `LV_CHART_CURSOR_NONE/RIGHT/UP/LEFT/DOWN` or their OR-ed values to tell in which direction(s) should the cursor be drawn.  

`lv_chart_set_cursor_point(chart, cursor, &point)` sets the position of the cursor. `point` is a pointer to an `lv_poin_t` variable. E.g. `lv_point_t point = {10, 20};`. The point is relative to the series area of the chart.

The `lv_coord_t p_index = lv_chart_get_nearest_index_from_coord(chart, x)` tells which point index is to the closest to a X coordinate (relative to the series area). It can be used to snap the cursor to a point for example when the chart is clicked.

`lv_chart_get_x_from_index(chart, series, id)` and `lv_chart_get_y_from_index(chart, series, id)` tells the X and Y coordinate of a given point. It's useful to place the cursor to given point.

The current series area can be retrieved with `lv_chart_get_series_area(chart, &area)` where `area` is a pointer to an `lv_area_t` variable to store the result. The area has absolute coordinates. 


## Events
Only the [Generic events](../overview/event.html#generic-events) are sent by the object type.

Learn more about [Events](/overview/event).

## Keys
No *Keys* are processed by the object type.

Learn more about [Keys](/overview/indev).

## Example

```eval_rst

.. include:: /lv_examples/src/lv_ex_widgets/lv_ex_chart/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_chart.h
  :project: lvgl

```
