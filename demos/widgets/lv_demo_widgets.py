#!/opt/bin/lv_micropython
import lvgl as lv
import time
import SDL
lv.init()

LV_DEMO_WIDGETS_SLIDESHOW = 0
LV_THEME_DEFAULT_COLOR_PRIMARY=lv.color_hex(0x01a2b1)
LV_THEME_DEFAULT_COLOR_SECONDARY=lv.color_hex(0x44d1b6)

LV_LED_BRIGHT_MIN = 120
LV_LED_BRIGHT_MAX = 255

SDL.init(w=480,h=320)

LV_DPI =130
LV_ANIM_REPEAT_INFINITE = -1

# Register SDL display driver.

disp_buf1 = lv.disp_buf_t()
buf1_1 = bytes(480 * 10)
disp_buf1.init(buf1_1, None, len(buf1_1)//4)
disp_drv = lv.disp_drv_t()
disp_drv.init()
disp_drv.buffer = disp_buf1
disp_drv.flush_cb = SDL.monitor_flush
disp_drv.hor_res = 480
disp_drv.ver_res = 320
disp_drv.register()

# Register SDL mouse driver

indev_drv = lv.indev_drv_t()
indev_drv.init()
indev_drv.type = lv.INDEV_TYPE.POINTER
indev_drv.read_cb = SDL.mouse_read
indev_drv.register()
# print("Running the Unix version")

# Create a screen and load it
scr=lv.obj()
lv.scr_load(scr)

def LV_DPX(n):
    if n == 0:
        return n
    scr=lv.scr_act()
    display = scr.get_disp()
    dpi = display.get_dpi()
    # print("dpi: ",dpi)
    tmp = (dpi*n+80)//160
    # print("tmp: ",tmp)
    if tmp > 1:
        return tmp
    else:
        return 1

def color_chg_event_cb(sw, e):
    if e == lv.EVENT.VALUE_CHANGED:
        flag = lv.THEME_MATERIAL_FLAG.LIGHT
        if sw.get_state():
            flag=lv.THEME_MATERIAL_FLAG.DARK
        theme = lv.theme_material_init(LV_THEME_DEFAULT_COLOR_PRIMARY,LV_THEME_DEFAULT_COLOR_SECONDARY,flag,
                                       lv.theme_get_font_small(), lv.theme_get_font_normal(), lv.theme_get_font_subtitle(),
                                       lv.theme_get_font_title())
def slider_event_cb(slider, e):
    if e == lv.EVENT.VALUE_CHANGED:
        if slider.get_type() == lv.slider.TYPE.NORMAL:
            slider_value = slider.get_value()
            slider.set_style_local_value_str(lv.slider.PART.KNOB, lv.STATE.DEFAULT, str(slider_value))
        else:
            slider_left=slider.get_left_value()
            slider_right=slider.get_value()
            slider_string = str(slider_left) + '-' + str(slider_right)
            # print("slider left value: %d, slider right value: %d"%(slider_left,slider_right))
            slider.set_style_local_value_str(lv.slider.PART.INDIC, lv.STATE.DEFAULT, slider_string)

def linemeter_anim(a,lmeter, value):
    lmeter.set_value(value)
    label = lmeter.get_child(None)
    label.set_text(str(value))
    label.align(lmeter, lv.ALIGN.CENTER, 0, 0)

def gauge_anim(a,gauge,val):
    gauge.set_value(0,val)
    label=gauge.get_child(None)
    label.set_text(str(val))
    label.align(gauge, lv.ALIGN.CENTER, 0, 0)

def bar_anim(task,bar):
    val = bar.get_value()
    # print("bar value: ",val)
    max_value = bar.get_max_value()
    val += 1
    if val > max_value:
        val=0
    bar.set_value(val,lv.ANIM.OFF)
    cpy_string = "Copying %d/%d"% (val, max_value)
    # print(cpy_string)
    bar.set_style_local_value_str(lv.bar.PART.BG, lv.STATE.DEFAULT, cpy_string)

def arc_phase1_ready_cb(a,arc):
    a_arc=lv.anim_t()
    a_arc.init()
    a_arc.set_custom_exec_cb(lambda a_arc, val: anim_phase2(a_arc,arc,val))
    a_arc.set_values(360, 0)
    a_arc.set_time(1000)
    a_arc.set_ready_cb(lambda a: arc_phase2_ready_cb(a,arc))
    lv.anim_t.start(a_arc)

def arc_phase2_ready_cb(a,arc):
    a_arc=lv.anim_t()
    a_arc.init()
    a_arc.set_custom_exec_cb(lambda a_arc, val: anim_phase1(a_arc,arc,val))
    a_arc.set_time(1000)
    a_arc.set_values(0, 360)
    a_arc.set_ready_cb(lambda a: arc_phase1_ready_cb(a,arc))
    lv.anim_t.start(a_arc)

def anim_phase1(a,arc,val):
    arc.set_end_angle(val)
    label = arc.get_child(None)
    label.set_text(str(val))
    label.align(arc, lv.ALIGN.CENTER, 0, 0)

def anim_phase2(a,arc,val):
    arc.set_end_angle(val)
    label = arc.get_child(None)
    label.set_text(str(val))
    label.align(arc, lv.ALIGN.CENTER, 0, 0)

def controls_create(parent):
    lv.page.set_scrl_layout(lv.page.__cast__(parent), lv.LAYOUT.PRETTY_TOP)
    if LV_DEMO_WIDGETS_SLIDESHOW == 0:
        btns = ["Cancel", "Ok", ""]
        m = lv.msgbox(lv.scr_act(), None)
        m.add_btns(btns)
        btnm = lv.btnmatrix.__cast__(m.get_btnmatrix())
        # print("type(btnm): ",type(btnm))
        btnm.set_btn_ctrl(1, lv.btnmatrix.CTRL.CHECK_STATE)

    h = lv.cont(parent, None)
    h.set_layout(lv.LAYOUT.PRETTY_MID)
    h.add_style(lv.cont.PART.MAIN, style_box)
    h.set_drag_parent(True)

    h.set_style_local_value_str(lv.cont.PART.MAIN, lv.STATE.DEFAULT, "Basics")

    disp_size = display.get_size_category()
    if disp_size <= lv.DISP_SIZE.SMALL:
        grid_w= lv.page.get_width_grid(lv.page.__cast__(parent),1,1)
        # print("grid_w: ",grid_w)
    else:
        grid_w= lv.page.get_width_grid(lv.page.__cast__(parent),2,1)
        # print("grid_w: ",grid_w)

    h.set_fit2(lv.FIT.NONE, lv.FIT.TIGHT)
    h.set_width(grid_w)
    btn = lv.btn(h,None)
    btn.set_fit2(lv.FIT.NONE, lv.FIT.TIGHT)
    if disp_size <= lv.DISP_SIZE.SMALL:
        button_width=h.get_width_grid(1,1)
    else:
        button_width=h.get_width_grid(2,1)
    # print("button_width: %d"%button_width)

    btn.set_width(button_width)
    label = lv.label(btn, None)
    label.set_text("Button")

    btn = lv.btn(h, btn)
    btn.toggle()
    label = lv.label(btn, None)
    label.set_text("Button")

    lv.switch(h, None)

    lv.checkbox(h, None)

    fit_w = h.get_width_fit()

    slider = lv.slider(h, None)
    slider.set_value(40, lv.ANIM.OFF)
    slider.set_event_cb(slider_event_cb)
    slider.set_width_margin(fit_w);

    # Use the knobs style value the display the current value in focused state
    slider.set_style_local_margin_top(lv.slider.PART.BG, lv.STATE.DEFAULT, LV_DPX(25))
    slider.set_style_local_value_font(lv.slider.PART.KNOB, lv.STATE.DEFAULT, lv.theme_get_font_small())
    slider.set_style_local_value_ofs_y(lv.slider.PART.KNOB, lv.STATE.FOCUSED, - LV_DPX(25))
    slider.set_style_local_value_opa(lv.slider.PART.KNOB, lv.STATE.DEFAULT, lv.OPA.TRANSP)
    slider.set_style_local_value_opa(lv.slider.PART.KNOB, lv.STATE.FOCUSED, lv.OPA.COVER)
    slider.set_style_local_transition_time(lv.slider.PART.KNOB, lv.STATE.DEFAULT, 300)
    slider.set_style_local_transition_prop_5(lv.slider.PART.KNOB, lv.STATE.DEFAULT, lv.STYLE.VALUE_OFS_Y)
    slider.set_style_local_transition_prop_6(lv.slider.PART.KNOB, lv.STATE.DEFAULT, lv.STYLE.VALUE_OPA)

    slider = lv.slider(h, None)
    slider.set_type(lv.slider.TYPE.RANGE)
    slider.set_value(70, lv.ANIM.OFF)
    slider.set_left_value(30, lv.ANIM.OFF)
    slider.set_style_local_value_ofs_y(lv.slider.PART.INDIC, lv.STATE.DEFAULT, - LV_DPX(25))
    slider.set_style_local_value_font(lv.slider.PART.INDIC, lv.STATE.DEFAULT, lv.theme_get_font_small())
    slider.set_style_local_value_opa(lv.slider.PART.INDIC, lv.STATE.DEFAULT, lv.OPA.COVER)
    slider.set_event_cb(slider_event_cb)
    lv.event_send(slider,lv.EVENT.VALUE_CHANGED, None)      # To refresh the text
    if slider.get_width() > fit_w:
        slider.set_width(fit_w)

    h = lv.cont(parent, h)
    h.set_fit(lv.FIT.NONE)
    h.set_style_local_value_str(lv.cont.PART.MAIN, lv.STATE.DEFAULT, "Text input")

    ta = lv.textarea(h, None)
    lv.cont.set_fit2(h,lv.FIT.PARENT, lv.FIT.NONE)
    ta.set_text("")
    ta.set_placeholder_text("E-mail address")
    ta.set_one_line(True)
    ta.set_cursor_hidden(True)
    #lv_obj_set_event_cb(ta, ta_event_cb);

    ta = lv.textarea(h, ta)
    ta.set_pwd_mode(True)
    ta.set_placeholder_text("Password")

    ta = lv.textarea(h, None)
    lv.cont.set_fit2(h,lv.FIT.PARENT, lv.FIT.NONE)
    ta.set_text("")
    ta.set_placeholder_text("Message")
    ta.set_cursor_hidden(True)
    #lv_obj_set_event_cb(ta, ta_event_cb);
    lv.cont.set_fit4(h, lv.FIT.PARENT, lv.FIT.PARENT, lv.FIT.NONE, lv.FIT.PARENT)

#    if LV_DEMO_WIDGETS_SLIDESHOW:
#        tab_content_anim_create(parent)

def visuals_create(parent):
    page = lv.page.__cast__(parent)
    lv.page.set_scrl_layout(page, lv.LAYOUT.PRETTY_TOP)

    disp_size = display.get_size_category()

    grid_h_chart = lv.page.get_height_grid(page,1,1)
    if disp_size <= lv.DISP_SIZE.LARGE:
        grid_w_chart=lv.page.get_width_grid(page,1,1)
    else:
        grid_w_chart=lv.page.get_width_grid(page,1,2)

    chart = lv.chart(parent, None)
    chart.add_style(lv.chart.PART.BG, style_box)
    if disp_size <= lv.DISP_SIZE.SMALL:
        chart.set_style_local_text_font(lv.chart.PART.SERIES_BG, lv.STATE.DEFAULT, lv.theme_get_font_small())

    chart.set_drag_parent(True)
    chart.set_style_local_value_str(lv.cont.PART.MAIN, lv.STATE.DEFAULT, "Line chart")
    chart.set_width_margin(grid_w_chart)
    chart.set_height_margin(grid_h_chart)
    chart.set_div_line_count(3, 0)
    chart.set_point_count(8)
    chart.set_type(lv.chart.TYPE.LINE)

    if disp_size > lv.DISP_SIZE.SMALL:
        chart.set_style_local_pad_left(lv.chart.PART.BG, lv.STATE.DEFAULT, 4 * (LV_DPI // 10))
        chart.set_style_local_pad_bottom(lv.chart.PART.BG, lv.STATE.DEFAULT, 3 * (LV_DPI // 10))
        chart.set_style_local_pad_right(lv.chart.PART.BG, lv.STATE.DEFAULT, 2 * (LV_DPI // 10))
        chart.set_style_local_pad_top(lv.chart.PART.BG, lv.STATE.DEFAULT, 2 * (LV_DPI // 10))
        chart.set_y_tick_length(0, 0)
        chart.set_x_tick_length(0, 0)
        chart.set_y_tick_texts("600\n500\n400\n300\n200", 0, lv.chart.AXIS.DRAW_LAST_TICK)
        chart.set_x_tick_texts("Jan\nFeb\nMar\nApr\nMay\nJun\nJul\nAug", 0, lv.chart.AXIS.DRAW_LAST_TICK)

    s1 = chart.add_series(LV_THEME_DEFAULT_COLOR_PRIMARY)
    s2 = chart.add_series(LV_THEME_DEFAULT_COLOR_SECONDARY)

    chart.set_next(s1, 10)
    chart.set_next(s1, 90)
    chart.set_next(s1, 30)
    chart.set_next(s1, 60)
    chart.set_next(s1, 10)
    chart.set_next(s1, 90)
    chart.set_next(s1, 30)
    chart.set_next(s1, 60)
    chart.set_next(s1, 10)
    chart.set_next(s1, 90)

    chart.set_next(s2, 32)
    chart.set_next(s2, 66)
    chart.set_next(s2, 5)
    chart.set_next(s2, 47)
    chart.set_next(s2, 32)
    chart.set_next(s2, 32)
    chart.set_next(s2, 66)
    chart.set_next(s2, 5)
    chart.set_next(s2, 47)
    chart.set_next(s2, 66)
    chart.set_next(s2, 5)
    chart.set_next(s2, 47)

    chart2 = lv.chart(parent, chart)
    chart2.set_type(lv.chart.TYPE.COLUMN)
    chart2.set_style_local_value_str(lv.cont.PART.MAIN, lv.STATE.DEFAULT, "Column chart")

    s1 = chart2.add_series(LV_THEME_DEFAULT_COLOR_PRIMARY)
    s2 = chart2.add_series(LV_THEME_DEFAULT_COLOR_SECONDARY)

    chart2.set_next(s1, 10)
    chart2.set_next(s1, 90)
    chart2.set_next(s1, 30)
    chart2.set_next(s1, 60)
    chart2.set_next(s1, 10)
    chart2.set_next(s1, 90)
    chart2.set_next(s1, 30)
    chart2.set_next(s1, 60)
    chart2.set_next(s1, 10)
    chart2.set_next(s1, 90)

    chart2.set_next(s2, 32)
    chart2.set_next(s2, 66)
    chart2.set_next(s2, 5)
    chart2.set_next(s2, 47)
    chart2.set_next(s2, 32)
    chart2.set_next(s2, 32)
    chart2.set_next(s2, 66)
    chart2.set_next(s2, 5)
    chart2.set_next(s2, 47)
    chart2.set_next(s2, 66)
    chart2.set_next(s2, 5)
    chart2.set_next(s2, 47)

    if disp_size <= lv.DISP_SIZE.SMALL:
        grid_w_meter = lv.page.get_width_grid(page, 1, 1)
    elif disp_size <= lv.DISP_SIZE.MEDIUM :
        grid_w_meter = lv.page.get_width_grid(page, 2, 1)
    else:
        grid_w_meter = lv.page.get_width_grid(page, 3, 1)

    meter_h = lv.page.get_height_fit(page)
    if grid_w_meter < meter_h:
        meter_size = grid_w_meter
    else:
        meter_size = meter_h

    lmeter = lv.linemeter(parent, None)
    lmeter.set_drag_parent(True)
    lmeter.set_value(50)
    lmeter.set_size(meter_size, meter_size)
    lmeter.add_style(lv.linemeter.PART.MAIN, style_box)
    lmeter.set_style_local_value_str(lv.linemeter.PART.MAIN, lv.STATE.DEFAULT, "Line meter")

    label = lv.label(lmeter, None)
    label.align(lmeter, lv.ALIGN.CENTER, 0, 0)
    label.set_style_local_text_font(lv.label.PART.MAIN, lv.STATE.DEFAULT, lv.theme_get_font_title());

    a_lm=lv.anim_t()
    a_lm.init()
    a_lm.set_custom_exec_cb(lambda a, val: linemeter_anim(a,lmeter,val))
    a_lm.set_values(0, 100)
    a_lm.set_time(4000)
    a_lm.set_playback_time(1000)
    a_lm.set_repeat_count(LV_ANIM_REPEAT_INFINITE)
    lv.anim_t.start(a_lm)

    gauge = lv.gauge(parent, None)
    gauge.set_drag_parent(True)
    gauge.set_size(meter_size, meter_size)
    gauge.add_style(lv.gauge.PART.MAIN, style_box)
    gauge.set_style_local_value_str(lv.gauge.PART.MAIN, lv.STATE.DEFAULT, "Gauge")

    label = lv.label(gauge, label)
    label.align(gauge, lv.ALIGN.CENTER, 0, grid_w_meter // 3)

    a_ga=lv.anim_t()
    a_ga.init()
    a_ga.set_custom_exec_cb(lambda a, val: linemeter_anim(a,lmeter,val))
    a_ga.set_values(0, 100)
    a_ga.set_time(4000)
    a_ga.set_playback_time(1000)
    a_ga.set_repeat_count(LV_ANIM_REPEAT_INFINITE)
    a_ga.set_custom_exec_cb(lambda a, val: gauge_anim(a,gauge,val))
    lv.anim_t.start(a_ga)

    arc = lv.arc(parent,None)
    arc.set_drag_parent(True)
    arc.set_bg_angles(0, 360)
    arc.set_rotation(270)
    arc.set_angles(0, 0)
    arc.set_size(meter_size, meter_size)
    arc.add_style(lv.arc.PART.BG, style_box)
    arc.set_style_local_value_str(lv.arc.PART.BG, lv.STATE.DEFAULT, "Arc");

    label = lv.label(arc)
    label.align(arc, lv.ALIGN.CENTER, 0, 0)

    a_arc=lv.anim_t()
    a_arc.init()
    a_arc.set_custom_exec_cb(lambda a_arc, val: anim_phase1(a_arc,arc,val))
    a_arc.set_values(0, 360)
    a_arc.set_ready_cb(lambda a: arc_phase1_ready_cb(a,arc))
    # a_arc.set_repeat_count(LV_ANIM_REPEAT_INFINITE)
    a_arc.set_time(1000)
    lv.anim_t.start(a_arc)

    # Create a bar and use the backgrounds value style property to display the current value
    bar_h = lv.cont(parent, None)
    bar_h.set_fit2(lv.FIT.NONE, lv.FIT.TIGHT)
    bar_h.add_style(lv.cont.PART.MAIN, style_box);
    bar_h.set_style_local_value_str(lv.cont.PART.MAIN, lv.STATE.DEFAULT, "Bar")

    if disp_size <= lv.DISP_SIZE.SMALL:
        bar_h.set_width(lv.page_get_width_grid(page, 1, 1))
    elif disp_size <= lv.DISP_SIZE.MEDIUM:
        bar_h.set_width(lv.page.get_width_grid(page, 2, 1))
    else:
        bar_h.set_width(lv.page.get_width_grid(parent, 3, 2))

    bar = lv.bar(bar_h,None)
    bar.set_width(lv.cont.get_width_fit(bar_h))
    bar.set_style_local_value_font(lv.bar.PART.BG, lv.STATE.DEFAULT, lv.theme_get_font_small())
    bar.set_style_local_value_align(lv.bar.PART.BG, lv.STATE.DEFAULT, lv.ALIGN.OUT_BOTTOM_MID)
    bar.set_style_local_value_ofs_y(lv.bar.PART.BG, lv.STATE.DEFAULT, LV_DPI // 20)
    bar.set_style_local_margin_bottom(lv.bar.PART.BG, lv.STATE.DEFAULT, LV_DPI // 7)
    bar.align(None, lv.ALIGN.CENTER, 0, 0)
    bar.set_value(30,lv.ANIM.OFF)

    led_h = lv.cont(parent, None)
    led_h.set_layout(lv.LAYOUT.PRETTY_MID)
    if disp_size <= lv.DISP_SIZE.SMALL:
        led_h.set_width(lv.page.get_width_grid(page, 1, 1))
    elif disp_size <= lv.DISP_SIZE.MEDIUM:
        led_h.set_width(lv.page.get_width_grid(page, 2, 1))
    else:
        led_h.set_width(lv.page.get_width_grid(page, 3, 1))

    led_h.set_height(lv.obj.get_height(lv.obj.__cast__(bar_h)))
    led_h.add_style(lv.cont.PART.MAIN, style_box)
    led_h.set_drag_parent(True)
    led_h.set_style_local_value_str(lv.cont.PART.MAIN, lv.STATE.DEFAULT, "LEDs")

    led = lv.led(led_h, None)
    led_size = lv.obj.get_height_fit(lv.obj.__cast__(led_h))
    led.set_size(led_size, led_size)
    led.off()

    led = lv.led(led_h, led)
    led.set_bright((LV_LED_BRIGHT_MAX - LV_LED_BRIGHT_MIN) // 2 + LV_LED_BRIGHT_MIN)

    led = lv.led(led_h, led)
    led.on()

    if disp_size == lv.DISP_SIZE.MEDIUM:
        led_h.add_protect(lv.PROTECT.POS)
        led_h.align(bar_h, lv.ALIGN.OUT_BOTTOM_MID, 0, lv.obj.get_style_margin_top(lv.obj.__cast__(led_h), lv.obj.PART.MAIN) + lv.obj.get_style_pad_inner(parent, lv.page.PART.SCROLLABLE))

    task = lv.task_create_basic()
    task.set_cb(lambda task: bar_anim(task, bar))
    task.set_period(100)
    task.set_prio(lv.TASK_PRIO.LOWEST)

def selectors_create(parent):
    page = lv.page.__cast__(parent)
    lv.page.set_scrl_layout(page, lv.LAYOUT.PRETTY_MID)

    grid_h = page.get_height_grid(1, 1)

    if disp_size <= lv.DISP_SIZE.SMALL:
        grid_w = page.get_width_grid(1, 1)
    else:
        grid_w = page.get_width_grid(2, 1)

    cal = lv.calendar(parent, None)
    cal.set_drag_parent(True)
    if disp_size > lv.DISP_SIZE.MEDIUM:
        cal.set_size(min(grid_h, grid_w), min(grid_h, grid_w));
    else :
        cal.set_size(grid_w, grid_w)
        if disp_size <= lv.DISP_SIZE.SMALL:
            cal.set_style_local_text_font(lv.calendat.PART.BG, lv.STATE.DEFAULT, lv.theme_get_font_small())

    hl = [{ "year":2020, "month":1, "day":5},
          {"year":2020, "month":1, "day":9}]

    h = lv.cont(parent, None)
    h.set_drag_parent(True)
    if disp_size <= lv.DISP_SIZE.SMALL:
        h.set_fit2(lv.FIT.NONE, lv.FIT.TIGHT)
        h.set_width(lv.page.get_width_fit(page))
        h.set_layout(lv.LAYOUT.COLUMN_MID)
    elif disp_size <= lv.DISP_SIZE.MEDIUM:
        h.set_size(lv.obj.get_width(lv.obj.__cast__(cal)), lv.obj.get_height(lv.obj.__cast__(cal)))
        h.set_layout(lv.LAYOUT.PRETTY_TOP);
    else :
        h.set_click(False)
        h.set_style_local_bg_opa(lv.PART.MAIN, lv.STATE.DEFAULT, lv.OPA.TRANSP)
        h.set_style_local_border_opa(lv.PART.MAIN, lv.STATE.DEFAULT, lv.OPA.TRANSP)
        h.set_style_local_pad_left(lv.cont.PART.MAIN, lv.STATE.DEFAULT, 0)
        h.set_style_local_pad_right(lv.cont.PART.MAIN, lv.STATE.DEFAULT, 0)
        h.set_style_local_pad_top(lv.cont.PART_MAIN, LV_STATE_DEFAULT, 0);
        h.set_size(min(grid_h, grid_w), min(grid_h, grid_w))
        h.set_layout(lv.LAYOUT.PRETTY_TOP)

    roller = lv.roller(h, None)
    roller.add_style(lv.cont.PART.MAIN, style_box)
    roller.set_style_local_value_str(lv.cont.PART.MAIN, lv.STATE.DEFAULT, "Roller")
    roller.set_auto_fit(False)
    roller.set_align(lv.label.ALIGN.CENTER)
    roller.set_visible_row_count(4)
    if disp_size <= lv.DISP_SIZE.SMALL:
        roller.set_width(lv.cont.get_width_grid(h, 1 , 1))
    else:
        roller.set_width(lv.cont.get_width_grid(h, 2 , 1))


    dd = lv.dropdown(h, None)
    dd.add_style(lv.cont.PART.MAIN, style_box)
    dd.set_style_local_value_str(lv.cont.PART.MAIN, lv.STATE.DEFAULT, "Dropdown")
    if disp_size <= lv.DISP_SIZE.SMALL:
        dd.set_width(lv.cont.get_width_grid(h, 1 , 1))
    else:
        dd.set_width(lv.cont.get_width_grid(h, 2 , 1))

    dd.set_options("Alpha\nBravo\nCharlie\nDelta\nEcho\nFoxtrot\nGolf\nHotel\nIndia\nJuliette\nKilo\nLima\nMike\nNovember\n"
                   "Oscar\nPapa\nQuebec\nRomeo\nSierra\nTango\nUniform\nVictor\nWhiskey\nXray\nYankee\nZulu")

    list = lv.list(parent, None)
    list.set_scroll_propagation(True)
    list.set_size(grid_w, grid_h)

    txts = [lv.SYMBOL.SAVE, "Save", lv.SYMBOL.CUT, "Cut", lv.SYMBOL.COPY, "Copy",
            lv.SYMBOL.OK, "This is a quite long text to scroll on the list", lv.SYMBOL.EDIT, "Edit", lv.SYMBOL.WIFI, "Wifi",
            lv.SYMBOL.BLUETOOTH, "Bluetooth",  lv.SYMBOL.GPS, "GPS", lv.SYMBOL.USB, "USB",
            lv.SYMBOL.SD_CARD, "SD card", lv.SYMBOL.CLOSE, "Close"]


    for i in range(0,len(txts)//2,2):
        btn = list.add_btn(txts[i], txts[i + 1]);
        lv.btn.set_checkable(lv.btn.__cast__(btn),True)

        # Make a button disabled
        if i == 4:
            btn.set_state(lv.btn.STATE.DISABLED)

    cal.set_highlighted_dates(hl, 2)

tv = lv.tabview(lv.scr_act(), None)
display = scr.get_disp()
disp_size = display.get_size_category()
tv.set_style_local_pad_left(lv.tabview.PART.TAB_BG, lv.STATE.DEFAULT, disp_drv.hor_res // 2)

sw = lv.switch(lv.scr_act(), None)
if lv.theme_get_flags() & lv.THEME_MATERIAL_FLAG.DARK:
   sw.on(LV_ANIM_OFF)
sw.set_event_cb(color_chg_event_cb)
sw.set_pos(LV_DPX(10), LV_DPX(10))
sw.set_style_local_value_str(lv.switch.PART.BG, lv.STATE.DEFAULT, "Dark")
sw.set_style_local_value_align(lv.switch.PART.BG, lv.STATE.DEFAULT, lv.ALIGN.OUT_RIGHT_MID)
sw.set_style_local_value_ofs_x(lv.switch.PART.BG, lv.STATE.DEFAULT, LV_DPI//35)

t1 = tv.add_tab("Controls")
t2 = tv.add_tab("Visuals")
t3 = tv.add_tab("Selectors")

style_box = lv.style_t()
style_box.init()
# print("LV_DPX(10): %d, LV_DPX(10): %d"%(LV_DPX(10),LV_DPX(30)))
style_box.set_value_align(lv.STATE.DEFAULT, lv.ALIGN.OUT_TOP_LEFT)
style_box.set_value_ofs_y(lv.STATE.DEFAULT, - LV_DPX(10))
style_box.set_margin_top(lv.STATE.DEFAULT, LV_DPX(30))

controls_create(t1)
visuals_create(t2)
selectors_create(t3)

