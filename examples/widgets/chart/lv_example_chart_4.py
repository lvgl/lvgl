#!/opt/bin/lv_micropython -i
import lvgl as lv
import display_driver

def event_cb(e):

    code = e.get_code()
    chart = e.get_target()

    if code == lv.EVENT.VALUE_CHANGED:
        chart.invalidate()
    
    if code == lv.EVENT.REFR_EXT_DRAW_SIZE:
        # s = lv.coord_t.__cast__(e.get_param())
        # print("s: {:d}".format(s))
        e.set_ext_draw_size(20)
    
    elif code == lv.EVENT.DRAW_POST_END:
        id = chart.get_pressed_point()
        if id == lv.CHART_POINT_NONE :
            return

        # print("Selected point {:d}".format(id))

        ser = chart.get_series_next(None)
        
        while(ser) :
            p = lv.point_t() 
            chart.get_point_pos_by_id(ser, id, p)
            # print("point coords: x: {:d}, y: {:d}".format(p.x,p.y))
            y_array = chart.get_y_array(ser)
            value = y_array[id]

            buf = lv.SYMBOL.DUMMY + "{:2d}".format(value)
            
            draw_rect_dsc = lv.draw_rect_dsc_t()
            draw_rect_dsc.init()
            draw_rect_dsc.bg_color = lv.color_black()
            draw_rect_dsc.bg_opa = lv.OPA._50
            draw_rect_dsc.radius = 3
            draw_rect_dsc.bg_img_src = buf
            draw_rect_dsc.bg_img_recolor = lv.color_white()
            
            coords = lv.area_t()
            chart.get_coords(coords)
	    # print("coords: x1: {:d}, y1: {:d}".format(coords.x1, coords.y1))
            a = lv.area_t()
            a.x1 = coords.x1 + p.x - 20
            a.x2 = coords.x1 + p.x + 20
            a.y1 = coords.y1 + p.y - 30
            a.y2 = coords.y1 + p.y - 10
            # print("a: x1: {:d}, x2: {:d}, y1: {:d}, y2: {:d}".format(a.x1,a.x2,a.y1,a.y2))
            draw_ctx = e.get_draw_ctx()
            draw_ctx.rect(draw_rect_dsc, a)
           
            ser = chart.get_series_next(ser)

    elif code == lv.EVENT.RELEASED:
        chart.invalidate()


# 
#  Show the value of the pressed points
#

# Create a chart
chart = lv.chart(lv.scr_act())
chart.set_size(200, 150)
chart.center()

chart.add_event_cb(event_cb, lv.EVENT.ALL, None)

chart.refresh_ext_draw_size()

# Zoom in a little in X
chart.set_zoom_x(800)

# Add two data series
ser1 = chart.add_series(lv.palette_main(lv.PALETTE.RED), lv.chart.AXIS.PRIMARY_Y)
ser2 = chart.add_series(lv.palette_main(lv.PALETTE.GREEN), lv.chart.AXIS.PRIMARY_Y)
for i in range(10):
    chart.set_next_value(ser1, lv.rand(60, 90))
    chart.set_next_value(ser2, lv.rand(10, 40))

