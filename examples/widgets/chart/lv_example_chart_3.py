def draw_event_cb(e):

    dsc = lv.obj_draw_part_dsc_t.__cast__(e.get_param())
    if dsc.part == lv.PART.TICKS and dsc.id == lv.chart.AXIS.PRIMARY_X: 
        month = ["Jan", "Febr", "March", "Apr", "May", "Jun", "July", "Aug", "Sept", "Oct", "Nov", "Dec"]
        # dsc.text is defined char text[16], I must therefore convert the Python string to a byte_array
        dsc.text = bytes(month[dsc.value],"ascii") 
#
# Add ticks and labels to the axis and demonstrate scrolling
#

# Create a chart
chart = lv.chart(lv.scr_act())
chart.set_size(200, 150)
chart.center()
chart.set_type(lv.chart.TYPE.BAR)
chart.set_range(lv.chart.AXIS.PRIMARY_Y, 0, 100)
chart.set_range(lv.chart.AXIS.SECONDARY_Y, 0, 400)
chart.set_point_count(12)
chart.add_event_cb(draw_event_cb, lv.EVENT.DRAW_PART_BEGIN, None)

# Add ticks and label to every axis
chart.set_axis_tick(lv.chart.AXIS.PRIMARY_X, 10, 5, 12, 3, True, 40)
chart.set_axis_tick(lv.chart.AXIS.PRIMARY_Y, 10, 5, 6, 2, True, 50)
chart.set_axis_tick(lv.chart.AXIS.SECONDARY_Y, 10, 5, 3, 4,True, 50)

# Zoom in a little in X
chart.set_zoom_x(800)

# Add two data series
ser1 = lv.chart.add_series(chart, lv.palette_lighten(lv.PALETTE.GREEN, 2), lv.chart.AXIS.PRIMARY_Y)
ser2 = lv.chart.add_series(chart, lv.palette_darken(lv.PALETTE.GREEN, 2), lv.chart.AXIS.SECONDARY_Y)

# Set the next points on 'ser1'
chart.set_next_value(ser1, 31)
chart.set_next_value(ser1, 66)
chart.set_next_value(ser1, 10)
chart.set_next_value(ser1, 89)
chart.set_next_value(ser1, 63)
chart.set_next_value(ser1, 56)
chart.set_next_value(ser1, 32)
chart.set_next_value(ser1, 35)
chart.set_next_value(ser1, 57)
chart.set_next_value(ser1, 85)
chart.set_next_value(ser1, 22)
chart.set_next_value(ser1, 58)

# Directly set points on 'ser2'
ser2.y_points = [92,71,61,15,21,35,35,58,31,53,33,73]

chart.refresh()  # Required after direct set

