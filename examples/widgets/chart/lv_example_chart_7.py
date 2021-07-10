#!/opt/bin/lv_micropython -i
import utime as time
import lvgl as lv
import display_driver

def draw_event_cb(e):
    dsc = e.get_draw_part_dsc()
    if dsc.part == lv.PART.ITEMS:
        obj = e.get_target()
        ser = obj.get_series_next(None)
        cnt = obj.get_point_count()
        # print("cnt: ",cnt)
        # Make older value more transparent
        dsc.rect_dsc.bg_opa = (lv.OPA.COVER *  dsc.id) // (cnt - 1)
        
        # Make smaller values blue, higher values red
        # x_array = chart.get_x_array(ser)
        # y_array = chart.get_y_array(ser)
        # dsc->id is the tells drawing order, but we need the ID of the point being drawn.
        start_point = chart.get_x_start_point(ser)
        # print("start point: ",start_point)
        p_act = (start_point + dsc.id) % cnt # Consider start point to get the index of the array
        # print("p_act", p_act)
        x_opa = (x_array[p_act] * lv.OPA._50) // 200
        y_opa = (y_array[p_act] * lv.OPA._50) // 1000
        
        dsc.rect_dsc.bg_color = lv.palette_main(lv.PALETTE.RED).color_mix(
                                             lv.palette_main(lv.PALETTE.BLUE),
                                             x_opa + y_opa)
        
def add_data(timer,chart):
    # print("add_data")
    x = lv.rand(0,200)
    y = lv.rand(0,1000)
    chart.set_next_value2(ser, x, y)
    # chart.set_next_value2(chart.gx, y)
    x_array.pop(0)
    x_array.append(x)
    y_array.pop(0)
    y_array.append(y)
    
#
# A scatter chart
#

chart = lv.chart(lv.scr_act())
chart.set_size(200, 150)
chart.align(lv.ALIGN.CENTER, 0, 0)
chart.add_event_cb(draw_event_cb, lv.EVENT.DRAW_PART_BEGIN, None)
chart.set_style_line_width(0, lv.PART.ITEMS)   # Remove the lines

chart.set_type(lv.chart.TYPE.SCATTER)

chart.set_axis_tick(lv.chart.AXIS.PRIMARY_X, 5, 5, 5, 1, True, 30)
chart.set_axis_tick(lv.chart.AXIS.PRIMARY_Y, 10, 5, 6, 5, True, 50)

chart.set_range(lv.chart.AXIS.PRIMARY_X, 0, 200)
chart.set_range(lv.chart.AXIS.PRIMARY_Y, 0, 1000)

chart.set_point_count(50)

ser = chart.add_series(lv.palette_main(lv.PALETTE.RED), lv.chart.AXIS.PRIMARY_Y)

x_array = []
y_array = []
for i in range(50):
    x_array.append(lv.rand(0, 200))
    y_array.append(lv.rand(0, 1000))
                        
ser.x_points = x_array
ser.y_points = y_array

# Create an `lv_timer` to update the chart.

timer = lv.timer_create_basic()
timer.set_period(100)
timer.set_cb(lambda src: add_data(timer,chart))
