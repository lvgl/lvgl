import fs_driver

def ta_event_cb(e,kb):
    code = e.get_code()
    ta = e.get_target()

    if code == lv.EVENT.FOCUSED:
        if lv.indev_get_act() != None and lv.indev_get_act().get_type() != lv.INDEV_TYPE.KEYPAD :
            kb.set_textarea(ta)
            kb.clear_flag(lv.obj.FLAG.HIDDEN)
        elif code == lv.EVENT.CANCEL:
            kb.add_flag(lv.obj.FLAG.HIDDEN)
            ta.clear_state(ta, LV_STATE_FOCUSED);
            lv.indev_reset(None, ta)   # To forget the last clicked object to make it focusable again

fs_drv = lv.fs_drv_t()
fs_driver.fs_register(fs_drv, 'S')
font_simsun_16_cjk = lv.font_load("S:../../assets/font/lv_font_simsun_16_cjk.fnt")
if font_simsun_16_cjk == None:
    print("Error when loading chinese font")
    
pinyin_ime = lv.ime_pinyin(lv.scr_act())
pinyin_ime.set_style_text_font(font_simsun_16_cjk, 0)
# pinyin_ime.pinyin_set_dict(your_dict)  # Use a custom dictionary. If it is not set, the built-in dictionary will be used.

# ta1 
ta1 = lv.textarea(lv.scr_act())
ta1.set_one_line(True)
ta1.set_style_text_font(font_simsun_16_cjk, 0)
ta1.align(lv.ALIGN.TOP_LEFT, 0, 0)

# Create a keyboard and add it to ime_pinyin
kb = lv.keyboard(lv.scr_act())
pinyin_ime.pinyin_set_keyboard(kb)
kb.set_textarea(ta1)

ta1.add_event_cb(lambda evt: ta_event_cb(evt,kb), lv.EVENT.ALL, None)

# Get the cand_panel, and adjust its size and position
cand_panel = pinyin_ime.pinyin_get_cand_panel()
cand_panel.set_size(lv.pct(100), lv.pct(10))
cand_panel.align_to(kb, lv.ALIGN.OUT_TOP_MID, 0, 0)

# Try using ime_pinyin to output the Chinese below in the ta1 above
cz_label = lv.label(lv.scr_act())
cz_label.set_text("嵌入式系统（Embedded System），\n是一种嵌入机械或电气系统内部、具有专一功能和实时计算性能的计算机系统。")
cz_label.set_style_text_font(font_simsun_16_cjk, 0)
cz_label.set_width(310)
cz_label.align_to(ta1, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 0)

