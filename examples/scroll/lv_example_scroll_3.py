class ScrollExample_3():
    def __init__(self):
        self.button_cnt = 1
        #
        # Create a list with a floating button
        #

        list = lv.list(lv.scr_act())
        list.set_size(280, 220)
        list.center()

        for button_cnt in range(2):
            list.add_button(lv.SYMBOL.AUDIO,"Track {:d}".format(button_cnt))

        float_button = lv.button(list)
        float_button.set_size(50, 50)
        float_button.add_flag(lv.obj.FLAG.FLOATING)
        float_button.align(lv.ALIGN.BOTTOM_RIGHT, 0, -list.get_style_pad_right(lv.PART.MAIN))
        float_button.add_event(lambda evt: self.float_button_event_cb(evt,list), lv.EVENT.ALL, None)
        float_button.set_style_radius(lv.RADIUS_CIRCLE, 0)
        float_button.set_style_bg_image_src(lv.SYMBOL.PLUS, 0)
        float_button.set_style_text_font(lv.theme_get_font_large(float_button), 0)

    def float_button_event_cb(self,e,list):
        code = e.get_code()
        float_button = e.get_target_obj()

        if code == lv.EVENT.CLICKED:
            list_button = list.add_button(lv.SYMBOL.AUDIO, "Track {:d}".format(self.button_cnt))
            self.button_cnt += 1

            float_button.move_foreground()

            list_button.scroll_to_view(lv.ANIM.ON)

scroll_example_3 = ScrollExample_3()


