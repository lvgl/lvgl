class LV_Example_Event_4:

    def __init__(self):
        #
        # Demonstrate the usage of draw event
        #
        self.size = 0
        self.size_dec = False
        self.cont = lv.obj(lv.scr_act())
        self.cont.set_size(200, 200)
        self.cont.center()
        self.cont.add_event_cb(self.event_cb, lv.EVENT.DRAW_PART_END, None)
        lv.timer_create(self.timer_cb, 30, None)
        
    def timer_cb(self,timer) :
        self.cont.invalidate()
        if self.size_dec :
            self.size -= 1
        else :
            self.size += 1

        if self.size == 50 :
            self.size_dec = True
        elif self.size == 0:
            self.size_dec = False

    def event_cb(self,e) :
        obj = e.get_target()
        dsc = e.get_draw_part_dsc()
        if dsc.class_p == lv.obj_class and dsc.part == lv.PART.MAIN :
            draw_dsc = lv.draw_rect_dsc_t()
            draw_dsc.init()
            draw_dsc.bg_color = lv.color_hex(0xffaaaa)
            draw_dsc.radius = lv.RADIUS_CIRCLE
            draw_dsc.border_color = lv.color_hex(0xff5555)
            draw_dsc.border_width = 2
            draw_dsc.outline_color = lv.color_hex(0xff0000)
            draw_dsc.outline_pad = 3
            draw_dsc.outline_width = 2
            
            a = lv.area_t()
            a.x1 = 0
            a.y1 = 0
            a.x2 = self.size
            a.y2 = self.size
            coords = lv.area_t()
            obj.get_coords(coords)
            coords.align(a, lv.ALIGN.CENTER, 0, 0)
            
            dsc.draw_ctx.rect(draw_dsc, a)

lv_example_event_4 = LV_Example_Event_4()
