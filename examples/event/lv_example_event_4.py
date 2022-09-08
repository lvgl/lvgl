LV_TRIGO_SHIFT = 15
#
# Demonstrate event bubbling
#

class LV_Example_Event_4:
    def __init__(self) :
        self.n = 3
        self.cont = lv.obj(lv.scr_act())
        self.cont.remove_style_all()
        self.cont.set_size(300, 300)
        self.label = lv.label(self.cont)
        self.label.set_text("{:d} sides".format(self.n))
        self.label.center()

        self.cont.add_event_cb(self.event_cb, lv.EVENT.DRAW_MAIN, None)
        lv.timer_create(self.timer_cb, 17, None)
    
    def timer_cb(self,timer) :
        print("timer_cb")
        if self.n < 3 or self.n > 32 :
            self.n = 3
    
        else :
            self.old_tick = 0
            self.tick = lv.tick_get()
            if not self.old_tick :
                self.old_tick = self.tick
                
            if self.tick - self.old_tick > 3000 :
                self.n += 1
                self.set_text("{:d} sides".format(n))
                self.old_tick = self.tick
        self.cont.invalidate()


    def event_cb(self,e):
        # The original target of the event. Can be the buttons or the container
        draw_ctx = e.get_draw_ctx()
        draw_dsc = lv.draw_rect_dsc_t()
        print("draw_ctx: ",draw_ctx)
        draw_dsc.init()
        draw_dsc.bg_color = lv.palette_main(lv.PALETTE.LIGHT_GREEN)
        draw_dsc.bg_opa = lv.OPA.COVER
        points = [lv.point_t()]*32
        r = 150.0
        tick = lv.tick_get()
        for i in range(self.n):
            print("tick: ",tick)
            print(i*360 // self.n)
            angle = i * 360 / self.n + ((tick % 36000) / 100)
            print("angle: ",angle)
            print("cos(angle: ", lv.trigo_cos(int(angle)))
            x = int(150 + (r * lv.trigo_cos(int(angle)))) >> LV_TRIGO_SHIFT
            y = int(150 + (r * lv.trigo_sin(int(angle)))) >> LV_TRIGO_SHIFT
            points[i].x = x
            points[i].y = y
                
        draw_ctx.draw_polygon(draw_ctx, draw_dsc, points, self.n)


lv_example_event_4 = LV_Example_Event_4()
