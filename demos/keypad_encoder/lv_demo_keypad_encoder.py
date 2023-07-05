class KeyboardEncoder:
    def __init__(self):
        self.g = lv.group_create()
        self.g.set_default()

        cur_drv = lv.indev_t()
        cur_drv = cur_drv.get_next()

        while cur_drv != None:

            if cur_drv.get_type() == lv.INDEV_TYPE.KEYPAD:
                print("Found keypad")
                cur_drv.set_group(self.g)
            if cur_drv.get_type() == lv.INDEV_TYPE.ENCODER:
                print("Found encoder")
                cur_drv.set_group(self.g)

            cur_drv = cur_drv.get_next()

        self.tv = lv.tabview(lv.scr_act(), lv.DIR.TOP, lv.DPI_DEF // 3)

        self.t1 = self.tv.add_tab("Selectors")
        self.t2 = self.tv.add_tab("Text input")

        self.selectors_create(self.t1)
        self.text_input_create(self.t2)

        self.msgbox_create()

    def selectors_create(self,parent):
        parent.set_flex_flow(lv.FLEX_FLOW.COLUMN)
        parent.set_flex_align(lv.FLEX_ALIGN.START, lv.FLEX_ALIGN.CENTER, lv.FLEX_ALIGN.CENTER)

        obj = lv.table(parent)
        obj.set_cell_value(0, 0, "00")
        obj.set_cell_value(0, 1, "01")
        obj.set_cell_value(1, 0, "10")
        obj.set_cell_value(1, 1, "11")
        obj.set_cell_value(2, 0, "20")
        obj.set_cell_value(2, 1, "21")
        obj.set_cell_value(3, 0, "30")
        obj.set_cell_value(3, 1, "31")
        obj.add_flag(lv.obj.FLAG.SCROLL_ON_FOCUS)

        obj = lv.calendar(parent)
        obj.add_flag(lv.obj.FLAG.SCROLL_ON_FOCUS)

        obj = lv.btnmatrix(parent)
        obj.add_flag(lv.obj.FLAG.SCROLL_ON_FOCUS)
        obj = lv.checkbox(parent)
        obj.add_flag(lv.obj.FLAG.SCROLL_ON_FOCUS);

        obj = lv.slider(parent)
        obj.set_range(0, 10)
        obj.add_flag(lv.obj.FLAG.SCROLL_ON_FOCUS)

        obj = lv.switch(parent)
        obj.add_flag(lv.obj.FLAG.SCROLL_ON_FOCUS)

        obj = lv.spinbox(parent)
        obj.add_flag(lv.obj.FLAG.SCROLL_ON_FOCUS)

        obj = lv.dropdown(parent)
        obj.add_flag(lv.obj.FLAG.SCROLL_ON_FOCUS)

        obj = lv.roller(parent)
        obj.add_flag(lv.obj.FLAG.SCROLL_ON_FOCUS)

        list = lv.list(parent)
        list.update_layout()
        if list.get_height() > parent.get_content_height() :
            list.set_height(parent.get_content_height())

            list.add_btn(lv.SYMBOL.OK, "Apply")
            list.add_btn(lv.SYMBOL.CLOSE, "Close")
            list.add_btn(lv.SYMBOL.EYE_OPEN, "Show")
            list.add_btn(lv.SYMBOL.EYE_CLOSE, "Hide")
            list.add_btn(lv.SYMBOL.TRASH, "Delete")
            list.add_btn(lv.SYMBOL.COPY, "Copy")
            list.add_btn(lv.SYMBOL.PASTE, "Paste")

    def text_input_create(self,parent) :

        parent.set_flex_flow(lv.FLEX_FLOW.COLUMN)

        ta1 = lv.textarea(parent)
        ta1.set_width(lv.pct(100))
        ta1.set_one_line(True)
        ta1.set_placeholder_text("Click with an encoder to show a keyboard")

        ta2 = lv.textarea(parent)
        ta2.set_width(lv.pct(100))
        ta2.set_one_line(True)
        ta2.set_placeholder_text("Type something")

        self.kb = lv.keyboard(lv.scr_act())
        self.kb.add_flag(lv.obj.FLAG.HIDDEN)

        ta1.add_event(self.ta_event_cb, lv.EVENT.ALL, None)
        ta2.add_event(self.ta_event_cb, lv.EVENT.ALL, None)


    def msgbox_create(self):

        btns = ["Ok", "Cancel", ""]
        mbox = lv.msgbox(None, "Hi", "Welcome to the keyboard and encoder demo", btns, False)
        mbox.add_event(self.msgbox_event_cb, lv.EVENT.ALL, None)
        lv.group_focus_obj(mbox.get_btns())
        mbox.get_btns().add_state(lv.STATE.FOCUS_KEY)
        self.g.focus_freeze(True)

        mbox.align(lv.ALIGN.CENTER, 0, 0)

        bg = mbox.get_parent()
        bg.set_style_bg_opa(lv.OPA._70, 0)
        bg.set_style_bg_color(lv.palette_main(lv.PALETTE.GREY), 0)


    def msgbox_event_cb(self,e):

        code = e.get_code();
        msgbox = e.get_target()

        if code == lv.EVENT.VALUE_CHANGED:
            txt = msgbox.get_active_btn_text()
            if txt:
                msgbox.close()
                self.g.focus_freeze(False)
                lv.group_focus_obj(self.t1.get_child(0))
                self.t1.scroll_to(0, 0, lv.ANIM.OFF)

    def ta_event_cb(self,e) :

        indev = lv.indev_get_act()
        if indev == None :
            return
        indev_type = indev.get_type()

        code = e.get_code()
        ta = e.get_target_obj()

        if code == lv.EVENT.CLICKED and  indev_type == lv.INDEV_TYPE.ENCODER:
            self.kb.set_textarea(ta)
            self.kb.clear_flag(lv.obj.FLAG.HIDDEN)
            self.kb.group_focus_obj()
            self.kb.get_group().set_editing()
            self.tv.set_height(lv.pct(50))
            lv_obj_align(kb, LV_ALIGN_BOTTOM_MID, 0, 0);

        if code == lv.EVENT.READY or code == lv.EVENT.CANCEL:
            self.kb.add_flag(lv.obj.FLAG.HIDDEN)
            self.tv.set_height(lv.pct(100))


keyboard_encoder = KeyboardEncoder()
