def scroll_event_cb(e):

    cont = e.get_target()

    cont_a = lv.area_t()
    cont.get_coords(cont_a)
    cont_y_center = cont_a.y1 + cont_a.get_height() // 2

    r = cont.get_height() * 7 // 10

    child_cnt = cont.get_child_cnt()
    for i in range(child_cnt):
        child = cont.get_child(i)
        child_a = lv.area_t()
        child.get_coords(child_a)

        child_y_center = child_a.y1 + child_a.get_height() // 2

        diff_y = child_y_center - cont_y_center
        diff_y = abs(diff_y)

        # Get the x of diff_y on a circle.

        # If diff_y is out of the circle use the last point of the circle (the radius)
        if diff_y >= r:
            x = r
        else:
            # Use Pythagoras theorem to get x from radius and y
            x_sqr = r * r - diff_y * diff_y
            res = lv.sqrt_res_t()
            lv.sqrt(x_sqr, res, 0x8000)   # Use lvgl's built in sqrt root function
            x = r - res.i

        # Translate the item by the calculated X coordinate
        child.set_style_translate_x(x, 0)

        # Use some opacity with larger translations
        opa = lv.map(x, 0, r, lv.OPA.TRANSP, lv.OPA.COVER)
        child.set_style_opa(lv.OPA.COVER - opa, 0)

#
# Translate the object as they scroll
#

cont = lv.obj(lv.scr_act())
cont.set_size(200, 200)
cont.center()
cont.set_flex_flow(lv.FLEX_FLOW.COLUMN)
cont.add_event_cb(scroll_event_cb, lv.EVENT.SCROLL, None)
cont.set_style_radius(lv.RADIUS.CIRCLE, 0)
cont.set_style_clip_corner(True, 0)
cont.set_scroll_dir(lv.DIR.VER)
cont.set_scroll_snap_y(lv.SCROLL_SNAP.CENTER)
cont.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)

for i in range(20):
    btn = lv.btn(cont)
    btn.set_width(lv.pct(100))

    label = lv.label(btn)
    label.set_text("Button " + str(i))

    # Update the buttons position manually for first*
    lv.event_send(cont, lv.EVENT.SCROLL, None)

    # Be sure the fist button is in the middle
    #lv.obj.scroll_to_view(cont.get_child(0), lv.ANIM.OFF)
    cont.get_child(0).scroll_to_view(lv.ANIM.OFF)
