def event_cb(e):
    obj = e.get_target()
    id = obj.get_selected_btn()
    if id == 0:
        prev = True
    else:
        prev = False
    if id == 6:
        next = True
    else:
        next = False
    if prev or next:
        # Find the checked butto
        for i in range(7):
            if obj.has_btn_ctrl(i, lv.btnmatrix.CTRL.CHECKED):
                break
        if prev and i > 1:
            i-=1
        elif next and i < 5:
            i+=1

        obj.set_btn_ctrl(i, lv.btnmatrix.CTRL.CHECKED)

#
# Make a button group
#

style_bg = lv.style_t()
style_bg.init()
style_bg.set_pad_all(0)
style_bg.set_pad_gap(0)
style_bg.set_clip_corner(True)
style_bg.set_radius(lv.RADIUS.CIRCLE)
style_bg.set_border_width(0)


style_btn = lv.style_t()
style_btn.init()
style_btn.set_radius(0)
style_btn.set_border_width(1)
style_btn.set_border_opa(lv.OPA._50)
style_btn.set_border_color(lv.palette_main(lv.PALETTE.GREY))
style_btn.set_border_side(lv.BORDER_SIDE.INTERNAL)
style_btn.set_radius(0)

map = [lv.SYMBOL.LEFT,"1","2", "3", "4", "5",lv.SYMBOL.RIGHT, ""]

btnm = lv.btnmatrix(lv.scr_act())
btnm.set_map(map)
btnm.add_style(style_bg, 0);
btnm.add_style(style_btn, lv.PART.ITEMS)
btnm.add_event_cb(event_cb, lv.EVENT.VALUE_CHANGED, None)
btnm.set_size(225, 35)

# Allow selecting on one number at time
btnm.set_btn_ctrl_all(lv.btnmatrix.CTRL.CHECKABLE)
btnm.clear_btn_ctrl(0, lv.btnmatrix.CTRL.CHECKABLE)
btnm.clear_btn_ctrl(6, lv.btnmatrix.CTRL.CHECKABLE)

btnm.set_one_checked(True);
btnm.set_btn_ctrl(1, lv.btnmatrix.CTRL.CHECKED)

btnm.center()

