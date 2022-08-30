#!/opt/bin/lv_micropython -i
import lvgl as lv
import display_driver_utils
import fs_driver
import time,os,sys
from imagetools import get_png_info, open_png

LV_THEME_DEFAULT_DARK = 0
LV_OBJ_FLAG_FLEX_IN_NEW_TRACK = 1<<23

global session_desktop,session_tablet,session_mobile
global down1,down2,down2
global calendar
calendar = None

session_desktop = 1000
session_tablet  = 1000
session_mobile  = 1000

down1 = False
down2 = False
down3 = False


def LV_GRID_FR(x):
    return lv.COORD.MAX -100 +x

def profile_create(parent):
    panel1 = lv.obj(parent)
    panel1.set_height(lv.SIZE_CONTENT)
    panel1.set_width(lv.SIZE_CONTENT)

    # Create an image from the png file
    try:
        if osVersion == "unix":
            with open('assets/avatar.png','rb') as f:
                png_data = f.read()
        else:
            with open('images/avatar.png','rb') as f:
                png_data = f.read()
    except:
        print("Could not find avatar.png")
        sys.exit()
    
    avatar_argb = lv.img_dsc_t({
        'data_size': len(png_data),
        'data': png_data 
    })
    
    avatar = lv.img(panel1)
    avatar.set_src(avatar_argb)
    
    name = lv.label(panel1)
    name.set_text("Elena Smith")
    name.add_style(style_title, 0)

    dsc = lv.label(panel1)
    dsc.add_style(style_text_muted, 0)
    dsc.set_text("This is a short description of me.\nTake a look at my profile!" )
    dsc.set_long_mode(lv.label.LONG.WRAP)

    email_icn = lv.label(panel1)
    email_icn.add_style(style_icon, 0)
    email_icn.set_text(lv.SYMBOL.KEYBOARD)

    email_label = lv.label(panel1)
    email_label.set_text("elena@smith.com")

    call_icn = lv.label(panel1)
    call_icn.add_style(style_icon, 0)
    call_icn.set_text(lv.SYMBOL.CALL)

    call_label = lv.label(panel1)
    call_label.set_text("+79 246 123 4567")

    log_out_btn = lv.btn(panel1)
    log_out_btn.set_height(lv.SIZE_CONTENT)

    label = lv.label(log_out_btn)
    label.set_text("Log out")
    label.center()

    invite_btn = lv.btn(panel1)
    invite_btn.add_state(lv.STATE.DISABLED)
    invite_btn.set_height(lv.SIZE_CONTENT)

    label = lv.label(invite_btn)
    label.set_text("Invite")
    label.center()
    
    # Create a keyboard

    kb = lv.keyboard(lv.scr_act())
    kb.add_flag(lv.obj.FLAG.HIDDEN)

    # Create the second panel
    panel2 = lv.obj(parent)
    panel2.set_height(lv.SIZE_CONTENT)

    panel2_title = lv.label(panel2)
    panel2_title.set_text("Your profile")
    panel2_title.add_style(style_title, 0)

    user_name_label = lv.label(panel2)
    user_name_label.set_text("User name")
    user_name_label.add_style(style_text_muted, 0)

    user_name = lv.textarea(panel2)
    user_name.set_one_line(True)
    user_name.set_placeholder_text("Your name")
    # user_name.add_event_cb(ta_event_cb, lv.event.ALL, None)
    user_name.add_event_cb(lambda e: ta_event_cb(e,kb,tv),lv.EVENT.ALL,None)
                        
    password_label = lv.label(panel2)
    password_label.set_text("Password");
    password_label.add_style(style_text_muted, 0)

    password = lv.textarea(panel2)
    password.set_one_line(True);
    password.set_password_mode(True)
    password.set_placeholder_text("Min. 8 chars.")
    # user_name.add_event_cb(ta_event_cb, lv.event.ALL, None)
    password.add_event_cb(lambda e: ta_event_cb(e,kb,tv),lv.EVENT.ALL,None)
    # password.add_event_cb(ta_event_cb, lv.event.ALL, kb)

    gender_label = lv.label(panel2)
    gender_label.set_text("Gender")
    gender_label.add_style(style_text_muted, 0)

    gender = lv.dropdown(panel2)
    gender.set_options_static("Male\nFemale\nOther")

    birthday_label = lv.label(panel2)
    birthday_label.set_text("Birthday")
    birthday_label.add_style(style_text_muted, 0)

    birthdate = lv.textarea(panel2)
    birthdate.set_one_line(True)
    birthdate.add_event_cb(birthday_event_cb, lv.EVENT.ALL, None)

    # Create the third panel
    panel3 = lv.obj(parent)
    panel3_title = lv.label(panel3)
    panel3_title.set_text("Your skills")
    panel3_title.add_style(style_title, 0)

    experience_label = lv.label(panel3)
    experience_label.set_text("Experience");
    experience_label.add_style(style_text_muted, 0)

    slider1 = lv.slider(panel3)
    slider1.set_width(lv.pct(95))
    # slider1.add_event_cb(slider_event_cb, lv.event.ALL, None)
    slider1.refresh_ext_draw_size()

    team_player_label = lv.label(panel3)
    team_player_label.set_text("Team player")
    team_player_label.add_style(style_text_muted, 0)

    sw1 = lv.switch(panel3)

    hard_working_label = lv.label(panel3)
    hard_working_label.set_text("Hard-working");
    hard_working_label.add_style(style_text_muted, 0)

    sw2 = lv.switch(panel3)
    
    if disp_size == DISP_LARGE:
        print("large")

    else:
        
        grid_main_col_dsc = [LV_GRID_FR(1), lv.GRID_TEMPLATE_LAST]
        grid_main_row_dsc = [lv.GRID_CONTENT, lv.GRID_CONTENT, lv.GRID_CONTENT, lv.GRID_TEMPLATE_LAST]
        parent.set_grid_dsc_array(grid_main_col_dsc, grid_main_row_dsc)

        # Create the top panel
        grid_1_col_dsc = [lv.GRID_CONTENT, LV_GRID_FR(1), lv.GRID_TEMPLATE_LAST]
        grid_1_row_dsc = [lv.GRID_CONTENT, # Avatar
                          lv.GRID_CONTENT, # Name
                          lv.GRID_CONTENT, # Description
                          lv.GRID_CONTENT, # Email
                          lv.GRID_CONTENT, # Phone number
                          lv.GRID_CONTENT, # Button1
                          lv.GRID_CONTENT, # Button2
                          lv.GRID_TEMPLATE_LAST]

        panel1.set_grid_dsc_array(grid_1_col_dsc, grid_1_row_dsc);


        grid_2_col_dsc = [LV_GRID_FR(1), LV_GRID_FR(1), lv.GRID_TEMPLATE_LAST]
        grid_2_row_dsc = [
            lv.GRID_CONTENT,            # Title
            5,                          # Separator
            lv.GRID_CONTENT,            # Box title
            40,                         # Box
            lv.GRID_CONTENT,            # Box title
            40,                         # Box
            lv.GRID_CONTENT,            # Box title
            40,                         # Box
            lv.GRID_CONTENT,            # Box title
            40, lv.GRID_TEMPLATE_LAST ] # Box


        panel2.set_grid_dsc_array(grid_2_col_dsc, grid_2_row_dsc)
        panel3.set_grid_dsc_array(grid_2_col_dsc, grid_2_row_dsc)

        panel1.set_grid_cell(lv.GRID_ALIGN.STRETCH, 0, 1, lv.GRID_ALIGN.CENTER, 0, 1)

        dsc.set_style_text_align(lv.TEXT_ALIGN.CENTER, 0)

        avatar.set_grid_cell(lv.GRID_ALIGN.CENTER, 0, 2, lv.GRID_ALIGN.CENTER, 0, 1)
        name.set_grid_cell(lv.GRID_ALIGN.CENTER, 0, 2, lv.GRID_ALIGN.CENTER, 1, 1)
        dsc.set_grid_cell(lv.GRID_ALIGN.STRETCH, 0, 2, lv.GRID_ALIGN.START, 2, 1)
        email_icn.set_grid_cell(lv.GRID_ALIGN.CENTER, 0, 1, lv.GRID_ALIGN.CENTER, 3, 1)
        email_label.set_grid_cell(lv.GRID_ALIGN.START, 1, 1, lv.GRID_ALIGN.CENTER, 3, 1)
        call_icn.set_grid_cell(lv.GRID_ALIGN.CENTER, 0, 1, lv.GRID_ALIGN.CENTER, 4, 1)
        call_label.set_grid_cell(lv.GRID_ALIGN.START, 1, 1, lv.GRID_ALIGN.CENTER, 4, 1)
        log_out_btn.set_grid_cell(lv.GRID_ALIGN.STRETCH, 0, 2, lv.GRID_ALIGN.CENTER, 5, 1)
        invite_btn.set_grid_cell(lv.GRID_ALIGN.STRETCH, 0, 2, lv.GRID_ALIGN.CENTER, 6, 1)

        panel2.set_grid_cell(lv.GRID_ALIGN.STRETCH, 0, 1, lv.GRID_ALIGN.START, 1, 1)
        panel2_title.set_grid_cell(lv.GRID_ALIGN.START, 0, 2, lv.GRID_ALIGN.CENTER, 0, 1)
        user_name_label.set_grid_cell(lv.GRID_ALIGN.START, 0, 2, lv.GRID_ALIGN.START, 2, 1)
        user_name.set_grid_cell(lv.GRID_ALIGN.STRETCH, 0, 2, lv.GRID_ALIGN.START, 3, 1)
        password_label.set_grid_cell(lv.GRID_ALIGN.START, 0, 2, lv.GRID_ALIGN.START, 4, 1)
        password.set_grid_cell(lv.GRID_ALIGN.STRETCH, 0, 2, lv.GRID_ALIGN.START, 5, 1)
        birthday_label.set_grid_cell(lv.GRID_ALIGN.START, 0, 2, lv.GRID_ALIGN.START, 6, 1)
        birthdate.set_grid_cell(lv.GRID_ALIGN.STRETCH, 0, 2, lv.GRID_ALIGN.START, 7, 1)
        gender_label.set_grid_cell(lv.GRID_ALIGN.START, 0, 2, lv.GRID_ALIGN.START, 8, 1);
        gender.set_grid_cell(lv.GRID_ALIGN.STRETCH, 0, 2, lv.GRID_ALIGN.START, 9, 1);

        panel3.set_height(lv.SIZE_CONTENT)
        panel3.set_grid_cell(lv.GRID_ALIGN.STRETCH, 0, 1, lv.GRID_ALIGN.START, 2, 1)
        panel3_title.set_grid_cell(lv.GRID_ALIGN.START, 0, 2, lv.GRID_ALIGN.CENTER, 0, 1)
        experience_label.set_grid_cell(lv.GRID_ALIGN.START, 0, 1, lv.GRID_ALIGN.START, 2, 1)
        slider1.set_grid_cell(lv.GRID_ALIGN.CENTER, 0, 2, lv.GRID_ALIGN.CENTER, 3, 1)
        hard_working_label.set_grid_cell(lv.GRID_ALIGN.START, 0, 1, lv.GRID_ALIGN.START, 4, 1)
        sw1.set_grid_cell(lv.GRID_ALIGN.START, 0, 1, lv.GRID_ALIGN.START, 5, 1);
        team_player_label.set_grid_cell(lv.GRID_ALIGN.START, 1, 1, lv.GRID_ALIGN.START, 4, 1)
        sw2.set_grid_cell(lv.GRID_ALIGN.START, 1, 1, lv.GRID_ALIGN.START, 5, 1)
        
