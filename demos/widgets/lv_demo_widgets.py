#!/opt/bin/lv_micropython -i
import lvgl as lv
import display_driver_utils
import fs_driver
import time,os,sys

SCREEN_SIZE_SMALL  = (320,240)
SCREEN_SIZE_MEDIUM = (480,320)
SCREEN_SIZE_LARGE  = (800,600)
SCREEN_SIZE        = SCREEN_SIZE_MEDIUM

LV_THEME_DEFAULT_DARK = 0
LV_OBJ_FLAG_FLEX_IN_NEW_TRACK = 1<<23
LV_USE_DRAW_MASKS = True

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
        if osVersion == "esp32":
            with open('images/avatar.png','rb') as f:
                png_data = f.read()
        else:
            with open(script_path + '/assets/avatar.png','rb') as f:
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
    user_name.add_event_cb(lambda e: ta_event_cb(e,kb,tv),lv.EVENT.ALL,None)
                        
    password_label = lv.label(panel2)
    password_label.set_text("Password")
    password_label.add_style(style_text_muted, 0)

    password = lv.textarea(panel2)
    password.set_one_line(True)
    password.set_password_mode(True)
    password.set_placeholder_text("Min. 8 chars.")
    password.add_event_cb(lambda e: ta_event_cb(e,kb,tv),lv.EVENT.ALL,None)

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
    experience_label.set_text("Experience")
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
    hard_working_label.set_text("Hard-working")
    hard_working_label.add_style(style_text_muted, 0)

    sw2 = lv.switch(panel3)
    
    if disp_size == DISP_LARGE:
        grid_main_col_dsc = [LV_GRID_FR(1), LV_GRID_FR(1), lv.GRID_TEMPLATE_LAST]
        grid_main_row_dsc = [lv.GRID_CONTENT, lv.GRID_CONTENT, lv.GRID_TEMPLATE_LAST]

        # Create the top panel
        grid_1_col_dsc = [lv.GRID_CONTENT, 5, lv.GRID_CONTENT, LV_GRID_FR(2), LV_GRID_FR(1), LV_GRID_FR(1), lv.GRID_TEMPLATE_LAST]
        grid_1_row_dsc = [lv.GRID_CONTENT, lv.GRID_CONTENT, 10, lv.GRID_CONTENT, lv.GRID_CONTENT, lv.GRID_TEMPLATE_LAST]

        grid_2_col_dsc = [LV_GRID_FR(1), LV_GRID_FR(1), lv.GRID_TEMPLATE_LAST]
        grid_2_row_dsc = [
            lv.GRID_CONTENT,  # Title
            5,                # Separator
            lv.GRID_CONTENT,  # Box title
            30,               # Boxes
            5,                # Separator
            lv.GRID_CONTENT,  # Box title
            30,               # Boxes
            lv.GRID_TEMPLATE_LAST]


        parent.set_grid_dsc_array(grid_main_col_dsc, grid_main_row_dsc)

        panel1.set_grid_cell(lv.GRID_ALIGN.STRETCH, 0, 2, lv.GRID_ALIGN.CENTER, 0, 1)

        panel1.set_grid_dsc_array(grid_1_col_dsc, grid_1_row_dsc)
        avatar.set_grid_cell(lv.GRID_ALIGN.CENTER, 0, 1, lv.GRID_ALIGN.CENTER, 0, 5)
        name.set_grid_cell(lv.GRID_ALIGN.START, 2, 2, lv.GRID_ALIGN.CENTER, 0, 1)
        dsc.set_grid_cell(lv.GRID_ALIGN.STRETCH, 2, 4, lv.GRID_ALIGN.START, 1, 1)
        email_icn.set_grid_cell(lv.GRID_ALIGN.CENTER, 2, 1, lv.GRID_ALIGN.CENTER, 3, 1)
        email_label.set_grid_cell(lv.GRID_ALIGN.START, 3, 1, lv.GRID_ALIGN.CENTER, 3, 1)
        call_icn.set_grid_cell(lv.GRID_ALIGN.CENTER, 2, 1, lv.GRID_ALIGN.CENTER, 4, 1)
        call_label.set_grid_cell(lv.GRID_ALIGN.START, 3, 1, lv.GRID_ALIGN.CENTER, 4, 1)
        log_out_btn.set_grid_cell(lv.GRID_ALIGN.STRETCH, 4, 1, lv.GRID_ALIGN.CENTER, 3, 2)
        invite_btn.set_grid_cell(lv.GRID_ALIGN.STRETCH, 5, 1, lv.GRID_ALIGN.CENTER, 3, 2)

        panel2.set_grid_cell(lv.GRID_ALIGN.STRETCH, 0, 1, lv.GRID_ALIGN.START, 1, 1)
        panel2.set_grid_dsc_array(grid_2_col_dsc, grid_2_row_dsc)
        panel2_title.set_grid_cell(lv.GRID_ALIGN.START, 0, 2, lv.GRID_ALIGN.CENTER, 0, 1)
        user_name.set_grid_cell(lv.GRID_ALIGN.STRETCH, 0, 1, lv.GRID_ALIGN.CENTER, 3, 1)
        user_name_label.set_grid_cell(lv.GRID_ALIGN.START, 0, 1, lv.GRID_ALIGN.START, 2, 1)
        password.set_grid_cell(lv.GRID_ALIGN.STRETCH, 1, 1, lv.GRID_ALIGN.CENTER, 3, 1)
        password_label.set_grid_cell(lv.GRID_ALIGN.START, 1, 1, lv.GRID_ALIGN.START, 2, 1)
        birthdate.set_grid_cell(lv.GRID_ALIGN.STRETCH, 1, 1, lv.GRID_ALIGN.CENTER, 6, 1)
        birthday_label.set_grid_cell(lv.GRID_ALIGN.START, 1, 1, lv.GRID_ALIGN.START, 5, 1)
        gender.set_grid_cell(lv.GRID_ALIGN.STRETCH, 0, 1, lv.GRID_ALIGN.CENTER, 6, 1)
        gender_label.set_grid_cell(lv.GRID_ALIGN.START, 0, 1, lv.GRID_ALIGN.START, 5, 1)

        panel3.set_grid_cell(lv.GRID_ALIGN.STRETCH, 1, 1, lv.GRID_ALIGN.STRETCH, 1, 1)
        panel3.set_grid_dsc_array(grid_2_col_dsc, grid_2_row_dsc)
        panel3_title.set_grid_cell(lv.GRID_ALIGN.START, 0, 2, lv.GRID_ALIGN.CENTER, 0, 1)
        slider1.set_grid_cell(lv.GRID_ALIGN.CENTER, 0, 2, lv.GRID_ALIGN.CENTER, 3, 1)
        experience_label.set_grid_cell(lv.GRID_ALIGN.START, 0, 1, lv.GRID_ALIGN.START, 2, 1)
        sw2.set_grid_cell(lv.GRID_ALIGN.START, 1, 1, lv.GRID_ALIGN.CENTER, 6, 1)
        hard_working_label.set_grid_cell(lv.GRID_ALIGN.START, 0, 1, lv.GRID_ALIGN.START, 5, 1)
        sw1.set_grid_cell(lv.GRID_ALIGN.START, 0, 1, lv.GRID_ALIGN.CENTER, 6, 1)
        team_player_label.set_grid_cell(lv.GRID_ALIGN.START, 1, 1, lv.GRID_ALIGN.START, 5, 1)
        
    elif disp_size == DISP_MEDIUM :
        grid_main_col_dsc = [LV_GRID_FR(1), LV_GRID_FR(1), lv.GRID_TEMPLATE_LAST]
        grid_main_row_dsc = [lv.GRID_CONTENT, lv.GRID_CONTENT, lv.GRID_TEMPLATE_LAST]


        # Create the top panel
        grid_1_col_dsc = [lv.GRID_CONTENT, 1, lv.GRID_CONTENT, LV_GRID_FR(1), lv.GRID_TEMPLATE_LAST]
        grid_1_row_dsc = [
            lv.GRID_CONTENT, # Name
            lv.GRID_CONTENT, # Description
            lv.GRID_CONTENT, # Email
            -20,
            lv.GRID_CONTENT, # Phone
            lv.GRID_CONTENT, # Buttons
            lv.GRID_TEMPLATE_LAST]

        grid_2_col_dsc= [LV_GRID_FR(1), LV_GRID_FR(1), lv.GRID_TEMPLATE_LAST]
        grid_2_row_dsc = [
            lv.GRID_CONTENT,  # Title
            5,                # Separator
            lv.GRID_CONTENT,  # Box title
            40,               # Box
            lv.GRID_CONTENT,  # Box title
            40,               # Box
            lv.GRID_CONTENT,  # Box title
            40,               # Box
            lv.GRID_CONTENT,  # Box title
            40,               # Box
            lv.GRID_TEMPLATE_LAST]


        parent.set_grid_dsc_array(grid_main_col_dsc, grid_main_row_dsc)
        panel1.set_grid_cell(lv.GRID_ALIGN.STRETCH, 0, 2, lv.GRID_ALIGN.CENTER, 0, 1)

        log_out_btn.set_width(120)
        invite_btn.set_width(120)

        panel1.set_grid_dsc_array(grid_1_col_dsc, grid_1_row_dsc)
        avatar.set_grid_cell(lv.GRID_ALIGN.CENTER, 0, 1, lv.GRID_ALIGN.START, 0, 4)
        name.set_grid_cell(lv.GRID_ALIGN.START, 2, 2, lv.GRID_ALIGN.CENTER, 0, 1)
        dsc.set_grid_cell(lv.GRID_ALIGN.STRETCH, 2, 2, lv.GRID_ALIGN.START, 1, 1)
        email_label.set_grid_cell(lv.GRID_ALIGN.START, 3, 1, lv.GRID_ALIGN.CENTER, 2, 1)
        email_icn.set_grid_cell(lv.GRID_ALIGN.CENTER, 2, 1, lv.GRID_ALIGN.CENTER, 2, 1)
        call_icn.set_grid_cell(lv.GRID_ALIGN.CENTER, 2, 1, lv.GRID_ALIGN.CENTER, 4, 1)
        call_label.set_grid_cell(lv.GRID_ALIGN.START, 3, 1, lv.GRID_ALIGN.CENTER, 4, 1)
        log_out_btn.set_grid_cell(lv.GRID_ALIGN.START, 1, 1, lv.GRID_ALIGN.CENTER, 5, 1)
        invite_btn.set_grid_cell(lv.GRID_ALIGN.END, 3, 1, lv.GRID_ALIGN.CENTER, 5, 1)

        panel2.set_grid_cell(lv.GRID_ALIGN.STRETCH, 0, 1, lv.GRID_ALIGN.START, 1, 1)
        panel2.set_grid_dsc_array(grid_2_col_dsc, grid_2_row_dsc)
        panel2_title.set_grid_cell(lv.GRID_ALIGN.START, 0, 2, lv.GRID_ALIGN.CENTER, 0, 1)
        user_name_label.set_grid_cell(lv.GRID_ALIGN.START, 0, 2, lv.GRID_ALIGN.START, 2, 1)
        user_name.set_grid_cell(lv.GRID_ALIGN.STRETCH, 0, 2, lv.GRID_ALIGN.START, 3, 1)
        password_label.set_grid_cell(lv.GRID_ALIGN.START, 0, 2, lv.GRID_ALIGN.START, 4, 1)
        password.set_grid_cell(lv.GRID_ALIGN.STRETCH, 0, 2, lv.GRID_ALIGN.START, 5, 1)
        birthday_label.set_grid_cell(lv.GRID_ALIGN.START, 0, 2, lv.GRID_ALIGN.START, 6, 1)
        birthdate.set_grid_cell(lv.GRID_ALIGN.STRETCH, 0, 2, lv.GRID_ALIGN.START, 7, 1)
        gender_label.set_grid_cell(lv.GRID_ALIGN.START, 0, 2, lv.GRID_ALIGN.START, 8, 1)
        gender.set_grid_cell(lv.GRID_ALIGN.STRETCH, 0, 2, lv.GRID_ALIGN.START, 9, 1)

        panel3.set_grid_cell(lv.GRID_ALIGN.STRETCH, 1, 1, lv.GRID_ALIGN.STRETCH, 1, 1)
        panel3.set_grid_dsc_array(grid_2_col_dsc, grid_2_row_dsc)
        panel3_title.set_grid_cell(lv.GRID_ALIGN.START, 0, 2, lv.GRID_ALIGN.CENTER, 0, 1)
        slider1.set_grid_cell(lv.GRID_ALIGN.CENTER, 0, 2, lv.GRID_ALIGN.CENTER, 3, 1)
        experience_label.set_grid_cell(lv.GRID_ALIGN.START, 0, 1, lv.GRID_ALIGN.START, 2, 1)
        hard_working_label.set_grid_cell(lv.GRID_ALIGN.START, 0, 1, lv.GRID_ALIGN.START, 4, 1)
        sw2.set_grid_cell(lv.GRID_ALIGN.START, 0, 1, lv.GRID_ALIGN.START, 5, 1)
        team_player_label.set_grid_cell(lv.GRID_ALIGN.START, 0, 1, lv.GRID_ALIGN.START, 6, 1)
        sw1.set_grid_cell(lv.GRID_ALIGN.START, 0, 1, lv.GRID_ALIGN.START, 7, 1)

    else :        
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

        panel1.set_grid_dsc_array(grid_1_col_dsc, grid_1_row_dsc)


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
        gender_label.set_grid_cell(lv.GRID_ALIGN.START, 0, 2, lv.GRID_ALIGN.START, 8, 1)
        gender.set_grid_cell(lv.GRID_ALIGN.STRETCH, 0, 2, lv.GRID_ALIGN.START, 9, 1)

        panel3.set_height(lv.SIZE_CONTENT)
        panel3.set_grid_cell(lv.GRID_ALIGN.STRETCH, 0, 1, lv.GRID_ALIGN.START, 2, 1)
        panel3_title.set_grid_cell(lv.GRID_ALIGN.START, 0, 2, lv.GRID_ALIGN.CENTER, 0, 1)
        experience_label.set_grid_cell(lv.GRID_ALIGN.START, 0, 1, lv.GRID_ALIGN.START, 2, 1)
        slider1.set_grid_cell(lv.GRID_ALIGN.CENTER, 0, 2, lv.GRID_ALIGN.CENTER, 3, 1)
        hard_working_label.set_grid_cell(lv.GRID_ALIGN.START, 0, 1, lv.GRID_ALIGN.START, 4, 1)
        sw1.set_grid_cell(lv.GRID_ALIGN.START, 0, 1, lv.GRID_ALIGN.START, 5, 1)
        team_player_label.set_grid_cell(lv.GRID_ALIGN.START, 1, 1, lv.GRID_ALIGN.START, 4, 1)
        sw2.set_grid_cell(lv.GRID_ALIGN.START, 1, 1, lv.GRID_ALIGN.START, 5, 1)
        
