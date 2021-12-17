def event_cb(e):
    code = e.get_code()
    chart = e.get_target()

    if code == lv.EVENT.VALUE_CHANGED:
        chart.invalidate()

    if code == lv.EVENT.REFR_EXT_DRAW_SIZE:
        e.set_ext_draw_size(20)

    elif code == lv.EVENT.DRAW_POST_END:
        id = lv.chart.get_pressed_point(chart)
        if id == lv.CHART_POINT.NONE:
            return
        # print("Selected point ", id)
        for i in range(len(series)):
            p = lv.point_t()
            chart.get_point_pos_by_id(series[i], id, p)
            value = series_points[i][id]
            buf = lv.SYMBOL.DUMMY + "$" + str(value)
            
            draw_rect_dsc = lv.draw_rect_dsc_t()
            draw_rect_dsc.init()
            draw_rect_dsc.bg_color = lv.color_black()
            draw_rect_dsc.bg_opa = lv.OPA._50
            draw_rect_dsc.radius = 3
            draw_rect_dsc.bg_img_src = buf
            draw_rect_dsc.bg_img_recolor = lv.color_white()
            
            a = lv.area_t()
            coords = lv.area_t()
            chart.get_coords(coords)
            a.x1 = coords.x1 + p.x - 20
            a.x2 = coords.x1 + p.x + 20
            a.y1 = coords.y1 + p.y - 30
            a.y2 = coords.y1 + p.y - 10
            
            clip_area = lv.area_t.__cast__(e.get_param())
            lv.draw_rect(a, clip_area, draw_rect_dsc)
            
    elif code == lv.EVENT.RELEASED:
        chart.invalidate()
            
# 
# Add ticks and labels to the axis and demonstrate scrolling
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

ser1_p = []
ser2_p = []
for i in range(10):
    ser1_p.append(lv.rand(60,90))
    ser2_p.append(lv.rand(10,40))
ser1.y_points = ser1_p
ser2.y_points = ser2_p

series = [ser1,ser2]
series_points=[ser1_p,ser2_p]