def analytics_create(parent):
    parent.set_flex_flow(lv.FLEX_FLOW.ROW_WRAP)

    grid_chart_row_dsc = [lv.GRID_CONTENT, LV_GRID_FR(1), 10, lv.GRID_TEMPLATE_LAST]
    grid_chart_col_dsc = [20, LV_GRID_FR(1), lv.GRID_TEMPLATE_LAST]

    chart1_cont = lv.obj(parent)
    chart1_cont.set_flex_grow(1)
    chart1_cont.set_grid_dsc_array(grid_chart_col_dsc, grid_chart_row_dsc);

    chart1_cont.set_height(lv.pct(100))
    chart1_cont.set_style_max_height(300, 0)

    title = lv.label(chart1_cont)
    title.set_text("Unique visitors");
    title.add_style(style_title, 0)
    title.set_grid_cell(lv.GRID_ALIGN.START, 0, 2, lv.GRID_ALIGN.START, 0, 1)

    chart1 = lv.chart(chart1_cont)
    #lv.group_add_obj(lv.group_get_default(), chart1)
    # group = lv.group_get_default()

    chart1.add_flag(lv.obj.FLAG.SCROLL_ON_FOCUS)
    chart1.set_grid_cell(lv.GRID_ALIGN.STRETCH, 1, 1, lv.GRID_ALIGN.STRETCH, 1, 1)
    chart1.set_axis_tick(lv.chart.AXIS.PRIMARY_Y, 0, 0, 5, 1, True, 80)
    chart1.set_axis_tick(lv.chart.AXIS.PRIMARY_X, 0, 0, 12, 1, True, 50)
    chart1.set_div_line_count(0, 12)
    chart1.set_point_count(12)
    # chart1.add_event_cb(chart_event_cb, lv.event.ALL, None)
    if disp_size == DISP_SMALL:
        chart1.set_zoom_x(256 * 3)
    elif disp_size == DISP_MEDIUM:
        chart1.set_zoom_x(256 * 2)

    chart1.set_style_border_side(lv.BORDER_SIDE.LEFT | lv.BORDER_SIDE.BOTTOM, 0)
    chart1.set_style_radius(0, 0)

    ser1 = chart1.add_series(lv.theme_get_color_primary(chart1), lv.chart.AXIS.PRIMARY_Y);
    chart1.set_next_value(ser1, lv.rand(10, 80))
    chart1.set_next_value(ser1, lv.rand(10, 80))
    chart1.set_next_value(ser1, lv.rand(10, 80))
    chart1.set_next_value(ser1, lv.rand(10, 80))
    chart1.set_next_value(ser1, lv.rand(10, 80))
    chart1.set_next_value(ser1, lv.rand(10, 80))
    chart1.set_next_value(ser1, lv.rand(10, 80))
    chart1.set_next_value(ser1, lv.rand(10, 80))
    chart1.set_next_value(ser1, lv.rand(10, 80))
    chart1.set_next_value(ser1, lv.rand(10, 80))
    chart1.set_next_value(ser1, lv.rand(10, 80))
    chart1.set_next_value(ser1, lv.rand(10, 80))
    chart1.set_next_value(ser1, lv.rand(10, 80))

    chart2_cont = lv.obj(parent)
    chart2_cont.add_flag(LV_OBJ_FLAG_FLEX_IN_NEW_TRACK)
    chart2_cont.set_flex_grow(1)

    chart2_cont.set_height(lv.pct(100))
    chart2_cont.set_style_max_height(300, 0)

    chart2_cont.set_grid_dsc_array(grid_chart_col_dsc, grid_chart_row_dsc)

    title = lv.label(chart2_cont)
    title.set_text("Monthly revenue")
    title.add_style(style_title, 0)
    title.set_grid_cell(lv.GRID_ALIGN.START, 0, 2, lv.GRID_ALIGN.START, 0, 1)

    chart2 = lv.chart(chart2_cont)
    # lv.group_add_obj(lv.group_get_default(), chart2)
    chart2.add_flag(lv.obj.FLAG.SCROLL_ON_FOCUS)

    chart2.set_grid_cell(lv.GRID_ALIGN.STRETCH, 1, 1, lv.GRID_ALIGN.STRETCH, 1, 1)
    chart2.set_axis_tick(lv.chart.AXIS.PRIMARY_Y, 0, 0, 5, 1, True, 80)
    chart2.set_axis_tick(lv.chart.AXIS.PRIMARY_X, 0, 0, 12, 1, True, 50)
    chart2.set_size(lv.pct(100), lv.pct(100))
    chart2.set_type(lv.chart.TYPE.BAR)
    chart2.set_div_line_count(6, 0)
    chart2.set_point_count(12)
    # chart2.add_event_cb(chart_event_cb, lv.event.ALL, None)
    chart2.set_zoom_x(256 * 2)
    chart2.set_style_border_side(lv.BORDER_SIDE.LEFT | lv.BORDER_SIDE.BOTTOM, 0)
    chart2.set_style_radius(0, 0)

    if disp_size == DISP_SMALL:
        chart2.set_style_pad_gap(0, lv.PART.ITEMS)
        chart2.set_style_pad_gap(2, lv.PART.MAIN)
    
    elif disp_size == DISP_LARGE:
        chart2.set_style_pad_gap(16, 0)

    ser2 = chart2.add_series(lv.palette_lighten(lv.PALETTE.GREY, 1), lv.chart.AXIS.PRIMARY_Y)
    chart2.set_next_value(ser2, lv.rand(10, 80))
    chart2.set_next_value(ser2, lv.rand(10, 80))
    chart2.set_next_value(ser2, lv.rand(10, 80))
    chart2.set_next_value(ser2, lv.rand(10, 80))
    chart2.set_next_value(ser2, lv.rand(10, 80))
    chart2.set_next_value(ser2, lv.rand(10, 80))
    chart2.set_next_value(ser2, lv.rand(10, 80))
    chart2.set_next_value(ser2, lv.rand(10, 80))
    chart2.set_next_value(ser2, lv.rand(10, 80))
    chart2.set_next_value(ser2, lv.rand(10, 80))
    chart2.set_next_value(ser2, lv.rand(10, 80))
    chart2.set_next_value(ser2, lv.rand(10, 80))
    chart2.set_next_value(ser2, lv.rand(10, 80))

    ser3 = chart2.add_series(lv.theme_get_color_primary(chart1), lv.chart.AXIS.PRIMARY_Y)
    chart2.set_next_value(ser3, lv.rand(10, 80))
    chart2.set_next_value(ser3, lv.rand(10, 80))
    chart2.set_next_value(ser3, lv.rand(10, 80))
    chart2.set_next_value(ser3, lv.rand(10, 80))
    chart2.set_next_value(ser3, lv.rand(10, 80))
    chart2.set_next_value(ser3, lv.rand(10, 80))
    chart2.set_next_value(ser3, lv.rand(10, 80))
    chart2.set_next_value(ser3, lv.rand(10, 80))
    chart2.set_next_value(ser3, lv.rand(10, 80))
    chart2.set_next_value(ser3, lv.rand(10, 80))
    chart2.set_next_value(ser3, lv.rand(10, 80))
    chart2.set_next_value(ser3, lv.rand(10, 80))
    chart2.set_next_value(ser3, lv.rand(10, 80))
    
    meter1 = create_meter_box(parent, "Monthly Target", "Revenue: 63%", "Sales: 44%", "Costs: 58%");
    lv.obj.get_parent(lv.obj.__cast__(meter1)).add_flag(LV_OBJ_FLAG_FLEX_IN_NEW_TRACK)
    scale = meter1.add_scale()
    meter1.set_scale_range(scale, 0, 100, 270, 90);
    meter1.set_scale_ticks(scale, 0, 0, 0, lv.color_black())
    indic1 = meter1.add_arc(scale, 15, lv.palette_main(lv.PALETTE.BLUE), 0)
    indic2 = meter1.add_arc(scale, 15, lv.palette_main(lv.PALETTE.RED), -20)
    indic3 = meter1.add_arc(scale, 15, lv.palette_main(lv.PALETTE.GREEN), -40)

    a1 = lv.anim_t()
    a1.init()
    a1.set_values(20, 100)
    a1.set_repeat_count(lv.ANIM_REPEAT_INFINITE)
    a1.set_time(4100)
    a1.set_var(indic1)
    a1.set_playback_time(2700)
    a1.set_custom_exec_cb(lambda a1,val: meter1_indic1_anim_cb(meter1,indic1,val))
    lv.anim_t.start(a1)
    
    a2 = lv.anim_t()
    a2.init()
    a2.set_values(20, 100)
    a2.set_repeat_count(lv.ANIM_REPEAT_INFINITE)
    a2.set_var(indic2)
    a2.set_time(2600)
    a2.set_playback_time(3200)
    a2.set_custom_exec_cb(lambda a2,val: meter1_indic2_anim_cb(meter1,indic2,val))
    lv.anim_t.start(a2)
    
    a3 = lv.anim_t()
    a3.init()
    a3.set_values(20, 100)
    a3.set_repeat_count(lv.ANIM_REPEAT_INFINITE)
    a3.set_var(indic3)
    a3.set_time(2800)
    a3.set_playback_time(1800)
    a3.set_custom_exec_cb(lambda a3,val: meter1_indic3_anim_cb(meter1,indic3,val))
    lv.anim_t.start(a3)

    meter2 = create_meter_box(parent, "Sessions", "Desktop: ", "Tablet: ", "Mobile: ")
    if disp_size < DISP_LARGE:
        lv.obj.get_parent(lv.obj.__cast__(meter2)).add_flag(LV_OBJ_FLAG_FLEX_IN_NEW_TRACK)
        
    scale = meter2.add_scale()
    meter2.set_scale_range(scale, 0, 100, 360, 90)
    meter2.set_scale_ticks(scale, 0, 0, 0, lv.color_black())

    meter2_indic= [None]*3
    meter2_indic[0] = meter2.add_arc(scale, 20, lv.palette_main(lv.PALETTE.RED), -10)
    meter2.set_indicator_start_value(meter2_indic[0], 0)
    meter2.set_indicator_end_value(meter2_indic[0], 39)

    meter2_indic[1] = meter2.add_arc(scale, 30, lv.palette_main(lv.PALETTE.BLUE), 0)
    meter2.set_indicator_start_value(meter2_indic[1], 40)
    meter2.set_indicator_end_value(meter2_indic[1], 69)

    meter2_indic[2] = meter2.add_arc(scale, 10, lv.palette_main(lv.PALETTE.GREEN), -20)
    meter2.set_indicator_start_value(meter2_indic[2], 70)
    meter2.set_indicator_end_value(meter2_indic[2], 99)

    lv.timer_create(lambda t: meter2_timer_cb(t,meter2,meter2_indic), 100, None)

    meter3 = create_meter_box(parent, "Network Speed", "Low speed", "Normal Speed", "High Speed")
    if disp_size < DISP_LARGE:
        lv.obj.get_parent(lv.obj.__cast__(meter3)).add_flag(LV_OBJ_FLAG_FLEX_IN_NEW_TRACK)

    # Add a special circle to the needle's pivot
    meter3.set_style_pad_hor(10, 0)
    meter3.set_style_size(10, 10, lv.PART.INDICATOR)
    meter3.set_style_radius(lv.RADIUS_CIRCLE, lv.PART.INDICATOR)
    meter3.set_style_bg_opa(lv.OPA.COVER, lv.PART.INDICATOR)
    meter3.set_style_bg_color(lv.palette_darken(lv.PALETTE.GREY, 4), lv.PART.INDICATOR)
    meter3.set_style_outline_color(lv.color_white(), lv.PART.INDICATOR);
    meter3.set_style_outline_width(3, lv.PART.INDICATOR)
    meter3.set_style_text_color(lv.palette_darken(lv.PALETTE.GREY, 1), lv.PART.TICKS)

    scale = meter3.add_scale()
    meter2.set_scale_range(scale, 10, 60, 220, 360 - 220)
    meter3.set_scale_ticks(scale, 21, 3, 17, lv.color_white())
    meter3.set_scale_major_ticks(scale, 4, 4, 22, lv.color_white(), 15)

    indic4 = meter3.add_arc(scale, 10, lv.palette_main(lv.PALETTE.RED), 0)
    meter3.set_indicator_start_value(indic4, 0)
    meter3.set_indicator_end_value(indic4, 20)

    indic5 = meter3.add_scale_lines(scale, lv.palette_darken(lv.PALETTE.RED, 3), lv.palette_darken(lv.PALETTE.RED, 3), True, 0)
    meter3.set_indicator_start_value(indic5, 0)
    meter3.set_indicator_end_value(indic5, 20)

    indic6 = meter3.add_arc(scale, 12, lv.palette_main(lv.PALETTE.BLUE), 0)
    meter3.set_indicator_start_value(indic6, 20)
    meter3.set_indicator_end_value(indic6, 40)

    indic7 = meter3.add_scale_lines(scale, lv.palette_darken(lv.PALETTE.BLUE, 3), lv.palette_darken(lv.PALETTE.BLUE, 3), True, 0)
    meter3.set_indicator_start_value(indic7, 20)
    meter3.set_indicator_end_value(indic7, 40)

    indic8 = meter3.add_arc(scale, 10, lv.palette_main(lv.PALETTE.GREEN), 0)
    meter3.set_indicator_start_value(indic8, 40)
    meter3.set_indicator_end_value(indic8, 60)

    indic9 = meter3.add_scale_lines(scale, lv.palette_darken(lv.PALETTE.GREEN, 3), lv.palette_darken(lv.PALETTE.GREEN, 3), True, 0)
    meter3.set_indicator_start_value(indic9, 40)
    meter3.set_indicator_end_value(indic9, 60)

    indic10 = meter3.add_needle_line(scale, 4, lv.palette_darken(lv.PALETTE.GREY, 4), -25)

    mbps_label = lv.label(meter3)
    mbps_label.set_text("-- Mbps")
    mbps_label.add_style(style_title, 0)

    # mbps_unit_label = lv.label(meter3)
    # mbps_unit_label.set_text("Mbps")
    
    a4= lv.anim_t()
    a4.init()
    a4.set_values(10, 60)
    a4.set_repeat_count(lv.ANIM_REPEAT_INFINITE)
    a4.set_var(indic10)
    a4.set_time(4100)
    a4.set_playback_time(800)
    a4.set_custom_exec_cb(lambda a4,val: meter3_anim_cb(meter3,indic10,mbps_label,val))
    lv.anim_t.start(a4)
                                     
    parent.update_layout()
    if disp_size == DISP_MEDIUM:
        meter1.set_size(200, 200)
        meter2.set_size(200, 200)
        meter3.set_size(200, 200)
    else :
        meter_w = meter1.get_width()
        meter1.set_height(meter_w)
        meter2.set_height(meter_w)
        meter3.set_height(meter_w)

    mbps_label.align(lv.ALIGN.TOP_MID, 10, lv.pct(55))
    # mbps_unit_label.align_to(mbps_label, lv.ALIGN.OUT_RIGHT_BOTTOM, 10, 0)
    
