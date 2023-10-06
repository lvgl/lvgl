def event_cb(e):
    obj = e.get_target_obj()
    id = obj.get_selected_button()
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
            if obj.has_button_ctrl(i, lv.buttonmatrix.CTRL.CHECKED):
                break
        if prev and i > 1:
            i-=1
        elif next and i < 5:
            i+=1

        obj.set_button_ctrl(i, lv.buttonmatrix.CTRL.CHECKED)

#
# Make a button group
#

style_bg = lv.style_t()
style_bg.init()
style_bg.set_pad_all(0)
style_bg.set_pad_gap(0)
style_bg.set_clip_corner(True)
style_bg.set_radius(lv.RADIUS_CIRCLE)
style_bg.set_border_width(0)


style_button = lv.style_t()
style_button.init()
style_button.set_radius(0)
style_button.set_border_width(1)
style_button.set_border_opa(lv.OPA._50)
style_button.set_border_color(lv.palette_main(lv.PALETTE.GREY))
style_button.set_border_side(lv.BORDER_SIDE.INTERNAL)
style_button.set_radius(0)

map = [lv.SYMBOL.LEFT,"1","2", "3", "4", "5",lv.SYMBOL.RIGHT, ""]

buttonm = lv.buttonmatrix(lv.scr_act())
buttonm.set_map(map)
buttonm.add_style(style_bg, 0)
buttonm.add_style(style_button, lv.PART.ITEMS)
buttonm.add_event(event_cb, lv.EVENT.VALUE_CHANGED, None)
buttonm.set_size(225, 35)

# Allow selecting on one number at time
buttonm.set_button_ctrl_all(lv.buttonmatrix.CTRL.CHECKABLE)
buttonm.clear_button_ctrl(0, lv.buttonmatrix.CTRL.CHECKABLE)
buttonm.clear_button_ctrl(6, lv.buttonmatrix.CTRL.CHECKABLE)

buttonm.set_one_checked(True)
buttonm.set_button_ctrl(1, lv.buttonmatrix.CTRL.CHECKED)

buttonm.center()

