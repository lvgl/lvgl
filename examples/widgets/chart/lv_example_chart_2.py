def draw_event_cb(e):

    obj = e.get_target()

    # Add the faded area before the lines are drawn
    dsc = lv.obj_draw_part_dsc_t.__cast__(e.get_param())
    if dsc.part != lv.PART.ITEMS:
        return
    if not dsc.p1 or not dsc.p2:
        return

    # Add a line mask that keeps the area below the line
    line_mask_param = lv.draw_mask_line_param_t()
    line_mask_param.points_init(dsc.p1.x, dsc.p1.y, dsc.p2.x, dsc.p2.y, lv.DRAW_MASK_LINE_SIDE.BOTTOM)
    # line_mask_id = line_mask_param.draw_mask_add(None)
    line_mask_id = lv.draw_mask_add(line_mask_param, None)
    # Add a fade effect: transparent bottom covering top
    h = obj.get_height()
    fade_mask_param = lv.draw_mask_fade_param_t()
    coords = lv.area_t()
    obj.get_coords(coords)
    fade_mask_param.init(coords, lv.OPA.COVER, coords.y1 + h // 8, lv.OPA.TRANSP,coords.y2)
    fade_mask_id = lv.draw_mask_add(fade_mask_param,None)

    # Draw a rectangle that will be affected by the mask
    draw_rect_dsc = lv.draw_rect_dsc_t()
    draw_rect_dsc.init()
    draw_rect_dsc.bg_opa = lv.OPA._20
    draw_rect_dsc.bg_color = dsc.line_dsc.color

    a = lv.area_t()
    a.x1 = dsc.p1.x
    a.x2 = dsc.p2.x - 1
    a.y1 = min(dsc.p1.y, dsc.p2.y)
    coords = lv.area_t()
    obj.get_coords(coords)
    a.y2 = coords.y2
    lv.draw_rect(a, dsc.clip_area, draw_rect_dsc)

    # Remove the masks
    lv.draw_mask_remove_id(line_mask_id)
    lv.draw_mask_remove_id(fade_mask_id)


def add_data(timer):
    # LV_UNUSED(timer);
    cnt = 0;
    chart1.set_next_value(ser1, lv.rand(20, 90))

    if cnt % 4 == 0:
        chart1.set_next_value(ser2, lv.rand(40, 60))

    cnt +=1

#
# Add a faded area effect to the line chart
#

# Create a chart1
chart1 = lv.chart(lv.scr_act())
chart1.set_size(200, 150)
chart1.center()
chart1.set_type(lv.chart.TYPE.LINE)    # Show lines and points too

chart1.add_event_cb(draw_event_cb, lv.EVENT.DRAW_PART_BEGIN, None)
chart1.set_update_mode(lv.chart.UPDATE_MODE.CIRCULAR)

# Add two data series
ser1 = chart1.add_series(lv.palette_main(lv.PALETTE.RED), lv.chart.AXIS.PRIMARY_Y)
ser2 = chart1.add_series(lv.palette_main(lv.PALETTE.BLUE), lv.chart.AXIS.SECONDARY_Y)

for i in range(10):
    chart1.set_next_value(ser1, lv.rand(20, 90))
    chart1.set_next_value(ser2, lv.rand(30, 70))

timer = lv.timer_create(add_data, 200, None)