def analytics_create(parent):
    parent.set_flex_flow(lv.FLEX_FLOW.ROW_WRAP)

    grid_chart_row_dsc = [lv.GRID_CONTENT, LV_GRID_FR(1), 10, lv.GRID_TEMPLATE_LAST]
    grid_chart_col_dsc = [20, LV_GRID_FR(1), lv.GRID_TEMPLATE_LAST]

    chart1_cont = lv.obj(parent)
    chart1_cont.set_flex_grow(1)
    chart1_cont.set_grid_dsc_array(grid_chart_col_dsc, grid_chart_row_dsc)

    chart1_cont.set_height(lv.pct(100))
    chart1_cont.set_style_max_height(300, 0)

    title = lv.label(chart1_cont)
    title.set_text("Unique visitors")
    title.add_style(style_title, 0)
    title.set_grid_cell(lv.GRID_ALIGN.START, 0, 2, lv.GRID_ALIGN.START, 0, 1)

    chart1 = lv.chart(chart1_cont)
    lv.group_get_default().add_obj(chart1)
    chart1.add_flag(lv.obj.FLAG.SCROLL_ON_FOCUS)
    chart1.set_grid_cell(lv.GRID_ALIGN.STRETCH, 1, 1, lv.GRID_ALIGN.STRETCH, 1, 1)
    chart1.set_axis_tick(lv.chart.AXIS.PRIMARY_Y, 0, 0, 5, 1, True, 80)
    chart1.set_axis_tick(lv.chart.AXIS.PRIMARY_X, 0, 0, 12, 1, True, 50)
    chart1.set_div_line_count(0, 12)
    chart1.set_point_count(12)
    chart1.add_event_cb(chart_event_cb, lv.EVENT.ALL, None)
    if disp_size == DISP_SMALL:
        chart1.set_zoom_x(256 * 3)
    elif disp_size == DISP_MEDIUM:
        chart1.set_zoom_x(256 * 2)

    chart1.set_style_border_side(lv.BORDER_SIDE.LEFT | lv.BORDER_SIDE.BOTTOM, 0)
    chart1.set_style_radius(0, 0)

    ser1 = chart1.add_series(lv.theme_get_color_primary(chart1), lv.chart.AXIS.PRIMARY_Y)
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
    lv.group_get_default().add_obj(chart2)
    chart2.add_flag(lv.obj.FLAG.SCROLL_ON_FOCUS)

    chart2.set_grid_cell(lv.GRID_ALIGN.STRETCH, 1, 1, lv.GRID_ALIGN.STRETCH, 1, 1)
    chart2.set_axis_tick(lv.chart.AXIS.PRIMARY_Y, 0, 0, 5, 1, True, 80)
    chart2.set_axis_tick(lv.chart.AXIS.PRIMARY_X, 0, 0, 12, 1, True, 50)
    chart2.set_size(lv.pct(100), lv.pct(100))
    chart2.set_type(lv.chart.TYPE.BAR)
    chart2.set_div_line_count(6, 0)
    chart2.set_point_count(12)
    chart2.add_event_cb(chart_event_cb, lv.EVENT.ALL, None)
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
    
    meter1 = create_meter_box(parent, "Monthly Target", "Revenue: 63%", "Sales: 44%", "Costs: 58%")
    lv.obj.get_parent(lv.obj.__cast__(meter1)).add_flag(LV_OBJ_FLAG_FLEX_IN_NEW_TRACK)
    scale = meter1.add_scale()
    meter1.set_scale_range(scale, 0, 100, 270, 90)
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
    meter3.set_style_outline_color(lv.color_white(), lv.PART.INDICATOR)
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
    label1.set_text(text1)

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

        cont.set_grid_dsc_array(grid_col_dsc, grid_row_dsc)
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
    chart3.add_event_cb(shop_chart_event_cb, lv.EVENT.ALL, None)

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

        chart3.set_size(lv.pct(100), lv.pct(100))
        chart3.set_style_pad_column(lv.dpx(30), 0)

        panel1.set_grid_dsc_array(grid1_col_dsc, grid1_row_dsc)
        title.set_grid_cell(lv.GRID_ALIGN.START, 0, 1, lv.GRID_ALIGN.START, 0, 1)
        date.set_grid_cell(lv.GRID_ALIGN.START, 0, 1, lv.GRID_ALIGN.START, 1, 1)
        amount.set_grid_cell(lv.GRID_ALIGN.START, 0, 1, lv.GRID_ALIGN.START, 3, 1)
        hint.set_grid_cell(lv.GRID_ALIGN.START, 0, 1, lv.GRID_ALIGN.START, 4, 1)
        chart3.set_grid_cell(lv.GRID_ALIGN.STRETCH, 1, 1, lv.GRID_ALIGN.STRETCH, 0, 5)
        
    elif disp_size == DISP_MEDIUM :
        grid1_col_dsc = [LV_GRID_FR(1), LV_GRID_FR(1), lv.GRID_TEMPLATE_LAST]
        grid1_row_dsc = [
                lv.GRID_CONTENT,  # Title + Date
                lv.GRID_CONTENT,  # Amount + Hint
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
        if osVersion == "esp32":
            with open('images/clothes.png','rb') as f:
                png_data = f.read()
        else:
            with open(script_path + '/assets/clothes.png','rb') as f:
                png_data = f.read()
    except:
        print("Could not find clothes.png")
        sys.exit()
    
    clothes_argb = lv.img_dsc_t({
        'data_size': len(png_data),
        'data': png_data 
    })

    create_shop_item(list, clothes_argb, "Blue jeans", "Clothes", "$722")    
    create_shop_item(list, clothes_argb, "Blue jeans", "Clothes", "$917")
    create_shop_item(list, clothes_argb, "Blue jeans", "Clothes", "$64")
    create_shop_item(list, clothes_argb, "Blue jeans", "Clothes", "$805")

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

# get the directory in which the script is running
try:
    script_path = __file__[:__file__.rfind('/')] if __file__.find('/') >= 0 else '.'
except NameError: 
    script_path = ''

# Initialize the display driver
driver = display_driver_utils.driver(width=SCREEN_SIZE[0],height=SCREEN_SIZE[1],orientation=display_driver_utils.ORIENT_LANDSCAPE)
    
# Create a screen and load it
scr=lv.obj()
lv.scr_load(scr)

# needed for dynamic font loading
fs_drv = lv.fs_drv_t()
fs_driver.fs_register(fs_drv, 'S')

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
    disp_size = DISP_LARGE

if disp_size == DISP_LARGE:
    tab_h = 70
    try:
        font_large = lv.font_montserrat_24
    except:
        print("Dynamically loading font_montserrat_24")
        if osVersion == "esp32":
            font_large = lv.font_load("S:font/montserrat-24.fnt")
        else:
            font_large = lv.font_load("S:" + script_path + "/assets/font/montserrat-24.fnt")
        
    try:
        font_normal = lv.font_montserrat_16
    except:
        print("Dynamically loading font_montserrat_16")
        if osVersion == "esp32":
            font_normal = lv.font_load("S:/font/montserrat-16.fnt")
        else :
            font_normal = lv.font_load("S:" + script_path + "/assets/font/montserrat-16.fnt")

elif disp_size == DISP_MEDIUM:
    tab_h = 45
    try:
        font_large = lv.font_montserrat_20
    except:
        print("Dynamically loading font_montserrat_20")
        if osVersion == "esp32":
            font_large = lv.font_load("S:/font/montserrat-20.fnt")
        else:
            font_large = lv.font_load("S:" + script_path + "/assets/font/montserrat-20.fnt")
            
    try:
        font_normal = lv.font_montserrat_14
    except:
        print("Dynamically loading font_montserrat_14")
        if osVersion == "esp32":
            font_normal = lv.font_load("S:/font/montserrat-14.fnt")
        else:
            font_normal = lv.font_load("S:" + script_path + "/assets/font/montserrat-14.fnt")

else:  # disp_size == DISP_SMALL 
    tab_h = 45
    try:
        font_large = lv.font_montserrat_18
    except:
        print("Dynamically loading font_montserrat_18")
        if osVersion == "esp32":
            font_large = lv.font_load("S:/font/montserrat-18.fnt")
        else:
            font_large = lv.font_load("S:" + script_path + "/assets/font/montserrat-18.fnt")
        
    try:
        font_normal = lv.font_montserrat_12
    except:
        print("Dynamically loading font_montserrat_12")
        if osVersion == "esp32":        
            font_normal = lv.font_load("S:/font/montserrat-12.fnt")            
        else:
            font_normal = lv.font_load("S:" + script_path + "/assets/font/montserrat-12.fnt")            

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
    tab_btns.set_style_pad_left(LV_HOR_RES // 2, 0)

    # Create an image from the png file
    try:
        if osVersion == "esp32":
            with open('images/lvgl_logo.png','rb') as f:
                png_data = f.read()
        else:
            with open(script_path + '/assets/lvgl_logo.png','rb') as f:
                png_data = f.read()
    except:
        print("Could not find lvgl_logo.png")
        sys.exit()
    
    lvgl_logo_argb = lv.img_dsc_t({
        'data_size': len(png_data),
        'data': png_data 
    })

    logo = lv.img(tab_btns)

    logo.set_src(lvgl_logo_argb)
    logo.align(lv.ALIGN.LEFT_MID, -LV_HOR_RES // 2 + 25, 0)

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
        color_cont.set_size(lv.dpx(52), lv.dpx(52))
    else:
        color_cont.set_size(lv.dpx(60), lv.dpx(60))

    color_cont.align(lv.ALIGN.BOTTOM_RIGHT, - lv.dpx(10),  - lv.dpx(10))

    for i in palette:
    # for(i = 0; palette[i] != _LV_PALETTE_LAST; i++) {
        c = lv.btn(color_cont)
        c.set_style_bg_color(lv.palette_main(i), 0)
        c.set_style_radius(lv.RADIUS_CIRCLE, 0)
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
         btn.set_size(lv.dpx(50), lv.dpx(50))
         btn.align(lv.ALIGN.BOTTOM_RIGHT, -lv.dpx(15), -lv.dpx(15))

def shop_chart_event_cb(e) :

    code = e.get_code()
    if code == lv.EVENT.DRAW_PART_BEGIN :
        dsc = lv.obj_draw_part_dsc_t.__cast__(e.get_param())
        # Set the markers' text
        if dsc.part == lv.PART.TICKS and dsc.id == lv.chart.AXIS.PRIMARY_X :
            month = ["Jan", "Febr", "March", "Apr", "May", "Jun", "July", "Aug", "Sept", "Oct", "Nov", "Dec"]
            dsc.text = month[dsc.value]

        if dsc.part == lv.PART.ITEMS:
            dsc.rect_dsc.bg_opa = lv.OPA.TRANSP  # We will draw it later

    if code == lv.EVENT.DRAW_PART_END :
        dsc = lv.obj_draw_part_dsc_t.__cast__(e.get_param())
        # Add the faded area before the lines are drawn 
        if dsc.part == lv.PART.ITEMS :
            devices = [32, 43, 21, 56, 29, 36, 19, 25, 62, 35]
            clothes = [12, 19, 23, 31, 27, 32, 32, 11, 21, 32]
            services = [56, 38, 56, 13, 44, 32, 49, 64, 17, 33]

            draw_rect_dsc = lv.draw_rect_dsc_t()
            draw_rect_dsc.init()

            h = dsc.draw_area.get_height()

            a = lv.area_t()
            a.x1 = dsc.draw_area.x1
            a.x2 = dsc.draw_area.x2

            a.y1 = dsc.draw_area.y1
            a.y2 = a.y1 + 4 + (devices[dsc.id] * h) // 100 # +4 to overlap the radius
            draw_rect_dsc.bg_color = lv.palette_main(lv.PALETTE.RED)
            draw_rect_dsc.radius = 4
            dsc.draw_ctx.rect(draw_rect_dsc,a)
            # lv_draw_rect(dsc->draw_ctx, &draw_rect_dsc, &a);

            a.y1 = a.y2 - 4                                    # -4 to overlap the radius
            a.y2 = a.y1 + (clothes[dsc.id] * h) // 100
            draw_rect_dsc.bg_color = lv.palette_main(lv.PALETTE.BLUE)
            draw_rect_dsc.radius = 0
            dsc.draw_ctx.rect(draw_rect_dsc,a)
            # lv_draw_rect(dsc->draw_ctx, &draw_rect_dsc, &a);

            a.y1 = a.y2
            a.y2 = a.y1 + (services[dsc.id] * h) // 100
            draw_rect_dsc.bg_color = lv.palette_main(lv.PALETTE.GREEN)
            dsc.draw_ctx.rect(draw_rect_dsc,a)
            # lv_draw_rect(dsc->draw_ctx, &draw_rect_dsc, &a);

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
    
    meter.set_indicator_end_value(indic, val)
    
    card = lv.obj.get_parent(lv.obj.__cast__(meter))
    label = lv.obj.get_child(lv.obj.__cast__(card), -1)
    label.set_text("Cost : {:d}%".format(val))


def meter2_timer_cb(timer,meter,indics) :

    global session_desktop,session_tablet,session_mobile
    global down1,down2,down3
    
    if down1 : 
        session_desktop -= 137
        if session_desktop < 1400:
            down1 = False
        else :
            session_desktop += 116
        if session_desktop > 4500:
            down1 = True

    if down2:
        session_tablet -= 3
        if session_tablet < 1400 :
            down2 = False
    else :
        session_tablet += 9
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

    all = session_desktop + session_tablet + session_mobile
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
    label.set_text("      "+str(val) + "Mbps")

def ta_event_cb(e,kb,tv) :

    code = e.get_code()
    ta = e.get_target()
    # lv_obj_t * kb = lv_event_get_user_data(e);
    
    indev = lv.indev_get_act()
    if code == lv.EVENT.FOCUSED :
        if (not indev) or indev.get_type() != lv.INDEV_TYPE.KEYPAD :
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
        if (indev) :
            indev.reset(None)

    elif code == lv.EVENT.READY or code == lv.EVENT.CANCEL:
        tv.set_height(LV_VER_RES)
        kb.add_flag(lv.obj.FLAG.HIDDEN)
        ta.clear_state(lv.STATE.FOCUSED)
        if (indev) :
            indev.reset(None)    # To forget the last clicked object to make it focusable again

def birthday_event_cb(e):
    global calendar
    code = e.get_code()
    ta = e.get_target()

    if code == lv.EVENT.FOCUSED :
        indev = lv.indev_get_act()
        if (not indev) or indev.get_type() == lv.INDEV_TYPE.POINTER:
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
    code = e.get_code()
    obj = lv.calendar.__cast__(e.get_current_target())
    if code == lv.EVENT.VALUE_CHANGED:
        d = lv.calendar_date_t()
        obj.get_pressed_date(d)
        ta.set_text("{:02d}.{:02d}.{:d}".format(d.day,d.month,d.year))

        calendar.delete()
        calendar = None
        lv.layer_top().clear_flag(lv.obj.FLAG.CLICKABLE)
        lv.layer_top().set_style_bg_opa(lv.OPA.TRANSP, 0)

def _lv_area_intersect(a1_p, a2_p) :

    # Get the smaller area from 'a1_p' and 'a2_p'
    res_p = lv.area_t()

    res_p.x1 = max(a1_p.x1, a2_p.x1)
    res_p.y1 = max(a1_p.y1, a2_p.y1)
    res_p.x2 = min(a1_p.x2, a2_p.x2)
    res_p.y2 = min(a1_p.y2, a2_p.y2)

    # If x1 or y1 greater than x2 or y2 then the areas union is empty

    if res_p.x1 > res_p.x2 or res_p.y1 > res_p.y2:
        return None
    
    return res_p

def chart_event_cb(e):

    code = e.get_code()
    obj = e.get_target()
    dsc = lv.obj_draw_part_dsc_t.__cast__(e.get_param())

    if code == lv.EVENT.PRESSED or code == lv.EVENT.RELEASED :
        obj.invalidate()  #To make the value boxes visible

    elif code == lv.EVENT.DRAW_PART_BEGIN:
        # Set the markers' text
        if dsc.part == lv.PART.TICKS and dsc.id == lv.chart.AXIS.PRIMARY_X:
            if obj.get_type() == lv.chart.TYPE.BAR: 
                month = ["I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX", "X", "XI", "XII"]
                dsc.text = month[dsc.value]
            else :
                month = ["Jan", "Febr", "March", "Apr", "May", "Jun", "July", "Aug", "Sept", "Oct", "Nov", "Dec"]
                dsc.text = month[dsc.value]

        # Add the faded area before the lines are drawn 

        elif dsc.part == lv.PART.ITEMS:
            if LV_USE_DRAW_MASKS:
                # Add  a line mask that keeps the area below the line
                if dsc.p1 and dsc.p2:
                    line_mask_param = lv.draw_mask_line_param_t()
                    line_mask_param.points_init(dsc.p1.x, dsc.p1.y, dsc.p2.x, dsc.p2.y, lv.DRAW_MASK_LINE_SIDE.BOTTOM)
                
                    line_mask_id = lv.draw_mask_add(line_mask_param, None)              
                    # Add a fade effect: transparent bottom covering top
                    h = obj.get_height()
                    coords = lv.area_t()
                    obj.get_coords(coords)

                    fade_mask_param = lv.draw_mask_fade_param_t()
                    fade_mask_param.init(coords, lv.OPA.COVER, coords.y1 + h // 8, lv.OPA.TRANSP,coords.y2)
                    fade_mask_id = lv.draw_mask_add(fade_mask_param,None)

                    # Draw a rectangle that will be affected by the mask
                    draw_rect_dsc = lv.draw_rect_dsc_t()
                    draw_rect_dsc.init()
                    draw_rect_dsc.bg_opa = lv.OPA._50
                    draw_rect_dsc.bg_color = dsc.line_dsc.color
                    # obj_clip_area = lv.area_t() 
                    obj_clip_area = _lv_area_intersect(dsc.draw_ctx.clip_area, coords)
                    
                    clip_area_ori = dsc.draw_ctx.clip_area
                    dsc.draw_ctx.clip_area = obj_clip_area

                    a = lv.area_t()
                    a.x1 = dsc.p1.x
                    a.x2 = dsc.p2.x - 1
                    a.y1 = min(dsc.p1.y, dsc.p2.y)
                    a.y2 = coords.y2
                
                    dsc.draw_ctx.rect(draw_rect_dsc,a)
                    dsc.draw_ctx.clip_area = clip_area_ori

                    # Remove the masks 
                    lv.draw_mask_free_param(line_mask_param)
                    lv.draw_mask_remove_id(line_mask_id)
                    lv.draw_mask_remove_id(fade_mask_id)

            ser = lv.chart_series_t.__cast__(dsc.sub_part_ptr)
                                             
            if obj.get_pressed_point() == dsc.id :
                if obj.get_type() == lv.chart.TYPE.LINE:
                    dsc.rect_dsc.outline_color = lv.color_white()
                    dsc.rect_dsc.outline_width = 2
                else :
                    dsc.rect_dsc.shadow_color = ser.color
                    dsc.rect_dsc.shadow_width = 15
                    dsc.rect_dsc.shadow_spread = 0
            
                buf = "{:2d}".format(dsc.value) 
                text_size = lv.point_t() 
                lv.txt_get_size(text_size, buf, font_normal, 0, 0, lv.COORD.MAX, lv.TEXT_FLAG.NONE)

                txt_area = lv.area_t()
                if obj.get_type() == lv.chart.TYPE.BAR :
                    txt_area.y2 = dsc.draw_area.y1 - lv.dpx(15)
                    txt_area.y1 = txt_area.y2 - text_size.y
                    if ser == obj.get_series_next(None) :
                        txt_area.x1 = dsc.draw_area.x1 + dsc.draw_area.get_width() // 2
                        txt_area.x2 = txt_area.x1 + text_size.x
                    else :
                        txt_area.x2 = dsc.draw_area.x1 + dsc.draw_area.get_width() // 2
                        txt_area.x1 = txt_area.x2 - text_size.x
                else :
                    txt_area.x1 = dsc.draw_area.x1 + dsc.draw_area.get_width() // 2 - text_size.x // 2
                    txt_area.x2 = txt_area.x1 + text_size.x
                    txt_area.y2 = dsc.draw_area.y1 - lv.dpx(15)
                    txt_area.y1 = txt_area.y2 - text_size.y

                bg_area = lv.area_t()
                bg_area.x1 = txt_area.x1 - lv.dpx(8)
                bg_area.x2 = txt_area.x2 + lv.dpx(8)
                bg_area.y1 = txt_area.y1 - lv.dpx(8)
                bg_area.y2 = txt_area.y2 + lv.dpx(8)

                rect_dsc = lv.draw_rect_dsc_t()
                rect_dsc.init()
                rect_dsc.bg_color = ser.color
                rect_dsc.radius = lv.dpx(5)
                dsc.draw_ctx.rect(rect_dsc,bg_area)

                label_dsc = lv.draw_label_dsc_t()
                label_dsc.init()
                label_dsc.color = lv.color_white()
                label_dsc.font = font_normal
                dsc.draw_ctx.label(label_dsc,txt_area,buf,None)

            else :
                dsc.rect_dsc.outline_width = 0
                dsc.rect_dsc.shadow_width = 0

t1 = tv.add_tab("Profile")
t2 = tv.add_tab("Analytics")
t3 = tv.add_tab("Shop")

profile_create(t1)
analytics_create(t2)
shop_create(t3)

color_changer_create(tv)