def create_meter_box(parent, title, text1, text2, text3):

    cont = lv.obj(parent)
    cont.set_height(lv.SIZE_CONTENT)
    cont.set_flex_grow(1)

    title_label = lv.label(cont)
    title_label.set_text(title)
    title_label.add_style(style_title, 0)

    meter = lv.meter(cont)
    meter.remove_style(None, lv.PART.MAIN)
    meter.remove_style(None, lv.PART.INDICATOR)
    meter.set_width(lv.pct(100))

    bullet1 = lv.obj(cont)
    bullet1.set_size(13, 13)
    bullet1.remove_style(None, lv.PART.SCROLLBAR)
    bullet1.add_style(style_bullet, 0)
    bullet1.set_style_bg_color(lv.palette_main(lv.PALETTE.RED), 0)
    label1 = lv.label(cont)
    label1.set_text(text1);

    bullet2 = lv.obj(cont)
    bullet2.set_size(13, 13)
    bullet2.remove_style(None, lv.PART.SCROLLBAR)
    bullet2.add_style(style_bullet, 0)
    bullet2.set_style_bg_color(lv.palette_main(lv.PALETTE.BLUE), 0)
    label2 = lv.label(cont)
    label2.set_text(text2)

    bullet3 = lv.obj(cont)
    bullet3.set_size(13, 13)
    bullet3.remove_style(None, lv.PART.SCROLLBAR)
    bullet3.add_style(style_bullet, 0)
    bullet3.set_style_bg_color(lv.palette_main(lv.PALETTE.GREEN), 0)
    label3 = lv.label(cont)
    label3.set_text(text3)

    if disp_size == DISP_MEDIUM:
        grid_col_dsc = [lv.GRID_CONTENT, LV_GRID_FR(1), lv.GRID_CONTENT,LV_GRID_FR(8), lv.GRID_TEMPLATE_LAST]
        grid_row_dsc = [lv.GRID_CONTENT, LV_GRID_FR(1), lv.GRID_CONTENT, lv.GRID_CONTENT, lv.GRID_CONTENT, LV_GRID_FR(1), lv.GRID_TEMPLATE_LAST]

        cont.set_grid_dsc_array(grid_col_dsc, grid_row_dsc);
        title_label.set_grid_cell(lv.GRID_ALIGN.START, 0, 4, lv.GRID_ALIGN.START, 0, 1)
        meter.set_grid_cell(lv.GRID_ALIGN.START, 0, 1, lv.GRID_ALIGN.START, 1, 3)
        bullet1.set_grid_cell(lv.GRID_ALIGN.START, 2, 1, lv.GRID_ALIGN.CENTER, 2, 1)
        bullet2.set_grid_cell(lv.GRID_ALIGN.START, 2, 1, lv.GRID_ALIGN.CENTER, 3, 1)
        bullet3.set_grid_cell(lv.GRID_ALIGN.START, 2, 1, lv.GRID_ALIGN.CENTER, 4, 1)
        label1.set_grid_cell(lv.GRID_ALIGN.STRETCH, 3, 1, lv.GRID_ALIGN.CENTER, 2, 1)
        label2.set_grid_cell(lv.GRID_ALIGN.STRETCH, 3, 1, lv.GRID_ALIGN.CENTER, 3, 1)
        label3.set_grid_cell(lv.GRID_ALIGN.STRETCH, 3, 1, lv.GRID_ALIGN.CENTER, 4, 1)

    else:
        grid_col_dsc = [lv.GRID_CONTENT, LV_GRID_FR(1), lv.GRID_TEMPLATE_LAST]
        grid_row_dsc = [lv.GRID_CONTENT, lv.GRID_CONTENT, lv.GRID_CONTENT, lv.GRID_CONTENT, lv.GRID_CONTENT, lv.GRID_TEMPLATE_LAST]
        cont.set_grid_dsc_array(grid_col_dsc, grid_row_dsc)
        title_label.set_grid_cell(lv.GRID_ALIGN.START, 0, 2, lv.GRID_ALIGN.START, 0, 1)
        meter.set_grid_cell(lv.GRID_ALIGN.START, 0, 2, lv.GRID_ALIGN.START, 1, 1)
        bullet1.set_grid_cell(lv.GRID_ALIGN.START, 0, 1, lv.GRID_ALIGN.START, 2, 1)
        bullet2.set_grid_cell(lv.GRID_ALIGN.START, 0, 1, lv.GRID_ALIGN.START, 3, 1)
        bullet3.set_grid_cell(lv.GRID_ALIGN.START, 0, 1, lv.GRID_ALIGN.START, 4, 1)
        label1.set_grid_cell(lv.GRID_ALIGN.STRETCH, 1, 1, lv.GRID_ALIGN.START, 2, 1)
        label2.set_grid_cell(lv.GRID_ALIGN.STRETCH, 1, 1, lv.GRID_ALIGN.START, 3, 1)
        label3.set_grid_cell(lv.GRID_ALIGN.STRETCH, 1, 1, lv.GRID_ALIGN.START, 4, 1)

    return meter

