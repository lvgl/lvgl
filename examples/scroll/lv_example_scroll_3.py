class ScrollExample_3():
    def __init__(self):
        self.btn_cnt = 1
        #
        # Create a list a with a floating button
        #

        list = lv.list(lv.scr_act())
        list.set_size(280, 220)
        list.center()

        for btn_cnt in range(2):
            list.add_btn(lv.SYMBOL.AUDIO,"Track {:d}".format(btn_cnt))

        float_btn = lv.btn(list)
        float_btn.set_size(50, 50)
        float_btn.add_flag(lv.obj.FLAG.FLOATING)
        float_btn.align(lv.ALIGN.BOTTOM_RIGHT, 0, -list.get_style_pad_right(lv.PART.MAIN))
        float_btn.add_event_cb(lambda evt: self.float_btn_event_cb(evt,list), lv.EVENT.ALL, None)
        float_btn.set_style_radius(lv.RADIUS.CIRCLE, 0)
        float_btn.set_style_bg_img_src(lv.SYMBOL.PLUS, 0)
        float_btn.set_style_text_font(lv.theme_get_font_large(float_btn), 0)
            
    def float_btn_event_cb(self,e,list):
        code = e.get_code()
        float_btn = e.get_target()

        if code == lv.EVENT.CLICKED:
            list_btn = list.add_btn(lv.SYMBOL.AUDIO, "Track {:d}".format(self.btn_cnt))
            self.btn_cnt += 1

            float_btn.move_foreground()

            list_btn.scroll_to_view(lv.ANIM.ON)
            
scroll_example_3 = ScrollExample_3()


