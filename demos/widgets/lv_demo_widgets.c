/**
 * @file lv_demo_widgets.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_widgets.h"

#if LV_USE_DEMO_WIDGETS

#if LV_MEM_CUSTOM == 0 && LV_MEM_SIZE < (38ul * 1024ul)
    #error Insufficient memory for lv_demo_widgets. Please set LV_MEM_SIZE to at least 38KB (38ul * 1024ul).  48KB is recommended.
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    DISP_SMALL,
    DISP_MEDIUM,
    DISP_LARGE,
} disp_size_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void profile_create(lv_obj_t * parent);
static void analytics_create(lv_obj_t * parent);
static void shop_create(lv_obj_t * parent);
static void color_changer_create(lv_obj_t * parent);

static lv_obj_t * create_meter_box(lv_obj_t * parent, const char * title, const char * text1, const char * text2,
                                   const char * text3);
static lv_obj_t * create_shop_item(lv_obj_t * parent, const void * img_src, const char * name, const char * category,
                                   const char * price);

static void color_changer_event_cb(lv_event_t * e);
static void color_event_cb(lv_event_t * e);
static void ta_event_cb(lv_event_t * e);
static void birthday_event_cb(lv_event_t * e);
static void calendar_event_cb(lv_event_t * e);
static void slider_event_cb(lv_event_t * e);
static void chart_event_cb(lv_event_t * e);
static void shop_chart_event_cb(lv_event_t * e);
static void meter1_indic1_anim_cb(void * var, int32_t v);
static void meter1_indic2_anim_cb(void * var, int32_t v);
static void meter1_indic3_anim_cb(void * var, int32_t v);
static void meter2_timer_cb(lv_timer_t * timer);
static void meter3_anim_cb(void * var, int32_t v);

/**********************
 *  STATIC VARIABLES
 **********************/
static disp_size_t disp_size;

static lv_obj_t * tv;
static lv_obj_t * calendar;
static lv_style_t style_text_muted;
static lv_style_t style_title;
static lv_style_t style_icon;
static lv_style_t style_bullet;

static lv_obj_t * meter1;
static lv_obj_t * meter2;
static lv_obj_t * meter3;

static lv_obj_t * chart1;
static lv_obj_t * chart2;
static lv_obj_t * chart3;

static lv_chart_series_t * ser1;
static lv_chart_series_t * ser2;
static lv_chart_series_t * ser3;
static lv_chart_series_t * ser4;

static const lv_font_t * font_large;
static const lv_font_t * font_normal;

static uint32_t session_desktop = 1000;
static uint32_t session_tablet = 1000;
static uint32_t session_mobile = 1000;

static lv_timer_t * meter2_timer;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_widgets(void)
{
    if(LV_HOR_RES <= 320) disp_size = DISP_SMALL;
    else if(LV_HOR_RES < 720) disp_size = DISP_MEDIUM;
    else disp_size = DISP_LARGE;

    font_large = LV_FONT_DEFAULT;
    font_normal = LV_FONT_DEFAULT;

    lv_coord_t tab_h;
    if(disp_size == DISP_LARGE) {
        tab_h = 70;
#if LV_FONT_MONTSERRAT_24
        font_large     = &lv_font_montserrat_24;
#else
        LV_LOG_WARN("LV_FONT_MONTSERRAT_24 is not enabled for the widgets demo. Using LV_FONT_DEFAULT instead.");
#endif
#if LV_FONT_MONTSERRAT_16
        font_normal    = &lv_font_montserrat_16;
#else
        LV_LOG_WARN("LV_FONT_MONTSERRAT_16 is not enabled for the widgets demo. Using LV_FONT_DEFAULT instead.");
#endif
    }
    else if(disp_size == DISP_MEDIUM) {
        tab_h = 45;
#if LV_FONT_MONTSERRAT_20
        font_large     = &lv_font_montserrat_20;
#else
        LV_LOG_WARN("LV_FONT_MONTSERRAT_20 is not enabled for the widgets demo. Using LV_FONT_DEFAULT instead.");
#endif
#if LV_FONT_MONTSERRAT_14
        font_normal    = &lv_font_montserrat_14;
#else
        LV_LOG_WARN("LV_FONT_MONTSERRAT_14 is not enabled for the widgets demo. Using LV_FONT_DEFAULT instead.");
#endif
    }
    else {   /* disp_size == DISP_SMALL */
        tab_h = 45;
#if LV_FONT_MONTSERRAT_18
        font_large     = &lv_font_montserrat_18;
#else
        LV_LOG_WARN("LV_FONT_MONTSERRAT_18 is not enabled for the widgets demo. Using LV_FONT_DEFAULT instead.");
#endif
#if LV_FONT_MONTSERRAT_12
        font_normal    = &lv_font_montserrat_12;
#else
        LV_LOG_WARN("LV_FONT_MONTSERRAT_12 is not enabled for the widgets demo. Using LV_FONT_DEFAULT instead.");
#endif
    }

#if LV_USE_THEME_DEFAULT
    lv_theme_default_init(NULL, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), LV_THEME_DEFAULT_DARK,
                          font_normal);
#endif

    lv_style_init(&style_text_muted);
    lv_style_set_text_opa(&style_text_muted, LV_OPA_50);

    lv_style_init(&style_title);
    lv_style_set_text_font(&style_title, font_large);

    lv_style_init(&style_icon);
    lv_style_set_text_color(&style_icon, lv_theme_get_color_primary(NULL));
    lv_style_set_text_font(&style_icon, font_large);

    lv_style_init(&style_bullet);
    lv_style_set_border_width(&style_bullet, 0);
    lv_style_set_radius(&style_bullet, LV_RADIUS_CIRCLE);

    tv = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, tab_h);

    lv_obj_set_style_text_font(lv_scr_act(), font_normal, 0);

    if(disp_size == DISP_LARGE) {
        lv_obj_t * tab_btns = lv_tabview_get_tab_btns(tv);
        lv_obj_set_style_pad_left(tab_btns, LV_HOR_RES / 2, 0);
        lv_obj_t * logo = lv_img_create(tab_btns);
        LV_IMG_DECLARE(img_lvgl_logo);
        lv_img_set_src(logo, &img_lvgl_logo);
        lv_obj_align(logo, LV_ALIGN_LEFT_MID, -LV_HOR_RES / 2 + 25, 0);

        lv_obj_t * label = lv_label_create(tab_btns);
        lv_obj_add_style(label, &style_title, 0);
        lv_label_set_text(label, "LVGL v8");
        lv_obj_align_to(label, logo, LV_ALIGN_OUT_RIGHT_TOP, 10, 0);

        label = lv_label_create(tab_btns);
        lv_label_set_text(label, "Widgets demo");
        lv_obj_add_style(label, &style_text_muted, 0);
        lv_obj_align_to(label, logo, LV_ALIGN_OUT_RIGHT_BOTTOM, 10, 0);
    }

    lv_obj_t * t1 = lv_tabview_add_tab(tv, "Profile");
    lv_obj_t * t2 = lv_tabview_add_tab(tv, "Analytics");
    lv_obj_t * t3 = lv_tabview_add_tab(tv, "Shop");
    profile_create(t1);
    analytics_create(t2);
    shop_create(t3);

    color_changer_create(tv);
}