def shop_create(parent):
    parent.set_flex_flow(lv.FLEX_FLOW.ROW_WRAP)

    panel1 = lv.obj(parent)
    panel1.set_size(lv.pct(100), lv.SIZE_CONTENT)
    panel1.set_style_pad_bottom(30, 0)

    title = lv.label(panel1)
    title.set_text("Monthly Summary")
    title.add_style(style_title, 0)

    date = lv.label(panel1)
    date.set_text("8-15 July, 2021")
    date.add_style(style_text_muted, 0)

    amount = lv.label(panel1)
    amount.set_text("$27,123.25")
    amount.add_style(style_title, 0)

    hint = lv.label(panel1)
    hint.set_text(lv.SYMBOL.UP + " 17% growth this week")
    hint.set_style_text_color(lv.palette_main(lv.PALETTE.GREEN), 0)

    chart3 = lv.chart(panel1)
    chart3.set_axis_tick(lv.chart.AXIS.PRIMARY_Y, 0, 0, 6, 1, True, 80)
    chart3.set_axis_tick(lv.chart.AXIS.PRIMARY_X, 0, 0, 7, 1, True, 50)
    chart3.set_type(lv.chart.TYPE.BAR)
    chart3.set_div_line_count(6, 0)
    chart3.set_point_count(7)
    #chart3.add_event_cb(chart3, shop_chart_event_cb, LV_EVENT_ALL, NULL);

    ser4 = chart3.add_series(lv.theme_get_color_primary(chart3), lv.chart.AXIS.PRIMARY_Y)
    chart3.set_next_value(ser4, lv.rand(60, 90))
    chart3.set_next_value(ser4, lv.rand(60, 90))
    chart3.set_next_value(ser4, lv.rand(60, 90))
    chart3.set_next_value(ser4, lv.rand(60, 90))
    chart3.set_next_value(ser4, lv.rand(60, 90))
    chart3.set_next_value(ser4, lv.rand(60, 90))
    chart3.set_next_value(ser4, lv.rand(60, 90))
    chart3.set_next_value(ser4, lv.rand(60, 90))
    chart3.set_next_value(ser4, lv.rand(60, 90))
    chart3.set_next_value(ser4, lv.rand(60, 90))
    chart3.set_next_value(ser4, lv.rand(60, 90))
    chart3.set_next_value(ser4, lv.rand(60, 90))

    if disp_size == DISP_LARGE:
        grid1_col_dsc = [LV_GRID_FR(1), LV_GRID_FR(1), lv.GRID_TEMPLATE_LAST]
        grid1_row_dsc = [
            lv.GRID_CONTENT,  # Title
            lv.GRID_CONTENT,  # Sub title
            20,               # Spacer
            lv.GRID_CONTENT,  # Amount
            lv.GRID_CONTENT,  # Hint
            lv.GRID_TEMPLATE_LAST]

        chart3.set_size(lv_pct(100), lv_pct(100))
        chart3.set_style_pad_column(lv.dpx(30), 0)

        panel1.grid_dsc_array(grid1,col_dsc, grid1_row_dsc)
        title.set_grid_cell(lv.GRID_ALIGN.START, 0, 1, lv.GRID_ALIGN.START, 0, 1)
        date.set_grid_cell(lv.GRID_ALIGN.START, 0, 1, lv.GRID_ALIGN.START, 1, 1)
        amount.set_grid_cell(lv.GRID_ALIGN.START, 0, 1, lv.GRID_ALIGN.START, 3, 1)
        hint.set_grid_cell(lv.GRID_ALIGN.START, 0, 1, lv.GRID_ALIGN.START, 4, 1)
        chart3.set_grid_cell(lv.GRID_ALIGN.STRETCH, 1, 1, lv.GRID_ALIGN.STRETCH, 0, 5)
    elif disp_size == DISP_MEDIUM :
        grid1_col_dsc = [LV_GRID_FR(1), LV_GRID_FR(1), lv.GRID_TEMPLATE_LAST]
        grid1_row_dsc = [
                lv.GRID_CONTENT,  # Title + Date
                Llv.GRID_CONTENT, # Amount + Hint
                200,              # Chart
                lv.GRID_TEMPLATE_LAST ]

        panel1.update_layout()
        chart3.set_width(lv.obj.get_content_width(panel1) - 20)
        chart3.set_style_pad_column(lv.dpx(30), 0)

        panel1.set_grid_dsc_array(grid1_col_dsc, grid1_row_dsc)
        title.set_grid_cell(lv.GRID_ALIGN.START, 0, 1, lv.GRID_ALIGN.CENTER, 0, 1)
        date.set_grid_cell(lv.GRID_ALIGN.START, 1, 1, lv.GRID_ALIGN.CENTER, 0, 1)
        amount.set_grid_cell(lv.GRID_ALIGN.START, 0, 1, lv.GRID_ALIGN.CENTER, 1, 1)
        hint.set_grid_cell(lv.GRID_ALIGN.START, 1, 1, lv.GRID_ALIGN.CENTER, 1, 1)
        chart3.set_grid_cell(lv.GRID_ALIGN.END, 0, 2, lv.GRID_ALIGN.STRETCH, 2, 1)
    else:
        grid1_col_dsc = [LV_GRID_FR(1), lv.GRID_TEMPLATE_LAST]
        grid1_row_dsc = [
                lv.GRID_CONTENT,  # Title
                lv.GRID_CONTENT,  # Date
                lv.GRID_CONTENT,  # Amount
                lv.GRID_CONTENT,  # Hint
                lv.GRID_CONTENT,  # Chart
                lv.GRID_TEMPLATE_LAST]

        chart3.set_width(lv.pct(95))
        chart3.set_height(LV_VER_RES - 70)
        chart3.set_style_max_height(300, 0)
        chart3.set_zoom_x(512)
        
        panel1.set_grid_dsc_array(grid1_col_dsc, grid1_row_dsc)
        title.set_grid_cell(lv.GRID_ALIGN.START, 0, 1, lv.GRID_ALIGN.START, 0, 1)
        date.set_grid_cell(lv.GRID_ALIGN.START, 0, 1, lv.GRID_ALIGN.START, 1, 1)
        amount.set_grid_cell(lv.GRID_ALIGN.START, 0, 1, lv.GRID_ALIGN.START, 2, 1)
        hint.set_grid_cell(lv.GRID_ALIGN.START, 0, 1, lv.GRID_ALIGN.START, 3, 1)
        chart3.set_grid_cell(lv.GRID_ALIGN.END, 0, 1, lv.GRID_ALIGN.START, 4, 1)

    list = lv.obj(parent)
    if disp_size == DISP_SMALL:
        list.add_flag(LV_OBJ_FLAG_FLEX_IN_NEW_TRACK)
        list.set_height(lv.pct(100))
    else:
        list.set_height(lv.pct(100))
        list.set_style_max_height(300, 0)
                        
    list.set_flex_flow(lv.FLEX_FLOW.COLUMN)
    list.set_flex_grow(1)
    list.add_flag(LV_OBJ_FLAG_FLEX_IN_NEW_TRACK)

    title = lv.label(list)
    title.set_text("Top products")
    title.add_style(style_title, 0)
                                    
    # Create an image from the png file
    try:
        if osVersion == "unix":
            with open('assets/clothes.png','rb') as f:
                png_data = f.read()
        else:
            with open('images/clothes.png','rb') as f:
                png_data = f.read()
    except:
        print("Could not find clothes.png")
        sys.exit()
    
    clothes_argb = lv.img_dsc_t({
        'data_size': len(png_data),
        'data': png_data 
    })

    create_shop_item(list, clothes_argb, "Blue jeans", "Clothes", "$722")    
    create_shop_item(list, clothes_argb, "Blue jeans", "Clothes", "$917");
    create_shop_item(list, clothes_argb, "Blue jeans", "Clothes", "$64");
    create_shop_item(list, clothes_argb, "Blue jeans", "Clothes", "$805");

    notifications = lv.obj(parent)
    if disp_size == DISP_SMALL:
        notifications.set_height(lv.pct(100))
        notifications.add_flag(LV_OBJ_FLAG_FLEX_IN_NEW_TRACK)
    else:
        notifications.set_height(lv.pct(100))
        notifications.set_style_max_height(300, 0)

    notifications.set_flex_flow(lv.FLEX_FLOW.COLUMN)
    notifications.set_flex_grow(1)
    
    title = lv.label(notifications)
    title.set_text("Notification")
    title.add_style(style_title, 0)

    cb = lv.checkbox(notifications)
    cb.set_text("Item purchased")

    cb = lv.checkbox(notifications)
    cb.set_text("New connection")

    cb = lv.checkbox(notifications)
    cb.set_text("New subscriber")
    cb.add_state(lv.STATE.CHECKED)

    cb = lv.checkbox(notifications)
    cb.set_text("New message")
    cb.add_state(lv.STATE.DISABLED)

    cb = lv.checkbox(notifications)
    cb.set_text("Milestone reached")
    cb.add_state(lv.STATE.CHECKED | lv.STATE.DISABLED)

    cb = lv.checkbox(notifications)
    cb.set_text("Out of stock")

