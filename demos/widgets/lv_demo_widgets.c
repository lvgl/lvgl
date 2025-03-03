/**
 * @file lv_demo_widgets.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_widgets.h"
#include "../../lvgl_private.h"

#if LV_USE_DEMO_WIDGETS

#if LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN && LV_MEM_SIZE < (38ul * 1024ul)
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

static lv_obj_t * create_scale_box(lv_obj_t * parent, const char * title, const char * text1, const char * text2,
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
static void scale1_indic1_anim_cb(void * var, int32_t v);
static void scale2_timer_cb(lv_timer_t * timer);
static void scale3_anim_cb(void * var, int32_t v);
static void scale3_size_changed_event_cb(lv_event_t * e);
static void scroll_anim_y_cb(void * var, int32_t v);
static void scroll_anim_y_cb(void * var, int32_t v);
static void delete_timer_event_cb(lv_event_t * e);
static void slideshow_anim_completed_cb(lv_anim_t * a_old);
static void scale3_delete_event_cb(lv_event_t * e);
static void tabview_delete_event_cb(lv_event_t * e);

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

static lv_obj_t * scale1;
static lv_obj_t * scale2;
static lv_obj_t * scale3;

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

static lv_style_t scale3_section1_main_style;
static lv_style_t scale3_section1_indicator_style;
static lv_style_t scale3_section1_tick_style;
static lv_style_t scale3_section2_main_style;
static lv_style_t scale3_section2_indicator_style;
static lv_style_t scale3_section2_tick_style;
static lv_style_t scale3_section3_main_style;
static lv_style_t scale3_section3_indicator_style;
static lv_style_t scale3_section3_tick_style;

static lv_obj_t * scale3_needle;
static lv_obj_t * scale3_mbps_label;

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

    int32_t tab_h;
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

    tv = lv_tabview_create(lv_screen_active());
    lv_tabview_set_tab_bar_size(tv, tab_h);
    lv_obj_add_event_cb(tv, tabview_delete_event_cb, LV_EVENT_DELETE, NULL);

    lv_obj_set_style_text_font(lv_screen_active(), font_normal, 0);

    lv_obj_t * t1 = lv_tabview_add_tab(tv, "Profile");
    lv_obj_t * t2 = lv_tabview_add_tab(tv, "Analytics");
    lv_obj_t * t3 = lv_tabview_add_tab(tv, "Shop");

    if(disp_size == DISP_LARGE) {
        lv_obj_t * tab_bar = lv_tabview_get_tab_bar(tv);
        lv_obj_set_style_pad_left(tab_bar, LV_HOR_RES / 2, 0);
        lv_obj_t * logo = lv_image_create(tab_bar);
        lv_obj_add_flag(logo, LV_OBJ_FLAG_IGNORE_LAYOUT);
        LV_IMAGE_DECLARE(img_lvgl_logo);
        lv_image_set_src(logo, &img_lvgl_logo);
        lv_obj_align(logo, LV_ALIGN_LEFT_MID, -LV_HOR_RES / 2 + 25, 0);

        lv_obj_t * label = lv_label_create(tab_bar);
        lv_obj_add_style(label, &style_title, 0);
        lv_obj_add_flag(label, LV_OBJ_FLAG_IGNORE_LAYOUT);
        lv_label_set_text_fmt(label, "LVGL v%d.%d.%d", lv_version_major(), lv_version_minor(), lv_version_patch());
        lv_obj_align_to(label, logo, LV_ALIGN_OUT_RIGHT_TOP, 10, 0);

        label = lv_label_create(tab_bar);
        lv_label_set_text_static(label, "Widgets demo");
        lv_obj_add_flag(label, LV_OBJ_FLAG_IGNORE_LAYOUT);
        lv_obj_add_style(label, &style_text_muted, 0);
        lv_obj_align_to(label, logo, LV_ALIGN_OUT_RIGHT_BOTTOM, 10, 0);
    }

    profile_create(t1);
    analytics_create(t2);
    shop_create(t3);

    color_changer_create(tv);
}

void lv_demo_widgets_start_slideshow(void)
{
    lv_obj_update_layout(tv);

    lv_obj_t * cont = lv_tabview_get_content(tv);

    lv_obj_t * tab = lv_obj_get_child(cont, 0);

    int32_t v = lv_obj_get_scroll_bottom(tab);
    uint32_t t = lv_anim_speed(lv_display_get_dpi(NULL));
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, scroll_anim_y_cb);
    lv_anim_set_duration(&a, t);
    lv_anim_set_reverse_duration(&a, t);
    lv_anim_set_values(&a, 0, v);
    lv_anim_set_var(&a, tab);
    lv_anim_set_completed_cb(&a, slideshow_anim_completed_cb);
    lv_anim_start(&a);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void profile_create(lv_obj_t * parent)
{
    lv_obj_t * panel1 = lv_obj_create(parent);
    lv_obj_set_height(panel1, LV_SIZE_CONTENT);

    LV_IMAGE_DECLARE(img_demo_widgets_avatar);
    lv_obj_t * avatar = lv_image_create(panel1);
    lv_image_set_src(avatar, &img_demo_widgets_avatar);

    lv_obj_t * name = lv_label_create(panel1);
    lv_label_set_text_static(name, "Elena Smith");
    lv_obj_add_style(name, &style_title, 0);

    lv_obj_t * dsc = lv_label_create(panel1);
    lv_obj_add_style(dsc, &style_text_muted, 0);
    lv_label_set_text_static(dsc, "This is a short description of me. Take a look at my profile!");
    lv_label_set_long_mode(dsc, LV_LABEL_LONG_MODE_WRAP);

    lv_obj_t * email_icn = lv_label_create(panel1);
    lv_obj_add_style(email_icn, &style_icon, 0);
    lv_label_set_text_static(email_icn, LV_SYMBOL_ENVELOPE);

    lv_obj_t * email_label = lv_label_create(panel1);
    lv_label_set_text_static(email_label, "elena@smith.com");

    lv_obj_t * call_icn = lv_label_create(panel1);
    lv_obj_add_style(call_icn, &style_icon, 0);
    lv_label_set_text_static(call_icn, LV_SYMBOL_CALL);

    lv_obj_t * call_label = lv_label_create(panel1);
    lv_label_set_text_static(call_label, "+79 246 123 4567");

    lv_obj_t * log_out_btn = lv_button_create(panel1);
    lv_obj_set_height(log_out_btn, LV_SIZE_CONTENT);

    lv_obj_t * label = lv_label_create(log_out_btn);
    lv_label_set_text_static(label, "Log out");
    lv_obj_center(label);

    lv_obj_t * invite_btn = lv_button_create(panel1);
    lv_obj_add_state(invite_btn, LV_STATE_DISABLED);
    lv_obj_set_height(invite_btn, LV_SIZE_CONTENT);

    label = lv_label_create(invite_btn);
    lv_label_set_text_static(label, "Invite");
    lv_obj_center(label);

    /*Create a keyboard*/
    lv_obj_t * kb = lv_keyboard_create(lv_screen_active());
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);

    /*Create the second panel*/
    lv_obj_t * panel2 = lv_obj_create(parent);
    lv_obj_set_height(panel2, LV_SIZE_CONTENT);

    lv_obj_t * panel2_title = lv_label_create(panel2);
    lv_label_set_text_static(panel2_title, "Your profile");
    lv_obj_add_style(panel2_title, &style_title, 0);

    lv_obj_t * user_name_label = lv_label_create(panel2);
    lv_label_set_text_static(user_name_label, "User name");
    lv_obj_add_style(user_name_label, &style_text_muted, 0);

    lv_obj_t * user_name = lv_textarea_create(panel2);
    lv_textarea_set_one_line(user_name, true);
    lv_textarea_set_placeholder_text(user_name, "Your name");
    lv_obj_add_event_cb(user_name, ta_event_cb, LV_EVENT_ALL, kb);

    lv_obj_t * password_label = lv_label_create(panel2);
    lv_label_set_text_static(password_label, "Password");
    lv_obj_add_style(password_label, &style_text_muted, 0);

    lv_obj_t * password = lv_textarea_create(panel2);
    lv_textarea_set_one_line(password, true);
    lv_textarea_set_password_mode(password, true);
    lv_textarea_set_placeholder_text(password, "Min. 8 chars.");
    lv_obj_add_event_cb(password, ta_event_cb, LV_EVENT_ALL, kb);

    lv_obj_t * gender_label = lv_label_create(panel2);
    lv_label_set_text_static(gender_label, "Gender");
    lv_obj_add_style(gender_label, &style_text_muted, 0);

    lv_obj_t * gender = lv_dropdown_create(panel2);
    lv_dropdown_set_options_static(gender, "Male\nFemale\nOther");

    lv_obj_t * birthday_label = lv_label_create(panel2);
    lv_label_set_text_static(birthday_label, "Birthday");
    lv_obj_add_style(birthday_label, &style_text_muted, 0);

    lv_obj_t * birthdate = lv_textarea_create(panel2);
    lv_textarea_set_one_line(birthdate, true);
    lv_obj_add_event_cb(birthdate, birthday_event_cb, LV_EVENT_ALL, NULL);

    /*Create the third panel*/
    lv_obj_t * panel3 = lv_obj_create(parent);
    lv_obj_t * panel3_title = lv_label_create(panel3);
    lv_label_set_text_static(panel3_title, "Your skills");
    lv_obj_add_style(panel3_title, &style_title, 0);

    lv_obj_t * experience_label = lv_label_create(panel3);
    lv_label_set_text_static(experience_label, "Experience");
    lv_obj_add_style(experience_label, &style_text_muted, 0);

    lv_obj_t * slider1 = lv_slider_create(panel3);
    lv_obj_set_width(slider1, LV_PCT(95));
    lv_obj_add_event_cb(slider1, slider_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_flag(slider1, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);
    lv_obj_refresh_ext_draw_size(slider1);

    lv_obj_t * team_player_label = lv_label_create(panel3);
    lv_label_set_text_static(team_player_label, "Team player");
    lv_obj_add_style(team_player_label, &style_text_muted, 0);

    lv_obj_t * sw1 = lv_switch_create(panel3);

    lv_obj_t * hard_working_label = lv_label_create(panel3);
    lv_label_set_text_static(hard_working_label, "Hard-working");
    lv_obj_add_style(hard_working_label, &style_text_muted, 0);

    lv_obj_t * sw2 = lv_switch_create(panel3);

    if(disp_size == DISP_LARGE) {
        static int32_t grid_main_col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        static int32_t grid_main_row_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};

        /*Create the top panel*/
        static int32_t grid_1_col_dsc[] = {LV_GRID_CONTENT, 5, LV_GRID_CONTENT, LV_GRID_FR(2), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        static int32_t grid_1_row_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, 10, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};

        static int32_t grid_2_col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        static int32_t grid_2_row_dsc[] = {
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
        static int32_t grid_main_col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        static int32_t grid_main_row_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};

        /*Create the top panel*/
        static int32_t grid_1_col_dsc[] = {LV_GRID_CONTENT, 1, LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        static int32_t grid_1_row_dsc[] = {
            LV_GRID_CONTENT, /*Name*/
            LV_GRID_CONTENT, /*Description*/
            LV_GRID_CONTENT, /*Email*/
            -20,
            LV_GRID_CONTENT, /*Phone*/
            LV_GRID_CONTENT, /*Buttons*/
            LV_GRID_TEMPLATE_LAST
        };

        static int32_t grid_2_col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        static int32_t grid_2_row_dsc[] = {
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
        static int32_t grid_main_col_dsc[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        static int32_t grid_main_row_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
        lv_obj_set_grid_dsc_array(parent, grid_main_col_dsc, grid_main_row_dsc);

        /*Create the top panel*/
        static int32_t grid_1_col_dsc[] = {LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        static int32_t grid_1_row_dsc[] = {LV_GRID_CONTENT, /*Avatar*/
                                           LV_GRID_CONTENT, /*Name*/
                                           LV_GRID_CONTENT, /*Description*/
                                           LV_GRID_CONTENT, /*Email*/
                                           LV_GRID_CONTENT, /*Phone number*/
                                           LV_GRID_CONTENT, /*Button1*/
                                           LV_GRID_CONTENT, /*Button2*/
                                           LV_GRID_TEMPLATE_LAST
                                          };

        lv_obj_set_grid_dsc_array(panel1, grid_1_col_dsc, grid_1_row_dsc);

        static int32_t grid_2_col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        static int32_t grid_2_row_dsc[] = {
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

static lv_obj_t * create_chart_with_scales(lv_obj_t * parent, const char * title,  const char * hor_text[])
{
    static const int32_t col_dsc[] = {40, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static const int32_t row_dsc[] = {LV_GRID_CONTENT, LV_GRID_FR(1), 40, LV_GRID_TEMPLATE_LAST};

    lv_obj_set_grid_dsc_array(parent, col_dsc, row_dsc);
    lv_obj_set_style_pad_column(parent, 0, 0);
    lv_obj_set_style_pad_row(parent, 0, 0);

    lv_obj_t * label = lv_label_create(parent);
    lv_label_set_text(label, title);
    lv_obj_add_style(label, &style_title, 0);
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);

    lv_obj_t * scale_ver = lv_scale_create(parent);
    lv_scale_set_mode(scale_ver, LV_SCALE_MODE_VERTICAL_LEFT);
    lv_obj_set_grid_cell(scale_ver, LV_GRID_ALIGN_END, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
    lv_scale_set_total_tick_count(scale_ver, 11);
    lv_scale_set_major_tick_every(scale_ver, 2);
    lv_scale_set_range(scale_ver, 0, 100);

    lv_obj_t * wrapper = lv_obj_create(parent);
    lv_obj_remove_style(wrapper, NULL, LV_PART_MAIN);
    lv_obj_set_grid_dsc_array(wrapper, NULL, NULL);
    lv_obj_set_grid_cell(wrapper, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 1, 2);
    lv_obj_set_scroll_dir(wrapper, LV_DIR_HOR);

    lv_obj_t * chart = lv_chart_create(wrapper);
    lv_group_add_obj(lv_group_get_default(), chart);
    lv_obj_add_flag(chart, LV_OBJ_FLAG_SCROLL_ON_FOCUS | LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);
    lv_chart_set_div_line_count(chart, 0, 12);
    lv_chart_set_point_count(chart, 12);
    lv_obj_set_grid_cell(chart, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
    lv_obj_set_style_border_width(chart, 0, 0);
    lv_obj_add_event_cb(chart, chart_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_set_width(chart, lv_pct(200));
    lv_obj_set_style_radius(chart, 0, 0);

    lv_obj_t * scale_hor = lv_scale_create(wrapper);
    lv_scale_set_mode(scale_hor, LV_SCALE_MODE_HORIZONTAL_BOTTOM);
    lv_obj_set_grid_cell(scale_hor, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 1, 1);
    lv_scale_set_total_tick_count(scale_hor, 12);
    lv_scale_set_major_tick_every(scale_hor, 1);
    lv_scale_set_text_src(scale_hor, hor_text);
    lv_obj_set_width(scale_hor, lv_pct(200));
    lv_obj_set_height(scale_hor, 40);
    lv_obj_set_style_pad_hor(scale_hor, lv_obj_get_style_pad_left(chart, 0), 0);
    lv_obj_set_style_pad_ver(scale_ver, lv_obj_get_style_pad_top(chart, 0), 0);
    return chart;
}

static void analytics_create(lv_obj_t * parent)
{
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_ROW);

    lv_obj_t * chart1_cont = lv_obj_create(parent);
    lv_obj_set_height(chart1_cont, lv_pct(100));
    lv_obj_set_style_max_height(chart1_cont, 300, 0);
    lv_obj_set_flex_grow(chart1_cont, 1);

    static const char * chart1_texts[] = {"Jan", "Feb", "March", "April", "May", "Jun", "July", "Aug", "Sept", "Oct", "Nov", "Dec", NULL};
    chart1 = create_chart_with_scales(chart1_cont, "Unique visitors", chart1_texts);

    ser1 = lv_chart_add_series(chart1, lv_theme_get_color_primary(chart1), LV_CHART_AXIS_PRIMARY_Y);
    uint32_t i;
    for(i = 0; i < 12; i++) {
        lv_chart_set_next_value(chart1, ser1, lv_rand(10, 80));
    }

    lv_obj_t * chart2_cont = lv_obj_create(parent);
    lv_obj_set_height(chart2_cont, lv_pct(100));
    lv_obj_set_style_max_height(chart2_cont, 300, 0);
    lv_obj_set_flex_grow(chart2_cont, 1);
    lv_obj_add_flag(chart2_cont, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);

    static const char * chart2_texts[] = {"I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX", "X", "XI", "XII", NULL};
    chart2 = create_chart_with_scales(chart2_cont, "Monthly revenue", chart2_texts);
    lv_chart_set_type(chart2, LV_CHART_TYPE_BAR);

    ser2 = lv_chart_add_series(chart2, lv_palette_main(LV_PALETTE_GREY), LV_CHART_AXIS_PRIMARY_Y);
    ser3 = lv_chart_add_series(chart2, lv_theme_get_color_primary(chart2), LV_CHART_AXIS_PRIMARY_Y);
    for(i = 0; i < 12; i++) {
        lv_chart_set_next_value(chart2, ser2, lv_rand(10, 80));
        lv_chart_set_next_value(chart2, ser3, lv_rand(10, 80));
    }
    lv_obj_t * chart2_hor_scale = lv_obj_get_sibling(chart2, 1);
    lv_obj_set_style_pad_hor(chart2_hor_scale, lv_chart_get_first_point_center_offset(chart2), 0);

    /*Create all 3 scales first to have their size resolved*/
    scale1 = create_scale_box(parent, "Monthly Target", "Revenue: -", "Sales: -", "Costs: -");
    lv_obj_add_flag(lv_obj_get_parent(scale1), LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);

    scale2 = create_scale_box(parent, "Sessions", "Desktop: -", "Tablet: -", "Mobile: -");
    if(disp_size < DISP_LARGE) lv_obj_add_flag(lv_obj_get_parent(scale2), LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);

    scale3 = create_scale_box(parent, "Network Speed", "Low speed", "Normal Speed", "High Speed");
    if(disp_size < DISP_LARGE) lv_obj_add_flag(lv_obj_get_parent(scale3), LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);

    lv_obj_update_layout(parent);
    int32_t scale_w;
    if(disp_size == DISP_MEDIUM) {
        scale_w = 200;
        lv_obj_set_size(scale1, scale_w, scale_w);
        lv_obj_set_size(scale2, scale_w, scale_w);
        lv_obj_set_size(scale3, scale_w, scale_w);
    }
    else {
        scale_w = lv_obj_get_width(scale1);
        lv_obj_set_height(scale1, scale_w);
        lv_obj_set_height(scale2, scale_w);
        lv_obj_set_height(scale3, scale_w);
    }

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_values(&a, 20, 100);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);

    /*Scale 1*/
    lv_scale_set_mode(scale1, LV_SCALE_MODE_ROUND_OUTER);
    lv_obj_set_style_pad_all(scale1, 30, 0);
    lv_obj_t * arc;
    arc = lv_arc_create(scale1);
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
    lv_obj_remove_style(arc, NULL, LV_PART_MAIN);
    lv_obj_set_size(arc, lv_pct(100), lv_pct(100));
    lv_obj_set_style_arc_opa(arc, 0, 0);
    lv_obj_set_style_arc_width(arc, 15, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc, lv_palette_main(LV_PALETTE_BLUE), LV_PART_INDICATOR);
    lv_obj_remove_flag(arc, LV_OBJ_FLAG_CLICKABLE);

    lv_anim_set_exec_cb(&a, scale1_indic1_anim_cb);
    lv_anim_set_var(&a, arc);
    lv_anim_set_duration(&a, 4100);
    lv_anim_set_reverse_duration(&a, 2700);
    lv_anim_start(&a);

    arc = lv_arc_create(scale1);
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
    lv_obj_set_size(arc, lv_pct(100), lv_pct(100));
    lv_obj_set_style_margin_all(arc, 20, 0);
    lv_obj_set_style_arc_opa(arc, 0, 0);
    lv_obj_set_style_arc_width(arc, 15, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc, lv_palette_main(LV_PALETTE_RED), LV_PART_INDICATOR);
    lv_obj_remove_flag(arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_center(arc);

    lv_anim_set_exec_cb(&a, scale1_indic1_anim_cb);
    lv_anim_set_var(&a, arc);
    lv_anim_set_duration(&a, 2600);
    lv_anim_set_reverse_duration(&a, 3200);
    lv_anim_start(&a);

    arc = lv_arc_create(scale1);
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
    lv_obj_set_size(arc, lv_pct(100), lv_pct(100));
    lv_obj_set_style_margin_all(arc, 40, 0);
    lv_obj_set_style_arc_opa(arc, 0, 0);
    lv_obj_set_style_arc_width(arc, 15, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc, lv_palette_main(LV_PALETTE_GREEN), LV_PART_INDICATOR);
    lv_obj_remove_flag(arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_center(arc);

    lv_anim_set_exec_cb(&a, scale1_indic1_anim_cb);
    lv_anim_set_var(&a, arc);
    lv_anim_set_duration(&a, 2800);
    lv_anim_set_reverse_duration(&a, 1800);
    lv_anim_start(&a);

    /*Scale 2*/
    static const char * scale2_text[] = {"0", "10", "20", "30", "40", "50", "60", "70", "80", "90", NULL};
    lv_scale_set_angle_range(scale2, 360);
    lv_scale_set_text_src(scale2, scale2_text);
    lv_scale_set_total_tick_count(scale2, 11);
    lv_obj_set_style_length(scale2, 30, LV_PART_INDICATOR);
    lv_scale_set_major_tick_every(scale2, 1);
    arc = lv_arc_create(scale2);
    lv_obj_set_size(arc, lv_pct(100), lv_pct(100));
    lv_obj_set_style_margin_all(arc, 10, 0);
    lv_obj_set_style_bg_opa(arc, 0, LV_PART_KNOB);
    lv_obj_set_style_arc_opa(arc, 0, LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc, 10, LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(arc, false, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc, lv_palette_main(LV_PALETTE_BLUE), LV_PART_INDICATOR);
    lv_obj_remove_flag(arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_center(arc);

    arc = lv_arc_create(scale2);
    lv_obj_set_size(arc, lv_pct(100), lv_pct(100));
    lv_obj_set_style_margin_all(arc, 5, 0);
    lv_obj_set_style_arc_opa(arc, 0, 0);
    lv_obj_set_style_bg_opa(arc, 0, LV_PART_KNOB);
    lv_obj_set_style_arc_opa(arc, 0, LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc, 20, LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(arc, false, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc, lv_palette_main(LV_PALETTE_RED), LV_PART_INDICATOR);
    lv_obj_remove_flag(arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_center(arc);

    arc = lv_arc_create(scale2);
    lv_obj_set_size(arc, lv_pct(100), lv_pct(100));
    lv_obj_set_style_arc_opa(arc, 0, 0);
    lv_obj_set_style_bg_opa(arc, 0, LV_PART_KNOB);
    lv_obj_set_style_arc_opa(arc, 0, LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc, 30, LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(arc, false, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc, lv_palette_main(LV_PALETTE_GREEN), LV_PART_INDICATOR);
    lv_obj_remove_flag(arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_center(arc);

    lv_timer_t * scale2_timer = lv_timer_create(scale2_timer_cb, 100, scale2);
    lv_obj_add_event_cb(scale2, delete_timer_event_cb, LV_EVENT_DELETE, scale2_timer);

    /*Scale 3*/
    lv_scale_set_range(scale3, 10, 60);
    lv_scale_set_total_tick_count(scale3, 21);
    lv_scale_set_major_tick_every(scale3, 4);
    lv_obj_set_style_length(scale3, 10, LV_PART_ITEMS);
    lv_obj_set_style_length(scale3, 20, LV_PART_INDICATOR);
    lv_scale_set_angle_range(scale3, 225);
    lv_scale_set_rotation(scale3, 135);

    lv_style_init(&scale3_section1_main_style);
    lv_style_set_arc_width(&scale3_section1_main_style, 8);
    lv_style_set_arc_color(&scale3_section1_main_style, lv_palette_main(LV_PALETTE_RED));

    lv_style_init(&scale3_section1_indicator_style);
    lv_style_set_line_width(&scale3_section1_indicator_style, 4);
    lv_style_set_line_color(&scale3_section1_indicator_style, lv_palette_darken(LV_PALETTE_RED, 2));

    lv_style_init(&scale3_section1_tick_style);
    lv_style_set_line_width(&scale3_section1_tick_style, 4);
    lv_style_set_line_color(&scale3_section1_tick_style, lv_palette_darken(LV_PALETTE_RED, 2));

    lv_style_init(&scale3_section2_main_style);
    lv_style_set_arc_width(&scale3_section2_main_style, 8);
    lv_style_set_arc_color(&scale3_section2_main_style, lv_palette_main(LV_PALETTE_BLUE));

    lv_style_init(&scale3_section2_indicator_style);
    lv_style_set_line_width(&scale3_section2_indicator_style, 4);
    lv_style_set_line_color(&scale3_section2_indicator_style, lv_palette_darken(LV_PALETTE_BLUE, 2));

    lv_style_init(&scale3_section2_tick_style);
    lv_style_set_line_width(&scale3_section2_tick_style, 4);
    lv_style_set_line_color(&scale3_section2_tick_style, lv_palette_darken(LV_PALETTE_BLUE, 2));

    lv_style_init(&scale3_section3_main_style);
    lv_style_set_arc_width(&scale3_section3_main_style, 8);
    lv_style_set_arc_color(&scale3_section3_main_style, lv_palette_main(LV_PALETTE_GREEN));

    lv_style_init(&scale3_section3_indicator_style);
    lv_style_set_line_width(&scale3_section3_indicator_style, 4);
    lv_style_set_line_color(&scale3_section3_indicator_style, lv_palette_darken(LV_PALETTE_GREEN, 2));

    lv_style_init(&scale3_section3_tick_style);
    lv_style_set_line_width(&scale3_section3_tick_style, 4);
    lv_style_set_line_color(&scale3_section3_tick_style, lv_palette_darken(LV_PALETTE_GREEN, 2));

    lv_obj_add_event_cb(scale3, scale3_delete_event_cb, LV_EVENT_DELETE, NULL);

    lv_scale_section_t * section;
    section = lv_scale_add_section(scale3);
    lv_scale_set_section_range(scale3, section, 0, 20);
    lv_scale_set_section_style_main(scale3, section, &scale3_section2_main_style);
    lv_scale_set_section_style_indicator(scale3, section, &scale3_section2_indicator_style);
    lv_scale_set_section_style_items(scale3, section, &scale3_section2_tick_style);

    section = lv_scale_add_section(scale3);
    lv_scale_set_section_range(scale3, section, 40, 60);
    lv_scale_set_section_style_main(scale3, section, &scale3_section3_main_style);
    lv_scale_set_section_style_indicator(scale3, section, &scale3_section3_indicator_style);
    lv_scale_set_section_style_items(scale3, section, &scale3_section3_tick_style);

    LV_IMAGE_DECLARE(img_demo_widgets_needle);
    scale3_needle = lv_image_create(scale3);
    lv_image_set_src(scale3_needle, &img_demo_widgets_needle);
    lv_image_set_pivot(scale3_needle, 3, 4);
    lv_obj_align(scale3_needle, LV_ALIGN_CENTER, 47, -2);

    scale3_mbps_label = lv_label_create(scale3);
    lv_label_set_text(scale3_mbps_label, "-");
    lv_obj_add_style(scale3_mbps_label, &style_title, 0);

    lv_obj_t * mbps_unit_label = lv_label_create(scale3);
    lv_label_set_text(mbps_unit_label, "Mbps");

    lv_anim_init(&a);
    lv_anim_set_values(&a, 10, 60);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_exec_cb(&a, scale3_anim_cb);
    lv_anim_set_var(&a, scale3);
    lv_anim_set_duration(&a, 4100);
    lv_anim_set_reverse_duration(&a, 800);
    lv_anim_start(&a);

    lv_obj_align(scale3_mbps_label, LV_ALIGN_TOP_MID, 10, lv_pct(55));
    lv_obj_align_to(mbps_unit_label, scale3_mbps_label, LV_ALIGN_OUT_RIGHT_BOTTOM, 10, 0);

    lv_obj_add_event_cb(scale3, scale3_size_changed_event_cb, LV_EVENT_SIZE_CHANGED, NULL);
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

    if(disp_size == DISP_LARGE) {
        static int32_t grid1_col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        static int32_t grid1_row_dsc[] = {
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
        static int32_t grid1_col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        static int32_t grid1_row_dsc[] = {
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
        static int32_t grid1_col_dsc[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        static int32_t grid1_row_dsc[] = {
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

    LV_IMAGE_DECLARE(img_clothes);
    create_shop_item(list, &img_clothes, "Blue T-shirt", "Clothes", "$722");
    create_shop_item(list, &img_clothes, "Blue T-shirt", "Clothes", "$411");
    create_shop_item(list, &img_clothes, "Blue T-shirt", "Clothes", "$917");
    create_shop_item(list, &img_clothes, "Blue T-shirt", "Clothes", "$64");
    create_shop_item(list, &img_clothes, "Blue T-shirt", "Clothes", "$805");

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
        LV_PALETTE_RED, LV_PALETTE_PURPLE, LV_PALETTE_TEAL, LV_PALETTE_LAST
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
    for(i = 0; palette[i] != LV_PALETTE_LAST; i++) {
        lv_obj_t * c = lv_button_create(color_cont);
        lv_obj_set_style_bg_color(c, lv_palette_main(palette[i]), 0);
        lv_obj_set_style_radius(c, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_opa(c, LV_OPA_TRANSP, 0);
        lv_obj_set_size(c, 20, 20);
        lv_obj_add_event_cb(c, color_event_cb, LV_EVENT_ALL, &palette[i]);
        lv_obj_remove_flag(c, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    }

    lv_obj_t * btn = lv_button_create(parent);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_FLOATING | LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_bg_color(btn, lv_color_white(), LV_STATE_CHECKED);
    lv_obj_set_style_pad_all(btn, 10, 0);
    lv_obj_set_style_radius(btn, LV_RADIUS_CIRCLE, 0);
    lv_obj_add_event_cb(btn, color_changer_event_cb, LV_EVENT_ALL, color_cont);
    lv_obj_set_style_shadow_width(btn, 0, 0);
    lv_obj_set_style_bg_image_src(btn, LV_SYMBOL_TINT, 0);

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
    int32_t max_w = lv_obj_get_width(lv_obj_get_parent(obj)) - LV_DPX(20);
    int32_t w;

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
    for(i = 0; i < lv_obj_get_child_count(obj); i++) {
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
            lv_anim_set_duration(&a, 200);
            lv_anim_start(&a);
        }
        else {
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, color_cont);
            lv_anim_set_exec_cb(&a, color_changer_anim_cb);
            lv_anim_set_values(&a, 256, 0);
            lv_anim_set_duration(&a, 200);
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
            lv_anim_set_duration(&a, 200);
            lv_anim_start(&a);
        }
    }
    else if(code == LV_EVENT_CLICKED) {
        lv_palette_t * palette_primary = lv_event_get_user_data(e);
        lv_palette_t palette_secondary = (*palette_primary) + 3; /*Use another palette as secondary*/
        if(palette_secondary >= LV_PALETTE_LAST) palette_secondary = 0;
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

static lv_obj_t * create_scale_box(lv_obj_t * parent, const char * title, const char * text1, const char * text2,
                                   const char * text3)
{
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_height(cont, LV_SIZE_CONTENT);
    lv_obj_set_flex_grow(cont, 1);

    lv_obj_t * title_label = lv_label_create(cont);
    lv_label_set_text_static(title_label, title);
    lv_obj_add_style(title_label, &style_title, 0);

    lv_obj_t * scale = lv_scale_create(cont);
    lv_scale_set_mode(scale, LV_SCALE_MODE_ROUND_INNER);
    lv_scale_set_post_draw(scale, true);
    lv_obj_set_width(scale, LV_PCT(100));

    lv_obj_t * bullet1 = lv_obj_create(cont);
    lv_obj_set_size(bullet1, 13, 13);
    lv_obj_remove_style(bullet1, NULL, LV_PART_SCROLLBAR);
    lv_obj_add_style(bullet1, &style_bullet, 0);
    lv_obj_set_style_bg_color(bullet1, lv_palette_main(LV_PALETTE_RED), 0);
    lv_obj_t * label1 = lv_label_create(cont);
    lv_label_set_text_static(label1, text1);

    lv_obj_t * bullet2 = lv_obj_create(cont);
    lv_obj_set_size(bullet2, 13, 13);
    lv_obj_remove_style(bullet2, NULL, LV_PART_SCROLLBAR);
    lv_obj_add_style(bullet2, &style_bullet, 0);
    lv_obj_set_style_bg_color(bullet2, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_obj_t * label2 = lv_label_create(cont);
    lv_label_set_text_static(label2, text2);

    lv_obj_t * bullet3 = lv_obj_create(cont);
    lv_obj_set_size(bullet3, 13, 13);
    lv_obj_remove_style(bullet3,  NULL, LV_PART_SCROLLBAR);
    lv_obj_add_style(bullet3, &style_bullet, 0);
    lv_obj_set_style_bg_color(bullet3, lv_palette_main(LV_PALETTE_GREEN), 0);
    lv_obj_t * label3 = lv_label_create(cont);
    lv_label_set_text_static(label3, text3);

    if(disp_size == DISP_MEDIUM) {
        static int32_t grid_col_dsc[] = {LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_CONTENT, LV_GRID_FR(8), LV_GRID_TEMPLATE_LAST};
        static int32_t grid_row_dsc[] = {LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

        lv_obj_set_grid_dsc_array(cont, grid_col_dsc, grid_row_dsc);
        lv_obj_set_grid_cell(title_label, LV_GRID_ALIGN_START, 0, 4, LV_GRID_ALIGN_START, 0, 1);
        lv_obj_set_grid_cell(scale, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 1, 3);
        lv_obj_set_grid_cell(bullet1, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_CENTER, 2, 1);
        lv_obj_set_grid_cell(bullet2, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_CENTER, 3, 1);
        lv_obj_set_grid_cell(bullet3, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_CENTER, 4, 1);
        lv_obj_set_grid_cell(label1, LV_GRID_ALIGN_STRETCH, 3, 1, LV_GRID_ALIGN_CENTER, 2, 1);
        lv_obj_set_grid_cell(label2, LV_GRID_ALIGN_STRETCH, 3, 1, LV_GRID_ALIGN_CENTER, 3, 1);
        lv_obj_set_grid_cell(label3, LV_GRID_ALIGN_STRETCH, 3, 1, LV_GRID_ALIGN_CENTER, 4, 1);
    }
    else {
        static int32_t grid_col_dsc[] = {LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        static int32_t grid_row_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
        lv_obj_set_grid_dsc_array(cont, grid_col_dsc, grid_row_dsc);
        lv_obj_set_grid_cell(title_label, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 0, 1);
        lv_obj_set_grid_cell(scale, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 1, 1);
        lv_obj_set_grid_cell(bullet1, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 2, 1);
        lv_obj_set_grid_cell(bullet2, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 3, 1);
        lv_obj_set_grid_cell(bullet3, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 4, 1);
        lv_obj_set_grid_cell(label1, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_START, 2, 1);
        lv_obj_set_grid_cell(label2, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_START, 3, 1);
        lv_obj_set_grid_cell(label3, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_START, 4, 1);
    }
    return scale;
}

static lv_obj_t * create_shop_item(lv_obj_t * parent, const void * img_src, const char * name, const char * category,
                                   const char * price)
{
    static int32_t grid_col_dsc[] = {LV_GRID_CONTENT, 5, LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static int32_t grid_row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_grid_dsc_array(cont, grid_col_dsc, grid_row_dsc);

    lv_obj_t * img = lv_image_create(cont);
    lv_image_set_src(img, img_src);
    lv_obj_set_grid_cell(img, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 0, 2);

    lv_obj_t * label;
    label = lv_label_create(cont);
    lv_label_set_text_static(label, name);
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_END, 0, 1);

    label = lv_label_create(cont);
    lv_label_set_text_static(label, category);
    lv_obj_add_style(label, &style_text_muted, 0);
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_START, 1, 1);

    label = lv_label_create(cont);
    lv_label_set_text_static(label, price);
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_END, 3, 1, LV_GRID_ALIGN_END, 0, 1);

    return cont;
}

static void ta_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    lv_obj_t * kb = lv_event_get_user_data(e);
    if(code == LV_EVENT_FOCUSED) {
        if(lv_indev_get_type(lv_indev_active()) != LV_INDEV_TYPE_KEYPAD) {
            lv_keyboard_set_textarea(kb, ta);
            lv_obj_set_style_max_height(kb, LV_HOR_RES * 2 / 3, 0);
            lv_obj_update_layout(tv);   /*Be sure the sizes are recalculated*/
            lv_obj_set_height(tv, LV_VER_RES - lv_obj_get_height(kb));
            lv_obj_remove_flag(kb, LV_OBJ_FLAG_HIDDEN);
            lv_obj_scroll_to_view_recursive(ta, LV_ANIM_OFF);
            lv_indev_wait_release(lv_event_get_param(e));
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
        lv_indev_reset(NULL, ta);   /*To forget the last clicked object to make it focusable again*/
    }
}

static void birthday_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);

    if(code == LV_EVENT_FOCUSED) {
        if(lv_indev_get_type(lv_indev_active()) == LV_INDEV_TYPE_POINTER) {
            if(calendar == NULL) {
                lv_obj_add_flag(lv_layer_top(), LV_OBJ_FLAG_CLICKABLE);
                calendar = lv_calendar_create(lv_layer_top());
                lv_obj_set_style_bg_opa(lv_layer_top(), LV_OPA_50, 0);
                lv_obj_set_style_bg_color(lv_layer_top(), lv_palette_main(LV_PALETTE_GREY), 0);
                if(disp_size == DISP_SMALL) lv_obj_set_size(calendar, 180, 200);
                else if(disp_size == DISP_MEDIUM) lv_obj_set_size(calendar, 200, 220);
                else  lv_obj_set_size(calendar, 300, 330);
                lv_calendar_set_month_shown(calendar, 1990, 01);
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

        lv_obj_delete(calendar);
        calendar = NULL;
        lv_obj_remove_flag(lv_layer_top(), LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_style_bg_opa(lv_layer_top(), LV_OPA_TRANSP, 0);
    }
}

static void slider_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);

    if(code == LV_EVENT_REFR_EXT_DRAW_SIZE) {
        int32_t * s = lv_event_get_param(e);
        *s = LV_MAX(*s, 60);
    }
    else if(code == LV_EVENT_DRAW_TASK_ADDED) {
        lv_draw_task_t * draw_task = lv_event_get_param(e);
        if(draw_task == NULL || lv_draw_task_get_type(draw_task) != LV_DRAW_TASK_TYPE_FILL) return;
        lv_draw_rect_dsc_t * draw_rect_dsc = lv_draw_task_get_draw_dsc(draw_task);

        if(draw_rect_dsc->base.part == LV_PART_KNOB && lv_obj_has_state(obj, LV_STATE_PRESSED)) {
            char buf[8];
            lv_snprintf(buf, sizeof(buf), "%"LV_PRId32, lv_slider_get_value(obj));

            lv_point_t text_size;
            lv_text_get_size(&text_size, buf, font_normal, 0, 0, LV_COORD_MAX, LV_TEXT_FLAG_NONE);

            lv_area_t txt_area;
            lv_area_t draw_task_area;
            lv_draw_task_get_area(draw_task, &draw_task_area);
            txt_area.x1 = draw_task_area.x1 + lv_area_get_width(&draw_task_area) / 2 - text_size.x / 2;
            txt_area.x2 = txt_area.x1 + text_size.x;
            txt_area.y2 = draw_task_area.y1 - 10;
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
            lv_draw_rect(draw_rect_dsc->base.layer, &rect_dsc, &bg_area);

            lv_draw_label_dsc_t label_dsc;
            lv_draw_label_dsc_init(&label_dsc);
            label_dsc.color = lv_color_white();
            label_dsc.font = font_normal;
            label_dsc.text = buf;
            label_dsc.text_local = 1;
            lv_draw_label(draw_rect_dsc->base.layer, &label_dsc, &txt_area);
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
    else if(code == LV_EVENT_DRAW_TASK_ADDED) {
        lv_draw_task_t * draw_task = lv_event_get_param(e);
        lv_draw_dsc_base_t * base_dsc = lv_draw_task_get_draw_dsc(draw_task);

        lv_draw_line_dsc_t * draw_line_dsc = lv_draw_task_get_line_dsc(draw_task);
        if(base_dsc->part == LV_PART_ITEMS && draw_line_dsc) {
            lv_area_t obj_coords;
            lv_obj_get_coords(obj, &obj_coords);
            const lv_chart_series_t * ser = lv_chart_get_series_next(obj, NULL);
            if(base_dsc->id1 == 1) ser = lv_chart_get_series_next(obj, ser);

            lv_draw_triangle_dsc_t tri_dsc;
            lv_draw_triangle_dsc_init(&tri_dsc);
            tri_dsc.p[0].x = (int32_t)draw_line_dsc->p1.x;
            tri_dsc.p[0].y = (int32_t)draw_line_dsc->p1.y;
            tri_dsc.p[1].x = (int32_t)draw_line_dsc->p2.x;
            tri_dsc.p[1].y = (int32_t)draw_line_dsc->p2.y;
            tri_dsc.p[2].x = (int32_t)(draw_line_dsc->p1.y < draw_line_dsc->p2.y ? draw_line_dsc->p1.x : draw_line_dsc->p2.x);
            tri_dsc.p[2].y = (int32_t)LV_MAX(draw_line_dsc->p1.y, draw_line_dsc->p2.y);
            tri_dsc.grad.dir = LV_GRAD_DIR_VER;

            int32_t full_h = lv_obj_get_height(obj);
            int32_t fract_uppter = (int32_t)(LV_MIN(draw_line_dsc->p1.y, draw_line_dsc->p2.y) - obj_coords.y1) * 255 / full_h;
            int32_t fract_lower = (int32_t)(LV_MAX(draw_line_dsc->p1.y, draw_line_dsc->p2.y) - obj_coords.y1) * 255 / full_h;
            tri_dsc.grad.stops[0].color = lv_chart_get_series_color(obj, ser);
            tri_dsc.grad.stops[0].opa = 255 - fract_uppter;
            tri_dsc.grad.stops[0].frac = 0;
            tri_dsc.grad.stops[1].color = lv_chart_get_series_color(obj, ser);
            tri_dsc.grad.stops[1].opa = 255 - fract_lower;
            tri_dsc.grad.stops[1].frac = 255;

            lv_draw_triangle(base_dsc->layer, &tri_dsc);

            lv_draw_rect_dsc_t rect_dsc;
            lv_draw_rect_dsc_init(&rect_dsc);
            rect_dsc.bg_grad.dir = LV_GRAD_DIR_VER;
            rect_dsc.bg_grad.stops[0].color = lv_chart_get_series_color(obj, ser);
            rect_dsc.bg_grad.stops[0].frac = 0;
            rect_dsc.bg_grad.stops[0].opa = 255 - fract_lower;
            rect_dsc.bg_grad.stops[1].color = lv_chart_get_series_color(obj, ser);
            rect_dsc.bg_grad.stops[1].frac = 255;
            rect_dsc.bg_grad.stops[1].opa = 0;

            lv_area_t rect_area;
            rect_area.x1 = (int32_t)draw_line_dsc->p1.x;
            rect_area.x2 = (int32_t)draw_line_dsc->p2.x;
            rect_area.y1 = (int32_t)LV_MAX(draw_line_dsc->p1.y, draw_line_dsc->p2.y);
            rect_area.y2 = (int32_t)obj_coords.y2;
            lv_draw_rect(base_dsc->layer, &rect_dsc, &rect_area);
        }

        bool add_value = false;
        if(base_dsc->part == LV_PART_INDICATOR && lv_chart_get_pressed_point(obj) == base_dsc->id2) {
            if(lv_chart_get_type(obj) == LV_CHART_TYPE_LINE) {
                lv_draw_rect_dsc_t outline_dsc;
                lv_draw_rect_dsc_init(&outline_dsc);
                outline_dsc.bg_opa = LV_OPA_TRANSP;
                outline_dsc.outline_color = lv_color_white();
                outline_dsc.outline_width = 2;
                outline_dsc.radius = LV_RADIUS_CIRCLE;
                lv_area_t draw_task_area;
                lv_draw_task_get_area(draw_task, &draw_task_area);
                lv_draw_rect(base_dsc->layer, &outline_dsc, &draw_task_area);
                add_value = true;
            }
        }
        if(base_dsc->part == LV_PART_ITEMS && lv_chart_get_pressed_point(obj) == base_dsc->id2) {
            const lv_chart_series_t * ser = lv_chart_get_series_next(obj, NULL);
            if(base_dsc->id1 == 1) ser = lv_chart_get_series_next(obj, ser);

            if(lv_chart_get_type(obj) == LV_CHART_TYPE_BAR) {
                lv_draw_fill_dsc_t * fill_dsc = lv_draw_task_get_draw_dsc(draw_task);
                lv_draw_rect_dsc_t shadow_dsc;
                lv_draw_rect_dsc_init(&shadow_dsc);
                shadow_dsc.radius = fill_dsc->radius;
                shadow_dsc.bg_opa = LV_OPA_TRANSP;
                shadow_dsc.shadow_color = lv_chart_get_series_color(obj, ser);
                shadow_dsc.shadow_width = 15;
                lv_area_t draw_task_area;
                lv_draw_task_get_area(draw_task, &draw_task_area);
                lv_draw_rect(base_dsc->layer, &shadow_dsc, &draw_task_area);
                add_value = true;
            }
        }

        if(add_value) {
            const lv_chart_series_t * ser = lv_chart_get_series_next(obj, NULL);
            if(base_dsc->id1 == 1) {
                ser = lv_chart_get_series_next(obj, ser);
            }

            char buf[8];
            lv_snprintf(buf, sizeof(buf), "%"LV_PRId32, lv_chart_get_series_y_array(obj, (lv_chart_series_t *)ser)[base_dsc->id2]);

            lv_point_t text_size;
            lv_text_get_size(&text_size, buf, font_normal, 0, 0, LV_COORD_MAX, LV_TEXT_FLAG_NONE);

            lv_area_t txt_area;
            lv_area_t draw_task_area;
            lv_draw_task_get_area(draw_task, &draw_task_area);
            txt_area.y2 = draw_task_area.y1 - LV_DPX(15);
            txt_area.y1 = txt_area.y2 - text_size.y;
            txt_area.x1 = draw_task_area.x1 + (lv_area_get_width(&draw_task_area) - text_size.x) / 2;
            txt_area.x2 = txt_area.x1 + text_size.x;

            lv_area_t bg_area;
            bg_area.x1 = txt_area.x1 - LV_DPX(8);
            bg_area.x2 = txt_area.x2 + LV_DPX(8);
            bg_area.y1 = txt_area.y1 - LV_DPX(8);
            bg_area.y2 = txt_area.y2 + LV_DPX(8);

            lv_draw_rect_dsc_t rect_dsc;
            lv_draw_rect_dsc_init(&rect_dsc);
            rect_dsc.bg_color = lv_chart_get_series_color(obj, ser);
            rect_dsc.radius = LV_DPX(5);
            lv_draw_rect(base_dsc->layer, &rect_dsc, &bg_area);

            lv_draw_label_dsc_t label_dsc;
            lv_draw_label_dsc_init(&label_dsc);
            label_dsc.color = lv_color_white();
            label_dsc.font = font_normal;
            label_dsc.text = buf;
            label_dsc.text_local = true;
            lv_draw_label(base_dsc->layer, &label_dsc, &txt_area);
        }

    }
}

static void shop_chart_event_cb(lv_event_t * e)
{
    LV_UNUSED(e);
}

static void scale1_indic1_anim_cb(void * var, int32_t v)
{
    lv_arc_set_value(var, v);

    lv_obj_t * card = lv_obj_get_parent(scale1);
    lv_obj_t * label = lv_obj_get_child(card, -5);
    lv_label_set_text_fmt(label, "Revenue: %"LV_PRId32" %%", v);
}

static void scale2_timer_cb(lv_timer_t * timer)
{
    LV_UNUSED(timer);

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
    uint32_t angle1 = (session_desktop * 354) / all;
    uint32_t angle2 = (session_tablet * 354) / all;

    lv_arc_set_angles(lv_obj_get_child(scale2, 0), 0, angle1);
    lv_arc_set_angles(lv_obj_get_child(scale2, 1), angle1 + 2, angle1 + 2 + angle2);
    lv_arc_set_angles(lv_obj_get_child(scale2, 2), angle1 + 2 + angle2 + 2, 358);

    lv_obj_t * card = lv_obj_get_parent(scale2);
    lv_obj_t * label;

    label = lv_obj_get_child(card, -5);
    lv_label_set_text_fmt(label, "Desktop: %"LV_PRIu32, session_desktop);

    label = lv_obj_get_child(card, -3);
    lv_label_set_text_fmt(label, "Tablet: %"LV_PRIu32, session_tablet);

    label = lv_obj_get_child(card, -1);
    lv_label_set_text_fmt(label, "Mobile: %"LV_PRIu32, session_mobile);
}

static void scale3_anim_cb(void * var, int32_t v)
{
    lv_obj_t * needle = lv_obj_get_child(var, 0);
    lv_scale_set_image_needle_value(var, needle, v);

    lv_obj_t * label = lv_obj_get_child(var, 1);
    lv_label_set_text_fmt(label, "%"LV_PRId32, v);
}

static void scale3_size_changed_event_cb(lv_event_t * e)
{
    LV_UNUSED(e);

    /* the center of the scale is half of the smaller dimension */
    int32_t width = lv_obj_get_width(scale3);
    int32_t height = lv_obj_get_height(scale3);
    int32_t minor_dim = LV_MIN(width, height);
    int32_t minor_dim_half = minor_dim / 2;

    /* Update needle position */
    lv_obj_align(scale3_needle, LV_ALIGN_TOP_LEFT, minor_dim_half, minor_dim_half);

    /* Update labels position */
    lv_obj_align(scale3_mbps_label, LV_ALIGN_TOP_LEFT, minor_dim_half, minor_dim * 55 / 100);
}

static void scroll_anim_y_cb(void * var, int32_t v)
{
    lv_obj_scroll_to_y(var, v, LV_ANIM_OFF);
}

static void delete_timer_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_DELETE) {
        lv_timer_t * t = lv_event_get_user_data(e);
        if(t) lv_timer_delete(t);
    }
}

static void slideshow_anim_completed_cb(lv_anim_t * a_old)
{
    LV_UNUSED(a_old);

    lv_obj_t * cont = lv_tabview_get_content(tv);
    uint32_t tab_id = lv_tabview_get_tab_active(tv);
    tab_id += 1;
    if(tab_id > 2) tab_id = 0;
    lv_tabview_set_active(tv, tab_id, LV_ANIM_ON);

    lv_obj_t * tab = lv_obj_get_child(cont, tab_id);
    lv_obj_scroll_to_y(tab, 0, LV_ANIM_OFF);
    lv_obj_update_layout(tv);

    int32_t v = lv_obj_get_scroll_bottom(tab);
    uint32_t t = lv_anim_speed(lv_display_get_dpi(NULL));

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, scroll_anim_y_cb);
    lv_anim_set_duration(&a, t);
    lv_anim_set_reverse_duration(&a, t);
    lv_anim_set_values(&a, 0, v);
    lv_anim_set_var(&a, tab);
    lv_anim_set_completed_cb(&a, slideshow_anim_completed_cb);
    lv_anim_start(&a);
}

static void scale3_delete_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_DELETE) {
        lv_style_reset(&scale3_section1_main_style);
        lv_style_reset(&scale3_section1_indicator_style);
        lv_style_reset(&scale3_section1_tick_style);
        lv_style_reset(&scale3_section2_main_style);
        lv_style_reset(&scale3_section2_indicator_style);
        lv_style_reset(&scale3_section2_tick_style);
        lv_style_reset(&scale3_section3_main_style);
        lv_style_reset(&scale3_section3_indicator_style);
        lv_style_reset(&scale3_section3_tick_style);
    }
}

static void tabview_delete_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_DELETE) {
        lv_style_reset(&style_text_muted);
        lv_style_reset(&style_title);
        lv_style_reset(&style_icon);
        lv_style_reset(&style_bullet);
    }
}
#endif
