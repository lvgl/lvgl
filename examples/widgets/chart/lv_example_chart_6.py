class ExampleChart_6():
    
    def __init__(self):
        self.last_id = -1
        #
        # Show cursor on the clicked point
        #

        chart = lv.chart(lv.scr_act())
        chart.set_size(200, 150)
        chart.align(lv.ALIGN.CENTER, 0, -10)
        
        chart.set_axis_tick(lv.chart.AXIS.PRIMARY_Y, 10, 5, 6, 5, True, 40)
        chart.set_axis_tick(lv.chart.AXIS.PRIMARY_X, 10, 5, 10, 1, True, 30)
        
        chart.add_event_cb(self.event_cb, lv.EVENT.ALL, None)
        chart.refresh_ext_draw_size()
        
        self.cursor = chart.add_cursor(lv.palette_main(lv.PALETTE.BLUE), lv.DIR.LEFT | lv.DIR.BOTTOM)
        
        self.ser = chart.add_series(lv.palette_main(lv.PALETTE.RED), lv.chart.AXIS.PRIMARY_Y)

        self.ser_p = []
        for i in range(10):
            self.ser_p.append(lv.rand(10,90))
        self.ser.y_points = self.ser_p                     

        newser = chart.get_series_next(None)
        # print("length of data points: ",len(newser.points))
        chart.set_zoom_x(500)

        label = lv.label(lv.scr_act())
        label.set_text("Click on a point")
        label.align_to(chart, lv.ALIGN.OUT_TOP_MID, 0, -5)
        
        
    def event_cb(self,e):

        code = e.get_code()
        chart = e.get_target()

        if code == lv.EVENT.VALUE_CHANGED:
            # print("last_id: ",self.last_id)
            self.last_id = chart.get_pressed_point()
            if self.last_id != lv.CHART_POINT.NONE:
                p = lv.point_t()
                chart.get_point_pos_by_id(self.ser, self.last_id, p)
                chart.set_cursor_point(self.cursor, None, self.last_id)
                
        elif code == lv.EVENT.DRAW_PART_END:
            # print("EVENT.DRAW_PART_END")
            dsc = lv.obj_draw_part_dsc_t.__cast__(e.get_param())
            # if dsc.p1 and dsc.p2:
                # print("p1, p2", dsc.p1,dsc.p2)
                # print("p1.y, p2.y", dsc.p1.y, dsc.p2.y)
                # print("last_id: ",self.last_id)
            if dsc.part == lv.PART.CURSOR and dsc.p1 and dsc.p2 and dsc.p1.y == dsc.p2.y and self.last_id >= 0:

                v = self.ser_p[self.last_id];
     
                # print("value: ",v)
                value_txt = str(v)
                size = lv.point_t()
                lv.txt_get_size(size, value_txt, lv.font_default(), 0, 0, lv.COORD.MAX, lv.TEXT_FLAG.NONE)
                
                a = lv.area_t()
                a.y2 = dsc.p1.y - 5
                a.y1 = a.y2 - size.y - 10
                a.x1 = dsc.p1.x + 10;
                a.x2 = a.x1 + size.x + 10;
                
                draw_rect_dsc = lv.draw_rect_dsc_t()
                draw_rect_dsc.init()
                draw_rect_dsc.bg_color = lv.palette_main(lv.PALETTE.BLUE)
                draw_rect_dsc.radius = 3;
                
                lv.draw_rect(a, dsc.clip_area, draw_rect_dsc)
                
                draw_label_dsc = lv.draw_label_dsc_t()
                draw_label_dsc.init()
                draw_label_dsc.color = lv.color_white()
                a.x1 += 5
                a.x2 -= 5
                a.y1 += 5
                a.y2 -= 5
                lv.draw_label(a, dsc.clip_area, draw_label_dsc, value_txt, None)

example_chart_6 = ExampleChart_6()