def create_shop_item(parent, img_src, name, category, price):
    grid_col_dsc = [lv.GRID_CONTENT, 5, LV_GRID_FR(1), LV_GRID_FR(1), lv.GRID_TEMPLATE_LAST]
    grid_row_dsc = [LV_GRID_FR(1), LV_GRID_FR(1), lv.GRID_TEMPLATE_LAST]
    
    cont = lv.obj(parent)
    cont.remove_style_all()
    cont.set_size(lv.pct(100), lv.SIZE_CONTENT)
    cont.set_grid_dsc_array(grid_col_dsc, grid_row_dsc)
    
    img = lv.img(cont)
    img.set_src(img_src)
    img.set_grid_cell(lv.GRID_ALIGN.START, 0, 1, lv.GRID_ALIGN.START, 0, 2)
    
    label = lv.label(cont)
    label.set_text(name)
    label.set_grid_cell(lv.GRID_ALIGN.START, 2, 1, lv.GRID_ALIGN.END, 0, 1)
    
    label = lv.label(cont)
    label.set_text(category)
    label.add_style(style_text_muted, 0)
    label.set_grid_cell(lv.GRID_ALIGN.START, 2, 1, lv.GRID_ALIGN.START, 1, 1)
    
    label = lv.label(cont)
    label.set_text(price)
    label.set_grid_cell(lv.GRID_ALIGN.END, 3, 1, lv.GRID_ALIGN.END, 0, 1)
    
    return cont



