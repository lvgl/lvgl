import display_driver
import lvgl as lv

def add_data(t):
    chart.set_next_value(ser, lv.rand(10, 90))

    p = chart.get_point_count()
    s = chart.get_x_start_point(ser)
    a = chart.get_y_array(ser)

    a[(s + 1) % p] = lv.CHART_POINT.NONE
    a[(s + 2) % p] = lv.CHART_POINT.NONE
    a[(s + 3) % p] = lv.CHART_POINT.NONE
    chart.refresh()

#
# Circular line chart with gap
#
chart = lv.chart(lv.scr_act())

chart.set_update_mode(lv.chart.UPDATE_MODE.CIRCULAR)
chart.set_size(200, 150)
chart.center()

chart.set_point_count(30)
ser = chart.add_series(lv.palette_main(lv.PALETTE.RED), lv.chart.AXIS.PRIMARY_Y)
#Prefill with data
for i in range(0, 30):
    chart.set_next_value(ser, lv.rand(10, 90))

lv.timer_create(add_data, 200, None)
