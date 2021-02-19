# Create a chart
chart = lv.chart(lv.scr_act())
chart.set_size(200, 150)
chart.align(None, lv.ALIGN.CENTER, 0, 0)
chart.set_type(lv.chart.TYPE.POINT | lv.chart.TYPE.LINE)   # Show lines and points too
chart.set_series_opa(lv.OPA._70)                           # Opacity of the data series
chart.set_series_width(4)                                  # Line width and point radious

chart.set_range(0, 100)

# Add two data series
ser1 = chart.add_series(lv.color_make(0xFF,0,0))
ser2 = chart.add_series(lv.color_make(0,0x80,0))

# Set points on 'dl1'
chart.set_points(ser1, [10, 10, 10, 10, 10, 10, 10, 30, 70, 90])

# Set points on 'dl2'
chart.set_points(ser2, [90, 70, 65, 65, 65, 65, 65, 65, 65, 65])