# Check on which system we are running 
try:
    osVersion=os.uname()[0]
except:
    osVersion="unix"

print("System name: ",osVersion)

# Initialize the display driver
driver = display_driver_utils.driver(width=320,height=240,orientation=display_driver_utils.ORIENT_LANDSCAPE)

    
# Create a screen and load it
scr=lv.obj()
lv.scr_load(scr)

# needed for dynamic font loading
fs_drv = lv.fs_drv_t()
fs_driver.fs_register(fs_drv, 'S')

# Register PNG image decoder
decoder = lv.img.decoder_create()
decoder.info_cb = get_png_info
decoder.open_cb = open_png

# get the display size
LV_HOR_RES = lv.scr_act().get_disp().driver.hor_res
LV_VER_RES = lv.scr_act().get_disp().driver.ver_res

print("Screen resolution: {:d}x{:d}".format(LV_HOR_RES,LV_VER_RES))

DISP_SMALL  = 0
DISP_MEDIUM = 1
DISP_LARGE  = 2

if LV_HOR_RES <= 320:
    disp_size = DISP_SMALL
elif LV_HOR_RES < 720:
    disp_size = DISP_MEDIUM
else:
    disp_size = DISP_LARGE;

if disp_size == DISP_LARGE:
    tab_h = 70;
    try:
        font_large = lv.font_montserrat_24
    except:
        print("Dynamically loading font_montserrat_24")
        font_large = lv.font_load("S:assets/font/montserrat-24.fnt")
        
    try:
        font_normal = lv.font_montserrat_16
    except:
        print("Dynamically loading font_montserrat_16")
        font_normal = lv.font_load("S:assets/font/montserrat-16.fnt")

