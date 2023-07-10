import fs_driver


def event_handler(e):
    code = e.get_code()
    obj = e.get_target()
    if code == lv.EVENT.VALUE_CHANGED:
        option = " "*10
        obj.get_selected_str(option, len(option))
        print("Selected value: %s\n" + option.strip())

#
# Roller with various alignments and larger text in the selected area
#

# A style to make the selected option larger
style_sel = lv.style_t()
style_sel.init()

try:
    style_sel.set_text_font(lv.font_montserrat_22)
except:
    fs_drv = lv.fs_drv_t()
    fs_driver.fs_register(fs_drv, 'S')
    print("montserrat-22 not enabled in lv_conf.h, dynamically loading the font")
    font_montserrat_22 = lv.font_load("S:" + "../../assets/font/montserrat-22.fnt")
    style_sel.set_text_font(font_montserrat_22)

opts = "\n".join(["1","2","3","4","5","6","7","8","9","10"])

# A roller on the left with left aligned text, and custom width
roller = lv.roller(lv.scr_act())
roller.set_options(opts, lv.roller.MODE.NORMAL)
roller.set_visible_row_count(2)
roller.set_width(100)
roller.add_style(style_sel, lv.PART.SELECTED)
roller.set_style_text_align(lv.TEXT_ALIGN.LEFT, 0)
roller.align(lv.ALIGN.LEFT_MID, 10, 0)
roller.add_event_cb(event_handler, lv.EVENT.ALL, None)
roller.set_selected(2, lv.ANIM.OFF)

# A roller in the middle with center aligned text, and auto (default) width
roller = lv.roller(lv.scr_act())
roller.set_options(opts, lv.roller.MODE.NORMAL)
roller.set_visible_row_count(3)
roller.add_style(style_sel, lv.PART.SELECTED)
roller.align(lv.ALIGN.CENTER, 0, 0)
roller.add_event_cb(event_handler, lv.EVENT.ALL, None)
roller.set_selected(5, lv.ANIM.OFF)

# A roller on the right with right aligned text, and custom width
roller = lv.roller(lv.scr_act())
roller.set_options(opts, lv.roller.MODE.NORMAL)
roller.set_visible_row_count(4)
roller.set_width(80)
roller.add_style(style_sel, lv.PART.SELECTED)
roller.set_style_text_align(lv.TEXT_ALIGN.RIGHT, 0)
roller.align(lv.ALIGN.RIGHT_MID, -10, 0)
roller.add_event_cb(event_handler, lv.EVENT.ALL, None)
roller.set_selected(8, lv.ANIM.OFF)
