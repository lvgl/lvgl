# Create a chart
chart = lv.chart(lv.scr_act())
chart.set_size(200, 150)
chart.center()
chart.set_type(lv.chart.TYPE.LINE)   # Show lines and points too

# Add two data series
ser1 = chart.add_series(lv.palette_main(lv.PALETTE.RED), lv.chart.AXIS.PRIMARY_Y);
ser2 = chart.add_series(lv.palette_main(lv.PALETTE.GREEN), lv.chart.AXIS.SECONDARY_Y)
print(ser2)
# Set next points on ser1
chart.set_next_value(ser1,10)
chart.set_next_value(ser1,10)
chart.set_next_value(ser1,10)
chart.set_next_value(ser1,10)
chart.set_next_value(ser1,10)
chart.set_next_value(ser1,10)
chart.set_next_value(ser1,10)
chart.set_next_value(ser1,30)
chart.set_next_value(ser1,70)
chart.set_next_value(ser1,90)

# Directly set points on 'ser2'
ser2.y_points = [90, 70, 65, 65, 65, 65, 65, 65, 65, 65]
chart.refresh()      #  Required after direct set