elif disp_size == DISP_MEDIUM:
    tab_h = 45
    try:
        font_large = lv.font_montserrat_20
    except:
        print("Dynamically loading font_montserrat_20")
        font_large = lv.font_load("S:assets/font/montserrat-20.fnt")
            
    try:
        font_normal = lv.font_montserrat_14
    except:
        print("Dynamically loading font_montserrat_14")
        font_normal = lv.font_load("S:assets/font/montserrat-14.fnt")

else:  # disp_size == DISP_SMALL 
    tab_h = 45
    try:
        font_large = lv.font_montserrat_18
    except:
        print("Dynamically loading font_montserrat_18")
        if osVersion == "esp32":
            font_large = lv.font_load("S:/font/montserrat-18.fnt")
        else:
            font_large = lv.font_load("S:assets/font/montserrat-18.fnt")
        
    try:
        font_normal = lv.font_montserrat_12
    except:
        print("Dynamically loading font_montserrat_12")
        if osVersion == "esp32":        
            font_normal = lv.font_load("S:/font/montserrat-12.fnt")            
        else:
            font_normal = lv.font_load("S:assets/font/montserrat-12.fnt")            

if not font_large or not font_normal:
    print("font loading failed")
    
lv.theme_default_init(None,lv.palette_main(lv.PALETTE.BLUE),lv.palette_main(lv.PALETTE.RED),
                      LV_THEME_DEFAULT_DARK,font_normal)

style_text_muted = lv.style_t()
style_text_muted.init()
style_text_muted.set_text_opa(lv.OPA._50)

style_title = lv.style_t()
style_title.init()
style_title.set_text_font(font_large)

style_icon = lv.style_t()
style_icon.init()
style_icon.set_text_color(lv.theme_get_color_primary(None))
style_icon.set_text_font(font_large)

style_bullet = lv.style_t()
style_bullet.init()
style_bullet.set_border_width(0)
style_bullet.set_radius(lv.RADIUS_CIRCLE)

tv = lv.tabview(lv.scr_act(), lv.DIR.TOP, tab_h)

lv.scr_act().set_style_text_font(font_normal, 0)

if disp_size == DISP_LARGE:
    tab_btns = tv.get_tab_btns()
    tab_btns.set_style_pad_left(LV_HOR_RES / 2, 0)
    # logo = lv_img_create(tab_btns);
    #    LV_IMG_DECLARE(img_lvgl_logo);
    #    lv_img_set_src(logo, &img_lvgl_logo);
    #    lv_obj_align(logo, LV_ALIGN_LEFT_MID, -LV_HOR_RES / 2 + 25, 0);

    label = lv.label(tab_btns)
    label.add_style(style_title, 0)
    label.set_text("LVGL v8")
    label.align_to(logo, lv.ALIGN.OUT_RIGHT_TOP, 10, 0)

    label = lv.label(tab_btns)
    label.set_text("Widgets demo")
    label.add_style(style_text_muted, 0)
    label.align_to(logo, lv.ALIGN.OUT_RIGHT_BOTTOM, 10, 0)
    
def color_changer_create(parent):
    palette = [
            lv.PALETTE.BLUE, lv.PALETTE.GREEN, lv.PALETTE.BLUE_GREY,  lv.PALETTE.ORANGE,
            lv.PALETTE.RED, lv.PALETTE.PURPLE, lv.PALETTE.TEAL]

    color_cont = lv.obj(parent)
    color_cont.remove_style_all()
    color_cont.set_flex_flow(lv.FLEX_FLOW.ROW)
    color_cont.set_flex_align(lv.FLEX_ALIGN.SPACE_EVENLY, lv.FLEX_ALIGN.CENTER, lv.FLEX_ALIGN.CENTER)
    color_cont.add_flag(lv.obj.FLAG.FLOATING)

    color_cont.set_style_bg_color(lv.color_white(), 0)
    if disp_size == DISP_SMALL:
        color_cont.set_style_pad_right(lv.dpx(47),0)
    else :
        color_cont.set_style_pad_right(lv.dpx(55), 0)
        
    color_cont.set_style_bg_opa(lv.OPA.COVER, 0)
    color_cont.set_style_radius(lv.RADIUS_CIRCLE, 0)

    if disp_size == DISP_SMALL:
        color_cont.set_size(lv.dpx(52), lv.dpx(52));
    else:
        color_cont.set_size(lv.dpx(60), lv.dpx(60))

    color_cont.align(lv.ALIGN.BOTTOM_RIGHT, - lv.dpx(10),  - lv.dpx(10))

    for i in palette:
    # for(i = 0; palette[i] != _LV_PALETTE_LAST; i++) {
        c = lv.btn(color_cont)
        c.set_style_bg_color(lv.palette_main(i), 0);
        c.set_style_radius(lv.RADIUS_CIRCLE, 0);
        c.set_style_opa(lv.OPA.TRANSP, 0)
        c.set_size(20, 20)
        # c.add_event_cb(c, color_event_cb, LV_EVENT_ALL, &palette[i])
        c.clear_flag(lv.obj.FLAG.SCROLL_ON_FOCUS)
        
    btn = lv.btn(parent)
    btn.add_flag(lv.obj.FLAG.FLOATING | lv.obj.FLAG.CLICKABLE)
    btn.set_style_bg_color(lv.color_white(), lv.STATE.CHECKED)
    btn.set_style_pad_all(10, 0)
    btn.set_style_radius(lv.RADIUS_CIRCLE, 0)
    # btn.add_event_cb(color_changer_event_cb, lv.EVENT-ALL, color_cont)
    btn.set_style_shadow_width(0, 0)
    btn.set_style_bg_img_src(lv.SYMBOL.NEW_LINE, 0)

    if disp_size == DISP_SMALL:
         btn.set_size(lv.dpx(42), lv.dpx(42))
         btn.align(lv.ALIGN.BOTTOM_RIGHT, -lv.dpx(15), -lv.dpx(15))
    else :
         btn.set_size(lv.dpx(50), lv.dpv(50))
         btn.align(lv.ALIGN.BOTTOM_RIGHT, -lv.dpx(15), -lv.dpx(15))