void lv_demo_widgets_close(void)
{
    /*Delete all animation*/
    lv_anim_del(NULL, NULL);

    lv_timer_del(meter2_timer);
    meter2_timer = NULL;

    lv_obj_clean(lv_scr_act());

    lv_style_reset(&style_text_muted);
    lv_style_reset(&style_title);
    lv_style_reset(&style_icon);
    lv_style_reset(&style_bullet);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void profile_create(lv_obj_t * parent)
{
    lv_obj_t * panel1 = lv_obj_create(parent);
    lv_obj_set_height(panel1, LV_SIZE_CONTENT);

    LV_IMG_DECLARE(img_demo_widgets_avatar);
    lv_obj_t * avatar = lv_img_create(panel1);
    lv_img_set_src(avatar, &img_demo_widgets_avatar);

    lv_obj_t * name = lv_label_create(panel1);
    lv_label_set_text(name, "Elena Smith");
    lv_obj_add_style(name, &style_title, 0);

    lv_obj_t * dsc = lv_label_create(panel1);
    lv_obj_add_style(dsc, &style_text_muted, 0);
    lv_label_set_text(dsc, "This is a short description of me. Take a look at my profile!");
    lv_label_set_long_mode(dsc, LV_LABEL_LONG_WRAP);

    lv_obj_t * email_icn = lv_label_create(panel1);
    lv_obj_add_style(email_icn, &style_icon, 0);
    lv_label_set_text(email_icn, LV_SYMBOL_ENVELOPE);

    lv_obj_t * email_label = lv_label_create(panel1);
    lv_label_set_text(email_label, "elena@smith.com");

    lv_obj_t * call_icn = lv_label_create(panel1);
    lv_obj_add_style(call_icn, &style_icon, 0);
    lv_label_set_text(call_icn, LV_SYMBOL_CALL);

    lv_obj_t * call_label = lv_label_create(panel1);
    lv_label_set_text(call_label, "+79 246 123 4567");

    lv_obj_t * log_out_btn = lv_btn_create(panel1);
    lv_obj_set_height(log_out_btn, LV_SIZE_CONTENT);

    lv_obj_t * label = lv_label_create(log_out_btn);
    lv_label_set_text(label, "Log out");
    lv_obj_center(label);

    lv_obj_t * invite_btn = lv_btn_create(panel1);
    lv_obj_add_state(invite_btn, LV_STATE_DISABLED);
    lv_obj_set_height(invite_btn, LV_SIZE_CONTENT);

    label = lv_label_create(invite_btn);
    lv_label_set_text(label, "Invite");
    lv_obj_center(label);

    /*Create a keyboard*/
    lv_obj_t * kb = lv_keyboard_create(lv_scr_act());
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);

    /*Create the second panel*/
    lv_obj_t * panel2 = lv_obj_create(parent);
    lv_obj_set_height(panel2, LV_SIZE_CONTENT);

    lv_obj_t * panel2_title = lv_label_create(panel2);
    lv_label_set_text(panel2_title, "Your profile");
    lv_obj_add_style(panel2_title, &style_title, 0);

    lv_obj_t * user_name_label = lv_label_create(panel2);
    lv_label_set_text(user_name_label, "User name");
    lv_obj_add_style(user_name_label, &style_text_muted, 0);

    lv_obj_t * user_name = lv_textarea_create(panel2);
    lv_textarea_set_one_line(user_name, true);
    lv_textarea_set_placeholder_text(user_name, "Your name");
    lv_obj_add_event_cb(user_name, ta_event_cb, LV_EVENT_ALL, kb);

    lv_obj_t * password_label = lv_label_create(panel2);
    lv_label_set_text(password_label, "Password");
    lv_obj_add_style(password_label, &style_text_muted, 0);

    lv_obj_t * password = lv_textarea_create(panel2);
    lv_textarea_set_one_line(password, true);
    lv_textarea_set_password_mode(password, true);
    lv_textarea_set_placeholder_text(password, "Min. 8 chars.");
    lv_obj_add_event_cb(password, ta_event_cb, LV_EVENT_ALL, kb);

    lv_obj_t * gender_label = lv_label_create(panel2);
    lv_label_set_text(gender_label, "Gender");
    lv_obj_add_style(gender_label, &style_text_muted, 0);

    lv_obj_t * gender = lv_dropdown_create(panel2);
    lv_dropdown_set_options_static(gender, "Male\nFemale\nOther");

    lv_obj_t * birthday_label = lv_label_create(panel2);
    lv_label_set_text(birthday_label, "Birthday");
    lv_obj_add_style(birthday_label, &style_text_muted, 0);

    lv_obj_t * birthdate = lv_textarea_create(panel2);
    lv_textarea_set_one_line(birthdate, true);
    lv_obj_add_event_cb(birthdate, birthday_event_cb, LV_EVENT_ALL, NULL);

    /*Create the third panel*/
    lv_obj_t * panel3 = lv_obj_create(parent);
    lv_obj_t * panel3_title = lv_label_create(panel3);
    lv_label_set_text(panel3_title, "Your skills");
    lv_obj_add_style(panel3_title, &style_title, 0);

    lv_obj_t * experience_label = lv_label_create(panel3);
    lv_label_set_text(experience_label, "Experience");
    lv_obj_add_style(experience_label, &style_text_muted, 0);

    lv_obj_t * slider1 = lv_slider_create(panel3);
    lv_obj_set_width(slider1, LV_PCT(95));
    lv_obj_add_event_cb(slider1, slider_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_refresh_ext_draw_size(slider1);

    lv_obj_t * team_player_label = lv_label_create(panel3);
    lv_label_set_text(team_player_label, "Team player");
    lv_obj_add_style(team_player_label, &style_text_muted, 0);

    lv_obj_t * sw1 = lv_switch_create(panel3);

    lv_obj_t * hard_working_label = lv_label_create(panel3);
    lv_label_set_text(hard_working_label, "Hard-working");
    lv_obj_add_style(hard_working_label, &style_text_muted, 0);

    lv_obj_t * sw2 = lv_switch_create(panel3);

    if(disp_size == DISP_LARGE) {
        static lv_coord_t grid_main_col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        static lv_coord_t grid_main_row_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};

        /*Create the top panel*/
        static lv_coord_t grid_1_col_dsc[] = {LV_GRID_CONTENT, 5, LV_GRID_CONTENT, LV_GRID_FR(2), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        static lv_coord_t grid_1_row_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, 10, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};

        static lv_coord_t grid_2_col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        static lv_coord_t grid_2_row_dsc[] = {
            LV_GRID_CONTENT,  /*Title*/
            5,                /*Separator*/
            LV_GRID_CONTENT,  /*Box title*/
            30,               /*Boxes*/
            5,                /*Separator*/
            LV_GRID_CONTENT,  /*Box title*/
            30,               /*Boxes*/
            LV_GRID_TEMPLATE_LAST
        };

        lv_obj_set_grid_dsc_array(parent, grid_main_col_dsc, grid_main_row_dsc);

        lv_obj_set_grid_cell(panel1, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_CENTER, 0, 1);

        lv_obj_set_grid_dsc_array(panel1, grid_1_col_dsc, grid_1_row_dsc);
        lv_obj_set_grid_cell(avatar, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 0, 5);
        lv_obj_set_grid_cell(name, LV_GRID_ALIGN_START, 2, 2, LV_GRID_ALIGN_CENTER, 0, 1);
        lv_obj_set_grid_cell(dsc, LV_GRID_ALIGN_STRETCH, 2, 4, LV_GRID_ALIGN_START, 1, 1);
        lv_obj_set_grid_cell(email_icn, LV_GRID_ALIGN_CENTER, 2, 1, LV_GRID_ALIGN_CENTER, 3, 1);
        lv_obj_set_grid_cell(email_label, LV_GRID_ALIGN_START, 3, 1, LV_GRID_ALIGN_CENTER, 3, 1);
        lv_obj_set_grid_cell(call_icn, LV_GRID_ALIGN_CENTER, 2, 1, LV_GRID_ALIGN_CENTER, 4, 1);
        lv_obj_set_grid_cell(call_label, LV_GRID_ALIGN_START, 3, 1, LV_GRID_ALIGN_CENTER, 4, 1);
        lv_obj_set_grid_cell(log_out_btn, LV_GRID_ALIGN_STRETCH, 4, 1, LV_GRID_ALIGN_CENTER, 3, 2);
        lv_obj_set_grid_cell(invite_btn, LV_GRID_ALIGN_STRETCH, 5, 1, LV_GRID_ALIGN_CENTER, 3, 2);

        lv_obj_set_grid_cell(panel2, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_START, 1, 1);
        lv_obj_set_grid_dsc_array(panel2, grid_2_col_dsc, grid_2_row_dsc);
        lv_obj_set_grid_cell(panel2_title, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_CENTER, 0, 1);
        lv_obj_set_grid_cell(user_name, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_CENTER, 3, 1);
        lv_obj_set_grid_cell(user_name_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 2, 1);
        lv_obj_set_grid_cell(password, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 3, 1);
        lv_obj_set_grid_cell(password_label, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_START, 2, 1);
        lv_obj_set_grid_cell(birthdate, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 6, 1);
        lv_obj_set_grid_cell(birthday_label, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_START, 5, 1);
        lv_obj_set_grid_cell(gender, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_CENTER, 6, 1);
        lv_obj_set_grid_cell(gender_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 5, 1);

        lv_obj_set_grid_cell(panel3, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
        lv_obj_set_grid_dsc_array(panel3, grid_2_col_dsc, grid_2_row_dsc);
        lv_obj_set_grid_cell(panel3_title, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_CENTER, 0, 1);
        lv_obj_set_grid_cell(slider1, LV_GRID_ALIGN_CENTER, 0, 2, LV_GRID_ALIGN_CENTER, 3, 1);
        lv_obj_set_grid_cell(experience_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 2, 1);
        lv_obj_set_grid_cell(sw2, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 6, 1);
        lv_obj_set_grid_cell(hard_working_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 5, 1);
        lv_obj_set_grid_cell(sw1, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 6, 1);
        lv_obj_set_grid_cell(team_player_label, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_START, 5, 1);
    }
    else if(disp_size == DISP_MEDIUM) {
        static lv_coord_t grid_main_col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        static lv_coord_t grid_main_row_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};

        /*Create the top panel*/
        static lv_coord_t grid_1_col_dsc[] = {LV_GRID_CONTENT, 1, LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        static lv_coord_t grid_1_row_dsc[] = {
            LV_GRID_CONTENT, /*Name*/
            LV_GRID_CONTENT, /*Description*/
            LV_GRID_CONTENT, /*Email*/
            -20,
            LV_GRID_CONTENT, /*Phone*/
            LV_GRID_CONTENT, /*Buttons*/
            LV_GRID_TEMPLATE_LAST
        };

        static lv_coord_t grid_2_col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        static lv_coord_t grid_2_row_dsc[] = {
            LV_GRID_CONTENT,  /*Title*/
            5,                /*Separator*/
            LV_GRID_CONTENT,  /*Box title*/
            40,               /*Box*/
            LV_GRID_CONTENT,  /*Box title*/
            40,               /*Box*/
            LV_GRID_CONTENT,  /*Box title*/
            40,               /*Box*/
            LV_GRID_CONTENT,  /*Box title*/
            40,               /*Box*/
            LV_GRID_TEMPLATE_LAST
        };

        lv_obj_set_grid_dsc_array(parent, grid_main_col_dsc, grid_main_row_dsc);
        lv_obj_set_grid_cell(panel1, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_CENTER, 0, 1);

        lv_obj_set_width(log_out_btn, 120);
        lv_obj_set_width(invite_btn, 120);

        lv_obj_set_grid_dsc_array(panel1, grid_1_col_dsc, grid_1_row_dsc);
        lv_obj_set_grid_cell(avatar, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_START, 0, 4);
        lv_obj_set_grid_cell(name, LV_GRID_ALIGN_START, 2, 2, LV_GRID_ALIGN_CENTER, 0, 1);
        lv_obj_set_grid_cell(dsc, LV_GRID_ALIGN_STRETCH, 2, 2, LV_GRID_ALIGN_START, 1, 1);
        lv_obj_set_grid_cell(email_label, LV_GRID_ALIGN_START, 3, 1, LV_GRID_ALIGN_CENTER, 2, 1);
        lv_obj_set_grid_cell(email_icn, LV_GRID_ALIGN_CENTER, 2, 1, LV_GRID_ALIGN_CENTER, 2, 1);
        lv_obj_set_grid_cell(call_icn, LV_GRID_ALIGN_CENTER, 2, 1, LV_GRID_ALIGN_CENTER, 4, 1);
        lv_obj_set_grid_cell(call_label, LV_GRID_ALIGN_START, 3, 1, LV_GRID_ALIGN_CENTER, 4, 1);
        lv_obj_set_grid_cell(log_out_btn, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 5, 1);
        lv_obj_set_grid_cell(invite_btn, LV_GRID_ALIGN_END, 3, 1, LV_GRID_ALIGN_CENTER, 5, 1);

        lv_obj_set_grid_cell(panel2, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_START, 1, 1);
        lv_obj_set_grid_dsc_array(panel2, grid_2_col_dsc, grid_2_row_dsc);
        lv_obj_set_grid_cell(panel2_title, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_CENTER, 0, 1);
        lv_obj_set_grid_cell(user_name_label, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 2, 1);
        lv_obj_set_grid_cell(user_name, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_START, 3, 1);
        lv_obj_set_grid_cell(password_label, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 4, 1);
        lv_obj_set_grid_cell(password, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_START, 5, 1);
        lv_obj_set_grid_cell(birthday_label, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 6, 1);
        lv_obj_set_grid_cell(birthdate, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_START, 7, 1);
        lv_obj_set_grid_cell(gender_label, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 8, 1);
        lv_obj_set_grid_cell(gender, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_START, 9, 1);

        lv_obj_set_grid_cell(panel3, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
        lv_obj_set_grid_dsc_array(panel3, grid_2_col_dsc, grid_2_row_dsc);
        lv_obj_set_grid_cell(panel3_title, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_CENTER, 0, 1);
        lv_obj_set_grid_cell(slider1, LV_GRID_ALIGN_CENTER, 0, 2, LV_GRID_ALIGN_CENTER, 3, 1);
        lv_obj_set_grid_cell(experience_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 2, 1);
        lv_obj_set_grid_cell(hard_working_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 4, 1);
        lv_obj_set_grid_cell(sw2, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 5, 1);
        lv_obj_set_grid_cell(team_player_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 6, 1);
        lv_obj_set_grid_cell(sw1, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 7, 1);
    }
    else if(disp_size == DISP_SMALL) {
        static lv_coord_t grid_main_col_dsc[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        static lv_coord_t grid_main_row_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
        lv_obj_set_grid_dsc_array(parent, grid_main_col_dsc, grid_main_row_dsc);

        /*Create the top panel*/
        static lv_coord_t grid_1_col_dsc[] = {LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        static lv_coord_t grid_1_row_dsc[] = {LV_GRID_CONTENT, /*Avatar*/
                                              LV_GRID_CONTENT, /*Name*/
                                              LV_GRID_CONTENT, /*Description*/
                                              LV_GRID_CONTENT, /*Email*/
                                              LV_GRID_CONTENT, /*Phone number*/
                                              LV_GRID_CONTENT, /*Button1*/
                                              LV_GRID_CONTENT, /*Button2*/
                                              LV_GRID_TEMPLATE_LAST
                                             };

        lv_obj_set_grid_dsc_array(panel1, grid_1_col_dsc, grid_1_row_dsc);

        static lv_coord_t grid_2_col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        static lv_coord_t grid_2_row_dsc[] = {
            LV_GRID_CONTENT,  /*Title*/
            5,                /*Separator*/
            LV_GRID_CONTENT,  /*Box title*/
            40,               /*Box*/
            LV_GRID_CONTENT,  /*Box title*/
            40,               /*Box*/
            LV_GRID_CONTENT,  /*Box title*/
            40,               /*Box*/
            LV_GRID_CONTENT,  /*Box title*/
            40, LV_GRID_TEMPLATE_LAST               /*Box*/
        };

        lv_obj_set_grid_dsc_array(panel2, grid_2_col_dsc, grid_2_row_dsc);
        lv_obj_set_grid_dsc_array(panel3, grid_2_col_dsc, grid_2_row_dsc);

        lv_obj_set_grid_cell(panel1, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);

        lv_obj_set_style_text_align(dsc, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_set_grid_cell(avatar, LV_GRID_ALIGN_CENTER, 0, 2, LV_GRID_ALIGN_CENTER, 0, 1);
        lv_obj_set_grid_cell(name, LV_GRID_ALIGN_CENTER, 0, 2, LV_GRID_ALIGN_CENTER, 1, 1);
        lv_obj_set_grid_cell(dsc, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_START, 2, 1);
        lv_obj_set_grid_cell(email_icn, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 3, 1);
        lv_obj_set_grid_cell(email_label, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 3, 1);
        lv_obj_set_grid_cell(call_icn, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 4, 1);
        lv_obj_set_grid_cell(call_label, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 4, 1);
        lv_obj_set_grid_cell(log_out_btn, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_CENTER, 5, 1);
        lv_obj_set_grid_cell(invite_btn, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_CENTER, 6, 1);

        lv_obj_set_grid_cell(panel2, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_START, 1, 1);
        lv_obj_set_grid_cell(panel2_title, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_CENTER, 0, 1);
        lv_obj_set_grid_cell(user_name_label, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 2, 1);
        lv_obj_set_grid_cell(user_name, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_START, 3, 1);
        lv_obj_set_grid_cell(password_label, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 4, 1);
        lv_obj_set_grid_cell(password, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_START, 5, 1);
        lv_obj_set_grid_cell(birthday_label, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 6, 1);
        lv_obj_set_grid_cell(birthdate, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_START, 7, 1);
        lv_obj_set_grid_cell(gender_label, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 8, 1);
        lv_obj_set_grid_cell(gender, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_START, 9, 1);

        lv_obj_set_height(panel3, LV_SIZE_CONTENT);
        lv_obj_set_grid_cell(panel3, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_START, 2, 1);
        lv_obj_set_grid_cell(panel3_title, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_CENTER, 0, 1);
        lv_obj_set_grid_cell(experience_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 2, 1);
        lv_obj_set_grid_cell(slider1, LV_GRID_ALIGN_CENTER, 0, 2, LV_GRID_ALIGN_CENTER, 3, 1);
        lv_obj_set_grid_cell(hard_working_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 4, 1);
        lv_obj_set_grid_cell(sw1, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 5, 1);
        lv_obj_set_grid_cell(team_player_label, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_START, 4, 1);
        lv_obj_set_grid_cell(sw2, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_START, 5, 1);
    }
}

static void analytics_create(lv_obj_t * parent)
{
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_ROW_WRAP);

    static lv_coord_t grid_chart_row_dsc[] = {LV_GRID_CONTENT, LV_GRID_FR(1), 10, LV_GRID_TEMPLATE_LAST};
    static lv_coord_t grid_chart_col_dsc[] = {20, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

    lv_obj_t * chart1_cont = lv_obj_create(parent);
    lv_obj_set_flex_grow(chart1_cont, 1);
    lv_obj_set_grid_dsc_array(chart1_cont, grid_chart_col_dsc, grid_chart_row_dsc);

    lv_obj_set_height(chart1_cont, LV_PCT(100));
    lv_obj_set_style_max_height(chart1_cont, 300, 0);

    lv_obj_t * title = lv_label_create(chart1_cont);
    lv_label_set_text(title, "Unique visitors");
    lv_obj_add_style(title, &style_title, 0);
    lv_obj_set_grid_cell(title, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 0, 1);

    chart1 = lv_chart_create(chart1_cont);
    lv_group_add_obj(lv_group_get_default(), chart1);
    lv_obj_add_flag(chart1, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_set_grid_cell(chart1, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
    lv_chart_set_axis_tick(chart1, LV_CHART_AXIS_PRIMARY_Y, 0, 0, 5, 1, true, 80);
    lv_chart_set_axis_tick(chart1, LV_CHART_AXIS_PRIMARY_X, 0, 0, 12, 1, true, 50);
    lv_chart_set_div_line_count(chart1, 0, 12);
    lv_chart_set_point_count(chart1, 12);
    lv_obj_add_event_cb(chart1, chart_event_cb, LV_EVENT_ALL, NULL);
    if(disp_size == DISP_SMALL) lv_chart_set_zoom_x(chart1, 256 * 3);
    else if(disp_size == DISP_MEDIUM) lv_chart_set_zoom_x(chart1, 256 * 2);

    lv_obj_set_style_border_side(chart1, LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_radius(chart1, 0, 0);

    ser1 = lv_chart_add_series(chart1, lv_theme_get_color_primary(chart1), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_next_value(chart1, ser1, lv_rand(10, 80));
    lv_chart_set_next_value(chart1, ser1, lv_rand(10, 80));
    lv_chart_set_next_value(chart1, ser1, lv_rand(10, 80));
    lv_chart_set_next_value(chart1, ser1, lv_rand(10, 80));
    lv_chart_set_next_value(chart1, ser1, lv_rand(10, 80));
    lv_chart_set_next_value(chart1, ser1, lv_rand(10, 80));
    lv_chart_set_next_value(chart1, ser1, lv_rand(10, 80));
    lv_chart_set_next_value(chart1, ser1, lv_rand(10, 80));
    lv_chart_set_next_value(chart1, ser1, lv_rand(10, 80));
    lv_chart_set_next_value(chart1, ser1, lv_rand(10, 80));
    lv_chart_set_next_value(chart1, ser1, lv_rand(10, 80));
    lv_chart_set_next_value(chart1, ser1, lv_rand(10, 80));
    lv_chart_set_next_value(chart1, ser1, lv_rand(10, 80));

    lv_obj_t * chart2_cont = lv_obj_create(parent);
    lv_obj_add_flag(chart2_cont, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
    lv_obj_set_flex_grow(chart2_cont, 1);

    lv_obj_set_height(chart2_cont, LV_PCT(100));
    lv_obj_set_style_max_height(chart2_cont, 300, 0);

    lv_obj_set_grid_dsc_array(chart2_cont, grid_chart_col_dsc, grid_chart_row_dsc);

    title = lv_label_create(chart2_cont);
    lv_label_set_text(title, "Monthly revenue");
    lv_obj_add_style(title, &style_title, 0);
    lv_obj_set_grid_cell(title, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 0, 1);

    chart2 = lv_chart_create(chart2_cont);
    lv_group_add_obj(lv_group_get_default(), chart2);
    lv_obj_add_flag(chart2, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    lv_obj_set_grid_cell(chart2, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
    lv_chart_set_axis_tick(chart2, LV_CHART_AXIS_PRIMARY_Y, 0, 0, 5, 1, true, 80);
    lv_chart_set_axis_tick(chart2, LV_CHART_AXIS_PRIMARY_X, 0, 0, 12, 1, true, 50);
    lv_obj_set_size(chart2, LV_PCT(100), LV_PCT(100));
    lv_chart_set_type(chart2, LV_CHART_TYPE_BAR);
    lv_chart_set_div_line_count(chart2, 6, 0);
    lv_chart_set_point_count(chart2, 12);
    lv_obj_add_event_cb(chart2, chart_event_cb, LV_EVENT_ALL, NULL);
    lv_chart_set_zoom_x(chart2, 256 * 2);
    lv_obj_set_style_border_side(chart2, LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_radius(chart2, 0, 0);

    if(disp_size == DISP_SMALL) {
        lv_obj_set_style_pad_gap(chart2, 0, LV_PART_ITEMS);
        lv_obj_set_style_pad_gap(chart2, 2, LV_PART_MAIN);
    }
    else if(disp_size == DISP_LARGE) {
        lv_obj_set_style_pad_gap(chart2, 16, 0);
    }

    ser2 = lv_chart_add_series(chart2, lv_palette_lighten(LV_PALETTE_GREY, 1), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_next_value(chart2, ser2, lv_rand(10, 80));
    lv_chart_set_next_value(chart2, ser2, lv_rand(10, 80));
    lv_chart_set_next_value(chart2, ser2, lv_rand(10, 80));
    lv_chart_set_next_value(chart2, ser2, lv_rand(10, 80));
    lv_chart_set_next_value(chart2, ser2, lv_rand(10, 80));
    lv_chart_set_next_value(chart2, ser2, lv_rand(10, 80));
    lv_chart_set_next_value(chart2, ser2, lv_rand(10, 80));
    lv_chart_set_next_value(chart2, ser2, lv_rand(10, 80));
    lv_chart_set_next_value(chart2, ser2, lv_rand(10, 80));
    lv_chart_set_next_value(chart2, ser2, lv_rand(10, 80));
    lv_chart_set_next_value(chart2, ser2, lv_rand(10, 80));
    lv_chart_set_next_value(chart2, ser2, lv_rand(10, 80));
    lv_chart_set_next_value(chart2, ser2, lv_rand(10, 80));

    ser3 = lv_chart_add_series(chart2, lv_theme_get_color_primary(chart1), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_next_value(chart2, ser3, lv_rand(10, 80));
    lv_chart_set_next_value(chart2, ser3, lv_rand(10, 80));
    lv_chart_set_next_value(chart2, ser3, lv_rand(10, 80));
    lv_chart_set_next_value(chart2, ser3, lv_rand(10, 80));
    lv_chart_set_next_value(chart2, ser3, lv_rand(10, 80));
    lv_chart_set_next_value(chart2, ser3, lv_rand(10, 80));
    lv_chart_set_next_value(chart2, ser3, lv_rand(10, 80));
    lv_chart_set_next_value(chart2, ser3, lv_rand(10, 80));
    lv_chart_set_next_value(chart2, ser3, lv_rand(10, 80));
    lv_chart_set_next_value(chart2, ser3, lv_rand(10, 80));
    lv_chart_set_next_value(chart2, ser3, lv_rand(10, 80));
    lv_chart_set_next_value(chart2, ser3, lv_rand(10, 80));
    lv_chart_set_next_value(chart2, ser3, lv_rand(10, 80));

    lv_meter_scale_t * scale;
    lv_meter_indicator_t * indic;
    meter1 = create_meter_box(parent, "Monthly Target", "Revenue: 63%", "Sales: 44%", "Costs: 58%");
    lv_obj_add_flag(lv_obj_get_parent(meter1), LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
    scale = lv_meter_add_scale(meter1);
    lv_meter_set_scale_range(meter1, scale, 0, 100, 270, 90);
    lv_meter_set_scale_ticks(meter1, scale, 0, 0, 0, lv_color_black());

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_values(&a, 20, 100);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);

    indic = lv_meter_add_arc(meter1, scale, 15, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_anim_set_exec_cb(&a, meter1_indic1_anim_cb);
    lv_anim_set_var(&a, indic);
    lv_anim_set_time(&a, 4100);
    lv_anim_set_playback_time(&a, 2700);
    lv_anim_start(&a);

    indic = lv_meter_add_arc(meter1, scale, 15, lv_palette_main(LV_PALETTE_RED), -20);
    lv_anim_set_exec_cb(&a, meter1_indic2_anim_cb);
    lv_anim_set_var(&a, indic);
    lv_anim_set_time(&a, 2600);
    lv_anim_set_playback_time(&a, 3200);
    a.user_data = indic;
    lv_anim_start(&a);

    indic = lv_meter_add_arc(meter1, scale, 15, lv_palette_main(LV_PALETTE_GREEN), -40);
    lv_anim_set_exec_cb(&a, meter1_indic3_anim_cb);
    lv_anim_set_var(&a, indic);
    lv_anim_set_time(&a, 2800);
    lv_anim_set_playback_time(&a, 1800);
    lv_anim_start(&a);

    meter2 = create_meter_box(parent, "Sessions", "Desktop: ", "Tablet: ", "Mobile: ");
    if(disp_size < DISP_LARGE) lv_obj_add_flag(lv_obj_get_parent(meter2), LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
    scale = lv_meter_add_scale(meter2);
    lv_meter_set_scale_range(meter2, scale, 0, 100, 360, 90);
    lv_meter_set_scale_ticks(meter2, scale, 0, 0, 0, lv_color_black());

    static lv_meter_indicator_t * meter2_indic[3];
    meter2_indic[0] = lv_meter_add_arc(meter2, scale, 20, lv_palette_main(LV_PALETTE_RED), -10);
    lv_meter_set_indicator_start_value(meter2, meter2_indic[0], 0);
    lv_meter_set_indicator_end_value(meter2, meter2_indic[0], 39);

    meter2_indic[1] = lv_meter_add_arc(meter2, scale, 30, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_meter_set_indicator_start_value(meter2, meter2_indic[1], 40);
    lv_meter_set_indicator_end_value(meter2, meter2_indic[1], 69);

    meter2_indic[2] = lv_meter_add_arc(meter2, scale, 10, lv_palette_main(LV_PALETTE_GREEN), -20);
    lv_meter_set_indicator_start_value(meter2, meter2_indic[2], 70);
    lv_meter_set_indicator_end_value(meter2, meter2_indic[2], 99);

    meter2_timer = lv_timer_create(meter2_timer_cb, 100, meter2_indic);

    meter3 = create_meter_box(parent, "Network Speed", "Low speed", "Normal Speed", "High Speed");
    if(disp_size < DISP_LARGE) lv_obj_add_flag(lv_obj_get_parent(meter3), LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);

    /*Add a special circle to the needle's pivot*/
    lv_obj_set_style_pad_hor(meter3, 10, 0);
    lv_obj_set_style_size(meter3, 10, LV_PART_INDICATOR);
    lv_obj_set_style_radius(meter3, LV_RADIUS_CIRCLE, LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(meter3, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(meter3, lv_palette_darken(LV_PALETTE_GREY, 4), LV_PART_INDICATOR);
    lv_obj_set_style_outline_color(meter3, lv_color_white(), LV_PART_INDICATOR);
    lv_obj_set_style_outline_width(meter3, 3, LV_PART_INDICATOR);
    lv_obj_set_style_text_color(meter3, lv_palette_darken(LV_PALETTE_GREY, 1), LV_PART_TICKS);

    scale = lv_meter_add_scale(meter3);
    lv_meter_set_scale_range(meter3, scale, 10, 60, 220, 360 - 220);
    lv_meter_set_scale_ticks(meter3, scale, 21, 3, 17, lv_color_white());
    lv_meter_set_scale_major_ticks(meter3, scale, 4, 4, 22, lv_color_white(), 15);

    indic = lv_meter_add_arc(meter3, scale, 10, lv_palette_main(LV_PALETTE_RED), 0);
    lv_meter_set_indicator_start_value(meter3, indic, 0);
    lv_meter_set_indicator_end_value(meter3, indic, 20);

    indic = lv_meter_add_scale_lines(meter3, scale, lv_palette_darken(LV_PALETTE_RED, 3), lv_palette_darken(LV_PALETTE_RED,
                                                                                                            3), true, 0);
    lv_meter_set_indicator_start_value(meter3, indic, 0);
    lv_meter_set_indicator_end_value(meter3, indic, 20);

    indic = lv_meter_add_arc(meter3, scale, 12, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_meter_set_indicator_start_value(meter3, indic, 20);
    lv_meter_set_indicator_end_value(meter3, indic, 40);

    indic = lv_meter_add_scale_lines(meter3, scale, lv_palette_darken(LV_PALETTE_BLUE, 3),
                                     lv_palette_darken(LV_PALETTE_BLUE, 3), true, 0);
    lv_meter_set_indicator_start_value(meter3, indic, 20);
    lv_meter_set_indicator_end_value(meter3, indic, 40);

    indic = lv_meter_add_arc(meter3, scale, 10, lv_palette_main(LV_PALETTE_GREEN), 0);
    lv_meter_set_indicator_start_value(meter3, indic, 40);
    lv_meter_set_indicator_end_value(meter3, indic, 60);

    indic = lv_meter_add_scale_lines(meter3, scale, lv_palette_darken(LV_PALETTE_GREEN, 3),
                                     lv_palette_darken(LV_PALETTE_GREEN, 3), true, 0);
    lv_meter_set_indicator_start_value(meter3, indic, 40);
    lv_meter_set_indicator_end_value(meter3, indic, 60);

    indic = lv_meter_add_needle_line(meter3, scale, 4, lv_palette_darken(LV_PALETTE_GREY, 4), -25);

    lv_obj_t * mbps_label = lv_label_create(meter3);
    lv_label_set_text(mbps_label, "-");
    lv_obj_add_style(mbps_label, &style_title, 0);

    lv_obj_t * mbps_unit_label = lv_label_create(meter3);
    lv_label_set_text(mbps_unit_label, "Mbps");

    lv_anim_init(&a);
    lv_anim_set_values(&a, 10, 60);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_exec_cb(&a, meter3_anim_cb);
    lv_anim_set_var(&a, indic);
    lv_anim_set_time(&a, 4100);
    lv_anim_set_playback_time(&a, 800);
    lv_anim_start(&a);

    lv_obj_update_layout(parent);
    if(disp_size == DISP_MEDIUM) {
        lv_obj_set_size(meter1, 200, 200);
        lv_obj_set_size(meter2, 200, 200);
        lv_obj_set_size(meter3, 200, 200);
    }
    else {
        lv_coord_t meter_w = lv_obj_get_width(meter1);
        lv_obj_set_height(meter1, meter_w);
        lv_obj_set_height(meter2, meter_w);
        lv_obj_set_height(meter3, meter_w);
    }

    lv_obj_align(mbps_label, LV_ALIGN_TOP_MID, 10, lv_pct(55));
    lv_obj_align_to(mbps_unit_label, mbps_label, LV_ALIGN_OUT_RIGHT_BOTTOM, 10, 0);
}

void shop_create(lv_obj_t * parent)
{
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_ROW_WRAP);

    lv_obj_t * panel1 = lv_obj_create(parent);
    lv_obj_set_size(panel1, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_style_pad_bottom(panel1, 30, 0);

    lv_obj_t * title = lv_label_create(panel1);
    lv_label_set_text(title, "Monthly Summary");
    lv_obj_add_style(title, &style_title, 0);

    lv_obj_t * date = lv_label_create(panel1);
    lv_label_set_text(date, "8-15 July, 2021");
    lv_obj_add_style(date, &style_text_muted, 0);

    lv_obj_t * amount = lv_label_create(panel1);
    lv_label_set_text(amount, "$27,123.25");
    lv_obj_add_style(amount, &style_title, 0);

    lv_obj_t * hint = lv_label_create(panel1);
    lv_label_set_text(hint, LV_SYMBOL_UP" 17% growth this week");
    lv_obj_set_style_text_color(hint, lv_palette_main(LV_PALETTE_GREEN), 0);

    chart3 = lv_chart_create(panel1);
    lv_chart_set_axis_tick(chart3, LV_CHART_AXIS_PRIMARY_Y, 0, 0, 6, 1, true, 80);
    lv_chart_set_axis_tick(chart3, LV_CHART_AXIS_PRIMARY_X, 0, 0, 7, 1, true, 50);
    lv_chart_set_type(chart3, LV_CHART_TYPE_BAR);
    lv_chart_set_div_line_count(chart3, 6, 0);
    lv_chart_set_point_count(chart3, 7);
    lv_obj_add_event_cb(chart3, shop_chart_event_cb, LV_EVENT_ALL, NULL);

    ser4 = lv_chart_add_series(chart3, lv_theme_get_color_primary(chart3), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_next_value(chart3, ser4, lv_rand(60, 90));
    lv_chart_set_next_value(chart3, ser4, lv_rand(60, 90));
    lv_chart_set_next_value(chart3, ser4, lv_rand(60, 90));
    lv_chart_set_next_value(chart3, ser4, lv_rand(60, 90));
    lv_chart_set_next_value(chart3, ser4, lv_rand(60, 90));
    lv_chart_set_next_value(chart3, ser4, lv_rand(60, 90));
    lv_chart_set_next_value(chart3, ser4, lv_rand(60, 90));
    lv_chart_set_next_value(chart3, ser4, lv_rand(60, 90));
    lv_chart_set_next_value(chart3, ser4, lv_rand(60, 90));
    lv_chart_set_next_value(chart3, ser4, lv_rand(60, 90));
    lv_chart_set_next_value(chart3, ser4, lv_rand(60, 90));
    lv_chart_set_next_value(chart3, ser4, lv_rand(60, 90));

    if(disp_size == DISP_LARGE) {
        static lv_coord_t grid1_col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        static lv_coord_t grid1_row_dsc[] = {
            LV_GRID_CONTENT,  /*Title*/
            LV_GRID_CONTENT,  /*Sub title*/
            20,               /*Spacer*/
            LV_GRID_CONTENT,  /*Amount*/
            LV_GRID_CONTENT,  /*Hint*/
            LV_GRID_TEMPLATE_LAST
        };

        lv_obj_set_size(chart3, lv_pct(100), lv_pct(100));
        lv_obj_set_style_pad_column(chart3, LV_DPX(30), 0);

        lv_obj_set_grid_dsc_array(panel1, grid1_col_dsc, grid1_row_dsc);
        lv_obj_set_grid_cell(title, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 0, 1);
        lv_obj_set_grid_cell(date, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 1, 1);
        lv_obj_set_grid_cell(amount, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 3, 1);
        lv_obj_set_grid_cell(hint, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 4, 1);
        lv_obj_set_grid_cell(chart3, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 5);
    }
    else if(disp_size == DISP_MEDIUM) {
        static lv_coord_t grid1_col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        static lv_coord_t grid1_row_dsc[] = {
            LV_GRID_CONTENT,  /*Title + Date*/
            LV_GRID_CONTENT,  /*Amount + Hint*/
            200,              /*Chart*/
            LV_GRID_TEMPLATE_LAST
        };

        lv_obj_update_layout(panel1);
        lv_obj_set_width(chart3, lv_obj_get_content_width(panel1) - 20);
        lv_obj_set_style_pad_column(chart3, LV_DPX(30), 0);

        lv_obj_set_grid_dsc_array(panel1, grid1_col_dsc, grid1_row_dsc);
        lv_obj_set_grid_cell(title, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);
        lv_obj_set_grid_cell(date, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);
        lv_obj_set_grid_cell(amount, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);
        lv_obj_set_grid_cell(hint, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);
        lv_obj_set_grid_cell(chart3, LV_GRID_ALIGN_END, 0, 2, LV_GRID_ALIGN_STRETCH, 2, 1);
    }
    else if(disp_size == DISP_SMALL) {
        static lv_coord_t grid1_col_dsc[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        static lv_coord_t grid1_row_dsc[] = {
            LV_GRID_CONTENT,  /*Title*/
            LV_GRID_CONTENT,  /*Date*/
            LV_GRID_CONTENT,  /*Amount*/
            LV_GRID_CONTENT,  /*Hint*/
            LV_GRID_CONTENT,  /*Chart*/
            LV_GRID_TEMPLATE_LAST
        };

        lv_obj_set_width(chart3, LV_PCT(95));
        lv_obj_set_height(chart3, LV_VER_RES - 70);
        lv_obj_set_style_max_height(chart3, 300, 0);
        lv_chart_set_zoom_x(chart3, 512);

        lv_obj_set_grid_dsc_array(panel1, grid1_col_dsc, grid1_row_dsc);
        lv_obj_set_grid_cell(title, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 0, 1);
        lv_obj_set_grid_cell(date, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 1, 1);
        lv_obj_set_grid_cell(amount, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 2, 1);
        lv_obj_set_grid_cell(hint, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 3, 1);
        lv_obj_set_grid_cell(chart3, LV_GRID_ALIGN_END, 0, 1, LV_GRID_ALIGN_START, 4, 1);
    }

    lv_obj_t * list = lv_obj_create(parent);
    if(disp_size == DISP_SMALL) {
        lv_obj_add_flag(list, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
        lv_obj_set_height(list, LV_PCT(100));
    }
    else {
        lv_obj_set_height(list, LV_PCT(100));
        lv_obj_set_style_max_height(list, 300, 0);
    }

    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_grow(list, 1);
    lv_obj_add_flag(list, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);

    title = lv_label_create(list);
    lv_label_set_text(title, "Top products");
    lv_obj_add_style(title, &style_title, 0);

    LV_IMG_DECLARE(img_clothes);
    create_shop_item(list, &img_clothes, "Blue jeans", "Clothes", "$722");
    create_shop_item(list, &img_clothes, "Blue jeans", "Clothes", "$411");
    create_shop_item(list, &img_clothes, "Blue jeans", "Clothes", "$917");
    create_shop_item(list, &img_clothes, "Blue jeans", "Clothes", "$64");
    create_shop_item(list, &img_clothes, "Blue jeans", "Clothes", "$805");

    lv_obj_t * notifications = lv_obj_create(parent);
    if(disp_size == DISP_SMALL) {
        lv_obj_add_flag(notifications, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
        lv_obj_set_height(notifications, LV_PCT(100));
    }
    else  {
        lv_obj_set_height(notifications, LV_PCT(100));
        lv_obj_set_style_max_height(notifications, 300, 0);
    }

    lv_obj_set_flex_flow(notifications, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_grow(notifications, 1);

    title = lv_label_create(notifications);
    lv_label_set_text(title, "Notification");
    lv_obj_add_style(title, &style_title, 0);

    lv_obj_t * cb;
    cb = lv_checkbox_create(notifications);
    lv_checkbox_set_text(cb, "Item purchased");

    cb = lv_checkbox_create(notifications);
    lv_checkbox_set_text(cb, "New connection");

    cb = lv_checkbox_create(notifications);
    lv_checkbox_set_text(cb, "New subscriber");
    lv_obj_add_state(cb, LV_STATE_CHECKED);

    cb = lv_checkbox_create(notifications);
    lv_checkbox_set_text(cb, "New message");
    lv_obj_add_state(cb, LV_STATE_DISABLED);

    cb = lv_checkbox_create(notifications);
    lv_checkbox_set_text(cb, "Milestone reached");
    lv_obj_add_state(cb, LV_STATE_CHECKED | LV_STATE_DISABLED);

    cb = lv_checkbox_create(notifications);
    lv_checkbox_set_text(cb, "Out of stock");

}

static void color_changer_create(lv_obj_t * parent)
{
    static lv_palette_t palette[] = {
        LV_PALETTE_BLUE, LV_PALETTE_GREEN, LV_PALETTE_BLUE_GREY,  LV_PALETTE_ORANGE,
        LV_PALETTE_RED, LV_PALETTE_PURPLE, LV_PALETTE_TEAL, _LV_PALETTE_LAST
    };

    lv_obj_t * color_cont = lv_obj_create(parent);
    lv_obj_remove_style_all(color_cont);
    lv_obj_set_flex_flow(color_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(color_cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_add_flag(color_cont, LV_OBJ_FLAG_FLOATING);

    lv_obj_set_style_bg_color(color_cont, lv_color_white(), 0);
    lv_obj_set_style_pad_right(color_cont, disp_size == DISP_SMALL ? LV_DPX(47) : LV_DPX(55), 0);
    lv_obj_set_style_bg_opa(color_cont, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(color_cont, LV_RADIUS_CIRCLE, 0);

    if(disp_size == DISP_SMALL) lv_obj_set_size(color_cont, LV_DPX(52), LV_DPX(52));
    else lv_obj_set_size(color_cont, LV_DPX(60), LV_DPX(60));

    lv_obj_align(color_cont, LV_ALIGN_BOTTOM_RIGHT, - LV_DPX(10),  - LV_DPX(10));

    uint32_t i;
    for(i = 0; palette[i] != _LV_PALETTE_LAST; i++) {
        lv_obj_t * c = lv_btn_create(color_cont);
        lv_obj_set_style_bg_color(c, lv_palette_main(palette[i]), 0);
        lv_obj_set_style_radius(c, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_opa(c, LV_OPA_TRANSP, 0);
        lv_obj_set_size(c, 20, 20);
        lv_obj_add_event_cb(c, color_event_cb, LV_EVENT_ALL, &palette[i]);
        lv_obj_clear_flag(c, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    }

    lv_obj_t * btn = lv_btn_create(parent);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_FLOATING | LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_bg_color(btn, lv_color_white(), LV_STATE_CHECKED);
    lv_obj_set_style_pad_all(btn, 10, 0);
    lv_obj_set_style_radius(btn, LV_RADIUS_CIRCLE, 0);
    lv_obj_add_event_cb(btn, color_changer_event_cb, LV_EVENT_ALL, color_cont);
    lv_obj_set_style_shadow_width(btn, 0, 0);
    lv_obj_set_style_bg_img_src(btn, LV_SYMBOL_TINT, 0);

    if(disp_size == DISP_SMALL) {
        lv_obj_set_size(btn, LV_DPX(42), LV_DPX(42));
        lv_obj_align(btn, LV_ALIGN_BOTTOM_RIGHT, -LV_DPX(15), -LV_DPX(15));
    }
    else {
        lv_obj_set_size(btn, LV_DPX(50), LV_DPX(50));
        lv_obj_align(btn, LV_ALIGN_BOTTOM_RIGHT, -LV_DPX(15), -LV_DPX(15));
    }
}

static void color_changer_anim_cb(void * var, int32_t v)
{
    lv_obj_t * obj = var;
    lv_coord_t max_w = lv_obj_get_width(lv_obj_get_parent(obj)) - LV_DPX(20);
    lv_coord_t w;

    if(disp_size == DISP_SMALL) {
        w = lv_map(v, 0, 256, LV_DPX(52), max_w);
        lv_obj_set_width(obj, w);
        lv_obj_align(obj, LV_ALIGN_BOTTOM_RIGHT, - LV_DPX(10),  - LV_DPX(10));
    }
    else {
        w = lv_map(v, 0, 256, LV_DPX(60), max_w);
        lv_obj_set_width(obj, w);
        lv_obj_align(obj, LV_ALIGN_BOTTOM_RIGHT, - LV_DPX(10),  - LV_DPX(10));
    }

    if(v > LV_OPA_COVER) v = LV_OPA_COVER;

    uint32_t i;
    for(i = 0; i < lv_obj_get_child_cnt(obj); i++) {
        lv_obj_set_style_opa(lv_obj_get_child(obj, i), v, 0);
    }

}

static void color_changer_event_cb(lv_event_t * e)
{
    if(lv_event_get_code(e) == LV_EVENT_CLICKED) {
        lv_obj_t * color_cont = lv_event_get_user_data(e);
        if(lv_obj_get_width(color_cont) < LV_HOR_RES / 2) {
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, color_cont);
            lv_anim_set_exec_cb(&a, color_changer_anim_cb);
            lv_anim_set_values(&a, 0, 256);
            lv_anim_set_time(&a, 200);
            lv_anim_start(&a);
        }
        else {
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, color_cont);
            lv_anim_set_exec_cb(&a, color_changer_anim_cb);
            lv_anim_set_values(&a, 256, 0);
            lv_anim_set_time(&a, 200);
            lv_anim_start(&a);
        }
    }
}
static void color_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);

    if(code == LV_EVENT_FOCUSED) {
        lv_obj_t * color_cont = lv_obj_get_parent(obj);
        if(lv_obj_get_width(color_cont) < LV_HOR_RES / 2) {
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, color_cont);
            lv_anim_set_exec_cb(&a, color_changer_anim_cb);
            lv_anim_set_values(&a, 0, 256);
            lv_anim_set_time(&a, 200);
            lv_anim_start(&a);
        }
    }
    else if(code == LV_EVENT_CLICKED) {
        lv_palette_t * palette_primary = lv_event_get_user_data(e);
        lv_palette_t palette_secondary = (*palette_primary) + 3; /*Use another palette as secondary*/
        if(palette_secondary >= _LV_PALETTE_LAST) palette_secondary = 0;
#if LV_USE_THEME_DEFAULT
        lv_theme_default_init(NULL, lv_palette_main(*palette_primary), lv_palette_main(palette_secondary),
                              LV_THEME_DEFAULT_DARK, font_normal);
#endif
        lv_color_t color = lv_palette_main(*palette_primary);
        lv_style_set_text_color(&style_icon, color);
        lv_chart_set_series_color(chart1, ser1, color);
        lv_chart_set_series_color(chart2, ser3, color);
    }
}

static lv_obj_t * create_meter_box(lv_obj_t * parent, const char * title, const char * text1, const char * text2,
                                   const char * text3)
{
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_height(cont, LV_SIZE_CONTENT);
    lv_obj_set_flex_grow(cont, 1);

    lv_obj_t * title_label = lv_label_create(cont);
    lv_label_set_text(title_label, title);
    lv_obj_add_style(title_label, &style_title, 0);

    lv_obj_t * meter = lv_meter_create(cont);
    lv_obj_remove_style(meter, NULL, LV_PART_MAIN);
    lv_obj_remove_style(meter, NULL, LV_PART_INDICATOR);
    lv_obj_set_width(meter, LV_PCT(100));

    lv_obj_t * bullet1 = lv_obj_create(cont);
    lv_obj_set_size(bullet1, 13, 13);
    lv_obj_remove_style(bullet1, NULL, LV_PART_SCROLLBAR);
    lv_obj_add_style(bullet1, &style_bullet, 0);
    lv_obj_set_style_bg_color(bullet1, lv_palette_main(LV_PALETTE_RED), 0);
    lv_obj_t * label1 = lv_label_create(cont);
    lv_label_set_text(label1, text1);

    lv_obj_t * bullet2 = lv_obj_create(cont);
    lv_obj_set_size(bullet2, 13, 13);
    lv_obj_remove_style(bullet2, NULL, LV_PART_SCROLLBAR);
    lv_obj_add_style(bullet2, &style_bullet, 0);
    lv_obj_set_style_bg_color(bullet2, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_obj_t * label2 = lv_label_create(cont);
    lv_label_set_text(label2, text2);

    lv_obj_t * bullet3 = lv_obj_create(cont);
    lv_obj_set_size(bullet3, 13, 13);
    lv_obj_remove_style(bullet3,  NULL, LV_PART_SCROLLBAR);
    lv_obj_add_style(bullet3, &style_bullet, 0);
    lv_obj_set_style_bg_color(bullet3, lv_palette_main(LV_PALETTE_GREEN), 0);
    lv_obj_t * label3 = lv_label_create(cont);
    lv_label_set_text(label3, text3);

    if(disp_size == DISP_MEDIUM) {
        static lv_coord_t grid_col_dsc[] = {LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_CONTENT, LV_GRID_FR(8), LV_GRID_TEMPLATE_LAST};
        static lv_coord_t grid_row_dsc[] = {LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

        lv_obj_set_grid_dsc_array(cont, grid_col_dsc, grid_row_dsc);
        lv_obj_set_grid_cell(title_label, LV_GRID_ALIGN_START, 0, 4, LV_GRID_ALIGN_START, 0, 1);
        lv_obj_set_grid_cell(meter, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 1, 3);
        lv_obj_set_grid_cell(bullet1, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_CENTER, 2, 1);
        lv_obj_set_grid_cell(bullet2, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_CENTER, 3, 1);
        lv_obj_set_grid_cell(bullet3, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_CENTER, 4, 1);
        lv_obj_set_grid_cell(label1, LV_GRID_ALIGN_STRETCH, 3, 1, LV_GRID_ALIGN_CENTER, 2, 1);
        lv_obj_set_grid_cell(label2, LV_GRID_ALIGN_STRETCH, 3, 1, LV_GRID_ALIGN_CENTER, 3, 1);
        lv_obj_set_grid_cell(label3, LV_GRID_ALIGN_STRETCH, 3, 1, LV_GRID_ALIGN_CENTER, 4, 1);
    }
    else {
        static lv_coord_t grid_col_dsc[] = {LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        static lv_coord_t grid_row_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
        lv_obj_set_grid_dsc_array(cont, grid_col_dsc, grid_row_dsc);
        lv_obj_set_grid_cell(title_label, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 0, 1);
        lv_obj_set_grid_cell(meter, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 1, 1);
        lv_obj_set_grid_cell(bullet1, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 2, 1);
        lv_obj_set_grid_cell(bullet2, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 3, 1);
        lv_obj_set_grid_cell(bullet3, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 4, 1);
        lv_obj_set_grid_cell(label1, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_START, 2, 1);
        lv_obj_set_grid_cell(label2, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_START, 3, 1);
        lv_obj_set_grid_cell(label3, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_START, 4, 1);
    }

    return meter;

}

static lv_obj_t * create_shop_item(lv_obj_t * parent, const void * img_src, const char * name, const char * category,
                                   const char * price)
{
    static lv_coord_t grid_col_dsc[] = {LV_GRID_CONTENT, 5, LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t grid_row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_grid_dsc_array(cont, grid_col_dsc, grid_row_dsc);

    lv_obj_t * img = lv_img_create(cont);
    lv_img_set_src(img, img_src);
    lv_obj_set_grid_cell(img, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 0, 2);

    lv_obj_t * label;
    label = lv_label_create(cont);
    lv_label_set_text(label, name);
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_END, 0, 1);

    label = lv_label_create(cont);
    lv_label_set_text(label, category);
    lv_obj_add_style(label, &style_text_muted, 0);
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_START, 1, 1);

    label = lv_label_create(cont);
    lv_label_set_text(label, price);
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_END, 3, 1, LV_GRID_ALIGN_END, 0, 1);

    return cont;
}

static void ta_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    lv_obj_t * kb = lv_event_get_user_data(e);
    if(code == LV_EVENT_FOCUSED) {
        if(lv_indev_get_type(lv_indev_get_act()) != LV_INDEV_TYPE_KEYPAD) {
            lv_keyboard_set_textarea(kb, ta);
            lv_obj_set_style_max_height(kb, LV_HOR_RES * 2 / 3, 0);
            lv_obj_update_layout(tv);   /*Be sure the sizes are recalculated*/
            lv_obj_set_height(tv, LV_VER_RES - lv_obj_get_height(kb));
            lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
            lv_obj_scroll_to_view_recursive(ta, LV_ANIM_OFF);
        }
    }
    else if(code == LV_EVENT_DEFOCUSED) {
        lv_keyboard_set_textarea(kb, NULL);
        lv_obj_set_height(tv, LV_VER_RES);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
        lv_indev_reset(NULL, ta);

    }
    else if(code == LV_EVENT_READY || code == LV_EVENT_CANCEL) {
        lv_obj_set_height(tv, LV_VER_RES);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_state(ta, LV_STATE_FOCUSED);
        lv_indev_reset(NULL, ta);   /*To forget the last clicked object to make it focusable again*/
    }
}

static void birthday_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);

    if(code == LV_EVENT_FOCUSED) {
        if(lv_indev_get_type(lv_indev_get_act()) == LV_INDEV_TYPE_POINTER) {
            if(calendar == NULL) {
                lv_obj_add_flag(lv_layer_top(), LV_OBJ_FLAG_CLICKABLE);
                calendar = lv_calendar_create(lv_layer_top());
                lv_obj_set_style_bg_opa(lv_layer_top(), LV_OPA_50, 0);
                lv_obj_set_style_bg_color(lv_layer_top(), lv_palette_main(LV_PALETTE_GREY), 0);
                if(disp_size == DISP_SMALL) lv_obj_set_size(calendar, 180, 200);
                else if(disp_size == DISP_MEDIUM) lv_obj_set_size(calendar, 200, 220);
                else  lv_obj_set_size(calendar, 300, 330);
                lv_calendar_set_showed_date(calendar, 1990, 01);
                lv_obj_align(calendar, LV_ALIGN_CENTER, 0, 30);
                lv_obj_add_event_cb(calendar, calendar_event_cb, LV_EVENT_ALL, ta);

                lv_calendar_header_dropdown_create(calendar);
            }
        }
    }
}

static void calendar_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_user_data(e);
    lv_obj_t * obj = lv_event_get_current_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        lv_calendar_date_t d;
        lv_calendar_get_pressed_date(obj, &d);
        char buf[32];
        lv_snprintf(buf, sizeof(buf), "%02d.%02d.%d", d.day, d.month, d.year);
        lv_textarea_set_text(ta, buf);

        lv_obj_del(calendar);
        calendar = NULL;
        lv_obj_clear_flag(lv_layer_top(), LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_style_bg_opa(lv_layer_top(), LV_OPA_TRANSP, 0);
    }
}

static void slider_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);

    if(code == LV_EVENT_REFR_EXT_DRAW_SIZE) {
        lv_coord_t * s = lv_event_get_param(e);
        *s = LV_MAX(*s, 60);
    }
    else if(code == LV_EVENT_DRAW_PART_END) {
        lv_obj_draw_part_dsc_t * dsc = lv_event_get_param(e);
        if(dsc->part == LV_PART_KNOB && lv_obj_has_state(obj, LV_STATE_PRESSED)) {
            char buf[8];
            lv_snprintf(buf, sizeof(buf), "%"LV_PRId32, lv_slider_get_value(obj));

            lv_point_t text_size;
            lv_txt_get_size(&text_size, buf, font_normal, 0, 0, LV_COORD_MAX, LV_TEXT_FLAG_NONE);

            lv_area_t txt_area;
            txt_area.x1 = dsc->draw_area->x1 + lv_area_get_width(dsc->draw_area) / 2 - text_size.x / 2;
            txt_area.x2 = txt_area.x1 + text_size.x;
            txt_area.y2 = dsc->draw_area->y1 - 10;
            txt_area.y1 = txt_area.y2 - text_size.y;

            lv_area_t bg_area;
            bg_area.x1 = txt_area.x1 - LV_DPX(8);
            bg_area.x2 = txt_area.x2 + LV_DPX(8);
            bg_area.y1 = txt_area.y1 - LV_DPX(8);
            bg_area.y2 = txt_area.y2 + LV_DPX(8);

            lv_draw_rect_dsc_t rect_dsc;
            lv_draw_rect_dsc_init(&rect_dsc);
            rect_dsc.bg_color = lv_palette_darken(LV_PALETTE_GREY, 3);
            rect_dsc.radius = LV_DPX(5);
            lv_draw_rect(dsc->draw_ctx, &rect_dsc, &bg_area);

            lv_draw_label_dsc_t label_dsc;
            lv_draw_label_dsc_init(&label_dsc);
            label_dsc.color = lv_color_white();
            label_dsc.font = font_normal;
            lv_draw_label(dsc->draw_ctx, &label_dsc, &txt_area, buf, NULL);
        }
    }
}

static void chart_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);

    if(code == LV_EVENT_PRESSED || code == LV_EVENT_RELEASED) {
        lv_obj_invalidate(obj); /*To make the value boxes visible*/
    }
    else if(code == LV_EVENT_DRAW_PART_BEGIN) {
        lv_obj_draw_part_dsc_t * dsc = lv_event_get_param(e);
        /*Set the markers' text*/
        if(dsc->part == LV_PART_TICKS && dsc->id == LV_CHART_AXIS_PRIMARY_X) {
            if(lv_chart_get_type(obj) == LV_CHART_TYPE_BAR) {
                const char * month[] = {"I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX", "X", "XI", "XII"};
                lv_snprintf(dsc->text, dsc->text_length, "%s", month[dsc->value]);
            }
            else {
                const char * month[] = {"Jan", "Febr", "March", "Apr", "May", "Jun", "July", "Aug", "Sept", "Oct", "Nov", "Dec"};
                lv_snprintf(dsc->text, dsc->text_length, "%s", month[dsc->value]);
            }
        }

        /*Add the faded area before the lines are drawn */
        else if(dsc->part == LV_PART_ITEMS) {
#if LV_DRAW_COMPLEX
            /*Add  a line mask that keeps the area below the line*/
            if(dsc->p1 && dsc->p2) {
                lv_draw_mask_line_param_t line_mask_param;
                lv_draw_mask_line_points_init(&line_mask_param, dsc->p1->x, dsc->p1->y, dsc->p2->x, dsc->p2->y,
                                              LV_DRAW_MASK_LINE_SIDE_BOTTOM);
                int16_t line_mask_id = lv_draw_mask_add(&line_mask_param, NULL);

                /*Add a fade effect: transparent bottom covering top*/
                lv_coord_t h = lv_obj_get_height(obj);
                lv_draw_mask_fade_param_t fade_mask_param;
                lv_draw_mask_fade_init(&fade_mask_param, &obj->coords, LV_OPA_COVER, obj->coords.y1 + h / 8, LV_OPA_TRANSP,
                                       obj->coords.y2);
                int16_t fade_mask_id = lv_draw_mask_add(&fade_mask_param, NULL);

                /*Draw a rectangle that will be affected by the mask*/
                lv_draw_rect_dsc_t draw_rect_dsc;
                lv_draw_rect_dsc_init(&draw_rect_dsc);
                draw_rect_dsc.bg_opa = LV_OPA_50;
                draw_rect_dsc.bg_color = dsc->line_dsc->color;

                lv_area_t obj_clip_area;
                _lv_area_intersect(&obj_clip_area, dsc->draw_ctx->clip_area, &obj->coords);
                const lv_area_t * clip_area_ori = dsc->draw_ctx->clip_area;
                dsc->draw_ctx->clip_area = &obj_clip_area;
                lv_area_t a;
                a.x1 = dsc->p1->x;
                a.x2 = dsc->p2->x - 1;
                a.y1 = LV_MIN(dsc->p1->y, dsc->p2->y);
                a.y2 = obj->coords.y2;
                lv_draw_rect(dsc->draw_ctx, &draw_rect_dsc, &a);
                dsc->draw_ctx->clip_area = clip_area_ori;
                /*Remove the masks*/
                lv_draw_mask_remove_id(line_mask_id);
                lv_draw_mask_remove_id(fade_mask_id);
            }
#endif

            const lv_chart_series_t * ser = dsc->sub_part_ptr;

            if(lv_chart_get_pressed_point(obj) == dsc->id) {
                if(lv_chart_get_type(obj) == LV_CHART_TYPE_LINE) {
                    dsc->rect_dsc->outline_color = lv_color_white();
                    dsc->rect_dsc->outline_width = 2;
                }
                else {
                    dsc->rect_dsc->shadow_color = ser->color;
                    dsc->rect_dsc->shadow_width = 15;
                    dsc->rect_dsc->shadow_spread = 0;
                }

                char buf[8];
                lv_snprintf(buf, sizeof(buf), "%"LV_PRIu32, dsc->value);

                lv_point_t text_size;
                lv_txt_get_size(&text_size, buf, font_normal, 0, 0, LV_COORD_MAX, LV_TEXT_FLAG_NONE);

                lv_area_t txt_area;
                if(lv_chart_get_type(obj) == LV_CHART_TYPE_BAR) {
                    txt_area.y2 = dsc->draw_area->y1 - LV_DPX(15);
                    txt_area.y1 = txt_area.y2 - text_size.y;
                    if(ser == lv_chart_get_series_next(obj, NULL)) {
                        txt_area.x1 = dsc->draw_area->x1 + lv_area_get_width(dsc->draw_area) / 2;
                        txt_area.x2 = txt_area.x1 + text_size.x;
                    }
                    else {
                        txt_area.x2 = dsc->draw_area->x1 + lv_area_get_width(dsc->draw_area) / 2;
                        txt_area.x1 = txt_area.x2 - text_size.x;
                    }
                }
                else {
                    txt_area.x1 = dsc->draw_area->x1 + lv_area_get_width(dsc->draw_area) / 2 - text_size.x / 2;
                    txt_area.x2 = txt_area.x1 + text_size.x;
                    txt_area.y2 = dsc->draw_area->y1 - LV_DPX(15);
                    txt_area.y1 = txt_area.y2 - text_size.y;
                }

                lv_area_t bg_area;
                bg_area.x1 = txt_area.x1 - LV_DPX(8);
                bg_area.x2 = txt_area.x2 + LV_DPX(8);
                bg_area.y1 = txt_area.y1 - LV_DPX(8);
                bg_area.y2 = txt_area.y2 + LV_DPX(8);

                lv_draw_rect_dsc_t rect_dsc;
                lv_draw_rect_dsc_init(&rect_dsc);
                rect_dsc.bg_color = ser->color;
                rect_dsc.radius = LV_DPX(5);
                lv_draw_rect(dsc->draw_ctx, &rect_dsc, &bg_area);

                lv_draw_label_dsc_t label_dsc;
                lv_draw_label_dsc_init(&label_dsc);
                label_dsc.color = lv_color_white();
                label_dsc.font = font_normal;
                lv_draw_label(dsc->draw_ctx, &label_dsc, &txt_area,  buf, NULL);
            }
            else {
                dsc->rect_dsc->outline_width = 0;
                dsc->rect_dsc->shadow_width = 0;
            }
        }
    }
}

static void shop_chart_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_DRAW_PART_BEGIN) {
        lv_obj_draw_part_dsc_t * dsc = lv_event_get_param(e);
        /*Set the markers' text*/
        if(dsc->part == LV_PART_TICKS && dsc->id == LV_CHART_AXIS_PRIMARY_X) {
            const char * month[] = {"Jan", "Febr", "March", "Apr", "May", "Jun", "July", "Aug", "Sept", "Oct", "Nov", "Dec"};
            lv_snprintf(dsc->text, dsc->text_length, "%s", month[dsc->value]);
        }
        if(dsc->part == LV_PART_ITEMS) {
            dsc->rect_dsc->bg_opa = LV_OPA_TRANSP; /*We will draw it later*/
        }
    }
    if(code == LV_EVENT_DRAW_PART_END) {
        lv_obj_draw_part_dsc_t * dsc = lv_event_get_param(e);
        /*Add the faded area before the lines are drawn */
        if(dsc->part == LV_PART_ITEMS) {
            static const uint32_t devices[10] = {32, 43, 21, 56, 29, 36, 19, 25, 62, 35};
            static const uint32_t clothes[10] = {12, 19, 23, 31, 27, 32, 32, 11, 21, 32};
            static const uint32_t services[10] = {56, 38, 56, 13, 44, 32, 49, 64, 17, 33};

            lv_draw_rect_dsc_t draw_rect_dsc;
            lv_draw_rect_dsc_init(&draw_rect_dsc);

            lv_coord_t h = lv_area_get_height(dsc->draw_area);

            lv_area_t a;
            a.x1 = dsc->draw_area->x1;
            a.x2 = dsc->draw_area->x2;

            a.y1 = dsc->draw_area->y1;
            a.y2 = a.y1 + 4 + (devices[dsc->id] * h) / 100; /*+4 to overlap the radius*/
            draw_rect_dsc.bg_color = lv_palette_main(LV_PALETTE_RED);
            draw_rect_dsc.radius = 4;
            lv_draw_rect(dsc->draw_ctx, &draw_rect_dsc, &a);

            a.y1 = a.y2 - 4;                                    /*-4 to overlap the radius*/
            a.y2 = a.y1 + (clothes[dsc->id] * h) / 100;
            draw_rect_dsc.bg_color = lv_palette_main(LV_PALETTE_BLUE);
            draw_rect_dsc.radius = 0;
            lv_draw_rect(dsc->draw_ctx, &draw_rect_dsc, &a);

            a.y1 = a.y2;
            a.y2 = a.y1 + (services[dsc->id] * h) / 100;
            draw_rect_dsc.bg_color = lv_palette_main(LV_PALETTE_GREEN);
            lv_draw_rect(dsc->draw_ctx, &draw_rect_dsc, &a);
        }
    }
}

static void meter1_indic1_anim_cb(void * var, int32_t v)
{
    lv_meter_set_indicator_end_value(meter1, var, v);

    lv_obj_t * card = lv_obj_get_parent(meter1);
    lv_obj_t * label = lv_obj_get_child(card, -5);
    lv_label_set_text_fmt(label, "Revenue: %"LV_PRId32" %%", v);
}

static void meter1_indic2_anim_cb(void * var, int32_t v)
{
    lv_meter_set_indicator_end_value(meter1, var, v);

    lv_obj_t * card = lv_obj_get_parent(meter1);
    lv_obj_t * label = lv_obj_get_child(card, -3);
    lv_label_set_text_fmt(label, "Sales: %"LV_PRId32" %%", v);

}

static void meter1_indic3_anim_cb(void * var, int32_t v)
{
    lv_meter_set_indicator_end_value(meter1, var, v);

    lv_obj_t * card = lv_obj_get_parent(meter1);
    lv_obj_t * label = lv_obj_get_child(card, -1);
    lv_label_set_text_fmt(label, "Costs: %"LV_PRId32" %%", v);
}

static void meter2_timer_cb(lv_timer_t * timer)
{
    lv_meter_indicator_t ** indics = timer->user_data;

    static bool down1 = false;
    static bool down2 = false;
    static bool down3 = false;

    if(down1) {
        session_desktop -= 137;
        if(session_desktop < 1400) down1 = false;
    }
    else {
        session_desktop += 116;
        if(session_desktop > 4500) down1 = true;
    }

    if(down2) {
        session_tablet -= 3;
        if(session_tablet < 1400) down2 = false;
    }
    else {
        session_tablet += 9;
        if(session_tablet > 4500) down2 = true;
    }

    if(down3) {
        session_mobile -= 57;
        if(session_mobile < 1400) down3 = false;
    }
    else {
        session_mobile += 76;
        if(session_mobile > 4500) down3 = true;
    }

    uint32_t all = session_desktop + session_tablet + session_mobile;
    uint32_t pct1 = (session_desktop * 97) / all;
    uint32_t pct2 = (session_tablet * 97) / all;

    lv_meter_set_indicator_start_value(meter2, indics[0], 0);
    lv_meter_set_indicator_end_value(meter2, indics[0], pct1);

    lv_meter_set_indicator_start_value(meter2, indics[1], pct1 + 1);
    lv_meter_set_indicator_end_value(meter2, indics[1], pct1 + 1 + pct2);

    lv_meter_set_indicator_start_value(meter2, indics[2], pct1 + 1 + pct2 + 1);
    lv_meter_set_indicator_end_value(meter2, indics[2], 99);

    lv_obj_t * card = lv_obj_get_parent(meter2);
    lv_obj_t * label;

    label = lv_obj_get_child(card, -5);
    lv_label_set_text_fmt(label, "Desktop: %"LV_PRIu32, session_desktop);

    label = lv_obj_get_child(card, -3);
    lv_label_set_text_fmt(label, "Tablet: %"LV_PRIu32, session_tablet);

    label = lv_obj_get_child(card, -1);
    lv_label_set_text_fmt(label, "Mobile: %"LV_PRIu32, session_mobile);
}

static void meter3_anim_cb(void * var, int32_t v)
{
    lv_meter_set_indicator_value(meter3, var, v);

    lv_obj_t * label = lv_obj_get_child(meter3, 0);
    lv_label_set_text_fmt(label, "%"LV_PRId32, v);
}

#endif