def meter1_indic1_anim_cb(meter,indic,val):
    
    meter.set_indicator_end_value(indic, val)    

    card = lv.obj.get_parent(lv.obj.__cast__(meter))
    label = lv.obj.get_child(lv.obj.__cast__(card), -5)
    label.set_text("Revenue : {:d}%".format(val))
    
def meter1_indic2_anim_cb(meter,indic,val) :

    meter.set_indicator_end_value(indic, val)

    card = lv.obj.get_parent(lv.obj.__cast__(meter))
    label = lv.obj.get_child(lv.obj.__cast__(card), -3)
    label.set_text("Sales : {:d}%".format(val))

def meter1_indic3_anim_cb(meter, indic, val) :
    
    meter.set_indicator_end_value(indic, val);
    
    card = lv.obj.get_parent(lv.obj.__cast__(meter))
    label = lv.obj.get_child(lv.obj.__cast__(card), -1)
    label.set_text("Cost : {:d}%".format(val))


def meter2_timer_cb(timer,meter,indics) :

    global session_desktop,session_tablet,session_mobile
    global down1,down2,down3
    
    if down1 : 
        session_desktop -= 137;
        if session_desktop < 1400:
            down1 = False
        else :
            session_desktop += 116;
        if session_desktop > 4500:
            down1 = True

    if down2:
        session_tablet -= 3;
        if session_tablet < 1400 :
            down2 = False
    else :
        session_tablet += 9;
        if session_tablet > 450:
            down2 = True

    if down3 :
        session_mobile -= 57
        if session_mobile < 1400 :
            down3 = False
    else :
        session_mobile += 76
        if session_mobile > 4500:
            down3 = True

    all = session_desktop + session_tablet + session_mobile;
    pct1 = (session_desktop * 97) // all
    pct2 = (session_tablet * 97) // all

    meter.set_indicator_start_value(indics[0], 0)
    meter.set_indicator_end_value(indics[0], pct1)

    meter.set_indicator_start_value(indics[1], pct1 + 1)
    meter.set_indicator_end_value(indics[1], pct1 + 1 + pct2)

    meter.set_indicator_start_value(indics[2], pct1 + 1 + pct2 + 1)
    meter.set_indicator_end_value(indics[2], 99)

    card = lv.obj.get_parent(lv.obj.__cast__(meter))

    label = lv.obj.get_child(lv.obj.__cast__(card), -5)
    label.set_text("Desktop : {:d}".format(session_desktop))
    # lv_label_set_text_fmt(label, "Desktop: %"LV_PRIu32, session_desktop);

    label = lv.obj.get_child(lv.obj.__cast__(card), -3)
    label.set_text("Tablet : {:d}".format(session_desktop))                   
    # lv_label_set_text_fmt(label, "Tablet: %"LV_PRIu32, session_tablet);

    label = lv.obj.get_child(lv.obj.__cast__(card), -1)
    label.set_text("Tablet : {:d}".format(session_desktop))                  
    #lv_label_set_text_fmt(label, "Mobile: %"LV_PRIu32, session_mobile);


def meter3_anim_cb(meter,indic, label, val) :

    meter.set_indicator_value(indic, val)

    # label = lv.obj.get_child(lv.obj.__cast__(meter), -0)
    label.set_text(str(val) + "Mbps")

def ta_event_cb(e,kb,tv) :

    code = e.get_code()
    ta = e.get_target()
    # lv_obj_t * kb = lv_event_get_user_data(e);
    
    indev = lv.indev_get_act()
    if code == lv.EVENT.FOCUSED :
        if indev.get_type() != lv.INDEV_TYPE.KEYPAD :
            kb.set_textarea(ta)
            kb.set_style_max_height(LV_HOR_RES * 2 // 3, 0)
            tv.update_layout()   # Be sure the sizes are recalculated
            tv.set_height(LV_VER_RES - kb.get_height())
            kb.clear_flag(lv.obj.FLAG.HIDDEN)
            ta.scroll_to_view_recursive(lv.ANIM.OFF)
 
    elif code == lv.EVENT.DEFOCUSED :
        kb.set_textarea(None)
        tv.set_height(LV_VER_RES)
        kb.add_flag(lv.obj.FLAG.HIDDEN)
        indev.reset(None)

    elif code == lv.EVENT.READY or code == lv.EVENT.CANCEL:
        tv.set_height(LV_VER_RES)
        kb.add_flag(lv.obj.FLAG.HIDDEN)
        ta.clear_state(lv.STATE.FOCUSED)
        indev.reset(None)    # To forget the last clicked object to make it focusable again

def birthday_event_cb(e):
    global calendar
    code = e.get_code()
    ta = e.get_target()

    if code == lv.EVENT.FOCUSED :
        indev = lv.indev_get_act()
        if indev.get_type() == lv.INDEV_TYPE.POINTER:
            if calendar == None:
                top_layer = lv.layer_top()
                top_layer.add_flag(lv.obj.FLAG.CLICKABLE)
                calendar = lv.calendar(top_layer)
                top_layer.set_style_bg_opa(lv.OPA._50, 0)
                top_layer.set_style_bg_color(lv.palette_main(lv.PALETTE.GREY), 0)
                if disp_size == DISP_SMALL:
                    calendar.set_size(180, 200)
                elif disp_size == DISP_MEDIUM:
                    calendar.set_size(200, 220)
                else :
                    calendar.set_size(300, 330) 
                calendar.set_showed_date(1990, 01)
                calendar.align(lv.ALIGN.CENTER, 0, 30)
                # password.add_event_cb(lambda e: ta_event_cb(e,kb,tv),lv.EVENT.ALL,None)
              
                calendar.add_event_cb(lambda e: calendar_event_cb(e,ta), lv.EVENT.ALL, None)

                header = lv.calendar_header_dropdown(calendar)

def calendar_event_cb(e,ta) :
    global calendar
    code = e.get_code();
    obj = lv.calendar.__cast__(e.get_current_target())
    if code == lv.EVENT.VALUE_CHANGED:
        d = lv.calendar_date_t()
        obj.get_pressed_date(d)
        ta.set_text("{:02d}.{:02d}.{:d}".format(d.day,d.month,d.year))

        calendar.delete()
        calendar = None
        lv.layer_top().clear_flag(lv.obj.FLAG.CLICKABLE)
        lv.layer_top().set_style_bg_opa(lv.OPA.TRANSP, 0)
   
t1 = tv.add_tab("Profile")
t2 = tv.add_tab("Analytics")
t3 = tv.add_tab("Shop")

profile_create(t1)
analytics_create(t2)
shop_create(t3)

color_changer_create(tv)

while True:
    lv.timer_handler()
    time.sleep(1)
