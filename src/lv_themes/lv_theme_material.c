/**
 * @file lv_theme_material.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lvgl.h" /*To see all the widgets*/

#if LV_USE_THEME_MATERIAL

/*********************
 *      DEFINES
 *********************/

/*SCREEN*/

#define COLOR_SCR        (IS_LIGHT ? lv_color_hex(0xeaeff3) : lv_color_hex(0x444b5a))
#define COLOR_SCR_TEXT   (IS_LIGHT ? lv_color_hex(0x3b3e42) : lv_color_hex(0xe7e9ec))

/*BUTTON*/
#define COLOR_BTN           (IS_LIGHT ? lv_color_hex(0xffffff) : lv_color_hex(0x586273))
#define COLOR_BTN_PR        (IS_LIGHT ? lv_color_mix(theme.color_primary, COLOR_BTN, LV_OPA_10) : lv_color_mix(theme.color_primary, COLOR_BTN, LV_OPA_30))

#define COLOR_BTN_CHK       (theme.color_primary)
#define COLOR_BTN_CHK_PR    (lv_color_darken(theme.color_primary, LV_OPA_30))
#define COLOR_BTN_DIS       (IS_LIGHT ? lv_color_hex3(0xccc) : lv_color_hex3(0x888))

#define COLOR_BTN_BORDER        theme.color_primary
#define COLOR_BTN_BORDER_PR     theme.color_primary
#define COLOR_BTN_BORDER_CHK    theme.color_primary
#define COLOR_BTN_BORDER_CHK_PR theme.color_primary
#define COLOR_BTN_BORDER_INA    (IS_LIGHT ? lv_color_hex3(0x888) : lv_color_hex(0x404040))

/*BACKGROUND*/
#define COLOR_BG            (IS_LIGHT ? lv_color_hex(0xffffff) : lv_color_hex(0x586273))
#define COLOR_BG_PR         (IS_LIGHT ? lv_color_hex(0xeeeeee) : lv_color_hex(0x494f57))
#define COLOR_BG_CHK        theme.color_primary
#define COLOR_BG_PR_CHK     lv_color_darken(theme.color_primary, LV_OPA_20)
#define COLOR_BG_DIS        COLOR_BG

#define COLOR_BG_BORDER         (IS_LIGHT ? lv_color_hex(0xd6dde3) : lv_color_hex(0x808a97))   /*dfe7ed*/
#define COLOR_BG_BORDER_PR      (IS_LIGHT ? lv_color_hex3(0xccc) : lv_color_hex(0x5f656e))
#define COLOR_BG_BORDER_CHK     (IS_LIGHT ? lv_color_hex(0x3b3e42) : lv_color_hex(0x5f656e))
#define COLOR_BG_BORDER_CHK_PR  (IS_LIGHT ? lv_color_hex(0x3b3e42) : lv_color_hex(0x5f656e))
#define COLOR_BG_BORDER_DIS     (IS_LIGHT ? lv_color_hex(0xd6dde3) : lv_color_hex(0x5f656e))

#define COLOR_BG_TEXT           (IS_LIGHT ? lv_color_hex(0x3b3e42) : lv_color_hex(0xffffff))
#define COLOR_BG_TEXT_PR        (IS_LIGHT ? lv_color_hex(0x3b3e42) : lv_color_hex(0xffffff))
#define COLOR_BG_TEXT_CHK       (IS_LIGHT ? lv_color_hex(0xffffff) : lv_color_hex(0xffffff))
#define COLOR_BG_TEXT_CHK_PR    (IS_LIGHT ? lv_color_hex(0xffffff) : lv_color_hex(0xffffff))
#define COLOR_BG_TEXT_DIS       (IS_LIGHT ? lv_color_hex3(0xaaa) : lv_color_hex3(0x999))

/*SECONDARY BACKGROUND*/
#define COLOR_BG_SEC            (IS_LIGHT ? lv_color_hex(0xd4d7d9) : lv_color_hex(0x45494d))
#define COLOR_BG_SEC_BORDER     (IS_LIGHT ? lv_color_hex(0xdfe7ed) : lv_color_hex(0x404040))
#define COLOR_BG_SEC_TEXT       (IS_LIGHT ? lv_color_hex(0x31404f) : lv_color_hex(0xa5a8ad))
#define COLOR_BG_SEC_TEXT_DIS   (IS_LIGHT ? lv_color_hex(0xaaaaaa) : lv_color_hex(0xa5a8ad))

#define TRANSITION_TIME         150
#define BORDER_WIDTH            LV_DPX(2)
#define IS_LIGHT (theme.flags & LV_THEME_MATERIAL_FLAG_LIGHT)

#define PAD_DEF (lv_disp_get_size_category(NULL) <= LV_DISP_SIZE_MEDIUM ? LV_DPX(15) : (LV_DPX(30)))

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void theme_apply(lv_obj_t * obj, lv_theme_style_t name);
static void style_init_reset(lv_style_t * style);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_theme_t theme;
static bool inited;

static lv_style_t scr;
static lv_style_t bg;
static lv_style_t bg_click;
static lv_style_t bg_sec;
static lv_style_t btn;
static lv_style_t pad_inner;
static lv_style_t pad_small;

#if LV_USE_ARC
    static lv_style_t arc_indic;
    static lv_style_t arc_bg;
#endif

#if LV_USE_BAR
    static lv_style_t bar_bg;
    static lv_style_t bar_indic;
#endif

#if LV_USE_CALENDAR
    static lv_style_t calendar_date_nums, calendar_header, calendar_daynames;
#endif

#if LV_USE_CPICKER
    static lv_style_t cpicker_bg, cpicker_indic;
#endif

#if LV_USE_CHART
    static lv_style_t chart_bg, chart_series_bg, chart_series;
#endif

#if LV_USE_CHECKBOX
    static lv_style_t cb_bg, cb_bullet;
#endif

#if LV_USE_DROPDOWN
    static lv_style_t ddlist_page, ddlist_sel;
#endif

#if LV_USE_GAUGE
    static lv_style_t gauge_main, gauge_strong, gauge_needle;
#endif

#if LV_USE_KEYBOARD
    static lv_style_t kb_bg;
#endif

#if LV_USE_LED
    static lv_style_t led;
#endif

#if LV_USE_LINEMETER
    static lv_style_t lmeter;
#endif

#if LV_USE_LIST
    static lv_style_t list_bg, list_btn;
#endif

#if LV_USE_MSGBOX
    static lv_style_t mbox_bg;
#endif

#if LV_USE_PAGE
    static lv_style_t sb;
    #if LV_USE_ANIMATION
        static lv_style_t edge_flash;
    #endif
#endif

#if LV_USE_ROLLER
    static lv_style_t roller_bg, roller_sel;
#endif

#if LV_USE_SLIDER
    static lv_style_t slider_knob, slider_bg;
#endif

#if LV_USE_SPINBOX
    static lv_style_t spinbox_cursor;
#endif

#if LV_USE_SWITCH
    static lv_style_t sw_knob;
#endif

#if LV_USE_TABLE
    static lv_style_t table_cell;
#endif

#if LV_USE_TABVIEW || LV_USE_WIN
    static lv_style_t tabview_btns, tabview_btns_bg, tabview_indic, tabview_page_scrl;
#endif

#if LV_USE_TEXTAREA
    static lv_style_t ta_cursor, ta_placeholder;
#endif

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void basic_init(void)
{
    style_init_reset(&scr);
    lv_style_set_bg_opa(&scr, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&scr, LV_STATE_DEFAULT, COLOR_SCR);
    lv_style_set_text_color(&scr, LV_STATE_DEFAULT, COLOR_SCR_TEXT);
    lv_style_set_value_color(&scr, LV_STATE_DEFAULT, COLOR_SCR_TEXT);
    lv_style_set_text_font(&scr, LV_STATE_DEFAULT, theme.font_normal);
    lv_style_set_value_font(&scr, LV_STATE_DEFAULT, theme.font_normal);

    style_init_reset(&bg);
    lv_style_set_radius(&bg, LV_STATE_DEFAULT, LV_DPX(8));
    lv_style_set_bg_opa(&bg, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&bg, LV_STATE_DEFAULT, COLOR_BG);
    lv_style_set_border_color(&bg, LV_STATE_DEFAULT, COLOR_BG_BORDER);
    lv_style_set_border_color(&bg, LV_STATE_FOCUSED, theme.color_primary);
    lv_style_set_border_color(&bg, LV_STATE_EDITED, theme.color_secondary);
    lv_style_set_border_width(&bg, LV_STATE_DEFAULT, BORDER_WIDTH);
    lv_style_set_border_post(&bg, LV_STATE_DEFAULT, true);
    lv_style_set_text_font(&bg, LV_STATE_DEFAULT, theme.font_normal);
    lv_style_set_text_color(&bg, LV_STATE_DEFAULT, COLOR_BG_TEXT);
    lv_style_set_value_font(&bg, LV_STATE_DEFAULT, theme.font_normal);
    lv_style_set_value_color(&bg, LV_STATE_DEFAULT, COLOR_BG_TEXT);
    lv_style_set_image_recolor(&bg, LV_STATE_DEFAULT, COLOR_BG_TEXT);
    lv_style_set_line_color(&bg, LV_STATE_DEFAULT, COLOR_BG_TEXT);
    lv_style_set_line_width(&bg, LV_STATE_DEFAULT, 1);
    lv_style_set_pad_left(&bg, LV_STATE_DEFAULT, PAD_DEF + BORDER_WIDTH);
    lv_style_set_pad_right(&bg, LV_STATE_DEFAULT, PAD_DEF + BORDER_WIDTH);
    lv_style_set_pad_top(&bg, LV_STATE_DEFAULT, PAD_DEF + BORDER_WIDTH);
    lv_style_set_pad_bottom(&bg, LV_STATE_DEFAULT, PAD_DEF + BORDER_WIDTH);
    lv_style_set_pad_inner(&bg, LV_STATE_DEFAULT, PAD_DEF);
    lv_style_set_transition_time(&bg, LV_STATE_DEFAULT, TRANSITION_TIME);
    lv_style_set_transition_prop_6(&bg, LV_STATE_DEFAULT, LV_STYLE_BORDER_COLOR);

    style_init_reset(&bg_sec);
    lv_style_copy(&bg_sec, &bg);
    lv_style_set_bg_color(&bg_sec, LV_STATE_DEFAULT, COLOR_BG_SEC);
    lv_style_set_border_color(&bg_sec, LV_STATE_DEFAULT, COLOR_BG_SEC_BORDER);
    lv_style_set_text_color(&bg_sec, LV_STATE_DEFAULT, COLOR_BG_SEC_TEXT);
    lv_style_set_value_color(&bg_sec, LV_STATE_DEFAULT, COLOR_BG_SEC_TEXT);
    lv_style_set_image_recolor(&bg_sec, LV_STATE_DEFAULT, COLOR_BG_SEC_TEXT);
    lv_style_set_line_color(&bg_sec, LV_STATE_DEFAULT, COLOR_BG_SEC_TEXT);

    style_init_reset(&bg_click);
    lv_style_set_bg_color(&bg_click, LV_STATE_PRESSED, COLOR_BG_PR);
    lv_style_set_bg_color(&bg_click, LV_STATE_CHECKED, COLOR_BG_CHK);
    lv_style_set_bg_color(&bg_click, LV_STATE_PRESSED | LV_STATE_CHECKED, COLOR_BG_PR_CHK);
    lv_style_set_bg_color(&bg_click, LV_STATE_DISABLED, COLOR_BG_DIS);
    lv_style_set_border_width(&bg_click, LV_STATE_CHECKED, 0);
    lv_style_set_border_color(&bg_click, LV_STATE_FOCUSED | LV_STATE_PRESSED, lv_color_darken(theme.color_primary,
                                                                                              LV_OPA_20));
    lv_style_set_border_color(&bg_click, LV_STATE_PRESSED, COLOR_BG_BORDER_PR);
    lv_style_set_border_color(&bg_click, LV_STATE_CHECKED, COLOR_BG_BORDER_CHK);
    lv_style_set_border_color(&bg_click, LV_STATE_PRESSED | LV_STATE_CHECKED, COLOR_BG_BORDER_CHK_PR);
    lv_style_set_border_color(&bg_click, LV_STATE_DISABLED, COLOR_BG_BORDER_DIS);
    lv_style_set_text_color(&bg_click, LV_STATE_PRESSED, COLOR_BG_TEXT_PR);
    lv_style_set_text_color(&bg_click, LV_STATE_CHECKED, COLOR_BG_TEXT_CHK);
    lv_style_set_text_color(&bg_click, LV_STATE_PRESSED | LV_STATE_CHECKED, COLOR_BG_TEXT_CHK_PR);
    lv_style_set_text_color(&bg_click, LV_STATE_DISABLED, COLOR_BG_TEXT_DIS);
    lv_style_set_image_recolor(&bg_click, LV_STATE_PRESSED, COLOR_BG_TEXT_PR);
    lv_style_set_image_recolor(&bg_click, LV_STATE_CHECKED, COLOR_BG_TEXT_CHK);
    lv_style_set_image_recolor(&bg_click, LV_STATE_PRESSED | LV_STATE_CHECKED, COLOR_BG_TEXT_CHK_PR);
    lv_style_set_image_recolor(&bg_click, LV_STATE_DISABLED, COLOR_BG_TEXT_DIS);
    lv_style_set_transition_prop_5(&bg_click, LV_STATE_DEFAULT, LV_STYLE_BG_COLOR);

    style_init_reset(&btn);
    lv_style_set_radius(&btn, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
    lv_style_set_bg_opa(&btn, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&btn, LV_STATE_DEFAULT, COLOR_BTN);
    lv_style_set_bg_color(&btn, LV_STATE_PRESSED, COLOR_BTN_PR);
    lv_style_set_bg_color(&btn, LV_STATE_CHECKED, COLOR_BTN_CHK);
    lv_style_set_bg_color(&btn, LV_STATE_CHECKED | LV_STATE_PRESSED, COLOR_BTN_CHK_PR);
    lv_style_set_bg_color(&btn, LV_STATE_DISABLED, COLOR_BTN);
    lv_style_set_bg_color(&btn, LV_STATE_DISABLED | LV_STATE_CHECKED, COLOR_BTN_DIS);
    lv_style_set_border_color(&btn, LV_STATE_DEFAULT, COLOR_BTN_BORDER);
    lv_style_set_border_color(&btn, LV_STATE_PRESSED, COLOR_BTN_BORDER_PR);
    lv_style_set_border_color(&btn, LV_STATE_DISABLED, COLOR_BTN_BORDER_INA);
    lv_style_set_border_width(&btn, LV_STATE_DEFAULT, BORDER_WIDTH);
    lv_style_set_border_opa(&btn, LV_STATE_CHECKED, LV_OPA_TRANSP);

    lv_style_set_text_color(&btn, LV_STATE_DEFAULT, IS_LIGHT ? lv_color_hex(0x31404f) : lv_color_hex(0xffffff));
    lv_style_set_text_color(&btn, LV_STATE_PRESSED, IS_LIGHT ? lv_color_hex(0x31404f) : lv_color_hex(0xffffff));
    lv_style_set_text_color(&btn, LV_STATE_CHECKED,  lv_color_hex(0xffffff));
    lv_style_set_text_color(&btn, LV_STATE_CHECKED | LV_STATE_PRESSED, lv_color_hex(0xffffff));
    lv_style_set_text_color(&btn, LV_STATE_DISABLED, IS_LIGHT ? lv_color_hex(0x888888) : lv_color_hex(0x888888));

    lv_style_set_image_recolor(&btn, LV_STATE_DEFAULT, IS_LIGHT ? lv_color_hex(0x31404f) : lv_color_hex(0xffffff));
    lv_style_set_image_recolor(&btn, LV_STATE_PRESSED, IS_LIGHT ? lv_color_hex(0x31404f) : lv_color_hex(0xffffff));
    lv_style_set_image_recolor(&btn, LV_STATE_PRESSED, lv_color_hex(0xffffff));
    lv_style_set_image_recolor(&btn, LV_STATE_CHECKED | LV_STATE_PRESSED, lv_color_hex(0xffffff));
    lv_style_set_image_recolor(&btn, LV_STATE_DISABLED, IS_LIGHT ? lv_color_hex(0x888888) : lv_color_hex(0x888888));

    lv_style_set_value_color(&btn, LV_STATE_DEFAULT, IS_LIGHT ? lv_color_hex(0x31404f) : lv_color_hex(0xffffff));
    lv_style_set_value_color(&btn, LV_STATE_PRESSED, IS_LIGHT ? lv_color_hex(0x31404f) : lv_color_hex(0xffffff));
    lv_style_set_value_color(&btn, LV_STATE_CHECKED,  lv_color_hex(0xffffff));
    lv_style_set_value_color(&btn, LV_STATE_CHECKED | LV_STATE_PRESSED, lv_color_hex(0xffffff));
    lv_style_set_value_color(&btn, LV_STATE_DISABLED, IS_LIGHT ? lv_color_hex(0x888888) : lv_color_hex(0x888888));

    lv_style_set_pad_left(&btn, LV_STATE_DEFAULT, LV_DPX(40));
    lv_style_set_pad_right(&btn, LV_STATE_DEFAULT, LV_DPX(40));
    lv_style_set_pad_top(&btn, LV_STATE_DEFAULT, LV_DPX(15));
    lv_style_set_pad_bottom(&btn, LV_STATE_DEFAULT, LV_DPX(15));
    lv_style_set_pad_inner(&btn, LV_STATE_DEFAULT, LV_DPX(20));
    lv_style_set_outline_width(&btn, LV_STATE_DEFAULT, 3);
    lv_style_set_outline_opa(&btn, LV_STATE_DEFAULT, LV_OPA_0);
    lv_style_set_outline_opa(&btn, LV_STATE_FOCUSED, LV_OPA_50);
    lv_style_set_outline_color(&btn, LV_STATE_DEFAULT, theme.color_primary);
    lv_style_set_outline_color(&btn, LV_STATE_EDITED, theme.color_secondary);
    lv_style_set_transition_time(&btn, LV_STATE_DEFAULT, TRANSITION_TIME);
    lv_style_set_transition_prop_4(&btn, LV_STATE_DEFAULT, LV_STYLE_BORDER_OPA);
    lv_style_set_transition_prop_5(&btn, LV_STATE_DEFAULT, LV_STYLE_BG_COLOR);
    lv_style_set_transition_prop_6(&btn, LV_STATE_DEFAULT, LV_STYLE_OUTLINE_OPA);
    lv_style_set_transition_delay(&btn, LV_STATE_DEFAULT, TRANSITION_TIME);
    lv_style_set_transition_delay(&btn, LV_STATE_PRESSED, 0);

    style_init_reset(&pad_inner);

    lv_style_set_pad_inner(&pad_inner, LV_STATE_DEFAULT,
                           lv_disp_get_size_category(NULL) <= LV_DISP_MEDIUM_LIMIT ? LV_DPX(20) : LV_DPX(40));

    style_init_reset(&pad_small);
    lv_style_int_t pad_small_value = lv_disp_get_size_category(NULL) <= LV_DISP_MEDIUM_LIMIT ? LV_DPX(10) : LV_DPX(20);
    lv_style_set_pad_left(&pad_small, LV_STATE_DEFAULT,  pad_small_value);
    lv_style_set_pad_right(&pad_small, LV_STATE_DEFAULT, pad_small_value);
    lv_style_set_pad_top(&pad_small, LV_STATE_DEFAULT,  pad_small_value);
    lv_style_set_pad_bottom(&pad_small, LV_STATE_DEFAULT, pad_small_value);
    lv_style_set_pad_inner(&pad_small, LV_STATE_DEFAULT, pad_small_value);
}

static void cont_init(void)
{
#if LV_USE_CONT != 0

#endif
}

static void btn_init(void)
{
#if LV_USE_BTN != 0

#endif
}

static void label_init(void)
{
#if LV_USE_LABEL != 0

#endif
}

static void bar_init(void)
{
#if LV_USE_BAR
    style_init_reset(&bar_bg);
    lv_style_set_radius(&bar_bg, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
    lv_style_set_bg_opa(&bar_bg, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&bar_bg, LV_STATE_DEFAULT, COLOR_BG_SEC);
    lv_style_set_value_color(&bar_bg, LV_STATE_DEFAULT, IS_LIGHT ? lv_color_hex(0x31404f) : LV_COLOR_WHITE);
    lv_style_set_outline_color(&bar_bg, LV_STATE_DEFAULT, theme.color_primary);
    lv_style_set_outline_color(&bar_bg, LV_STATE_EDITED, theme.color_secondary);
    lv_style_set_outline_opa(&bar_bg, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_style_set_outline_opa(&bar_bg, LV_STATE_FOCUSED, LV_OPA_50);
    lv_style_set_outline_width(&bar_bg, LV_STATE_DEFAULT, 3);
    lv_style_set_transition_time(&bar_bg, LV_STATE_DEFAULT, TRANSITION_TIME);
    lv_style_set_transition_prop_6(&bar_bg, LV_STATE_DEFAULT, LV_STYLE_OUTLINE_OPA);

    style_init_reset(&bar_indic);
    lv_style_set_bg_opa(&bar_indic, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_radius(&bar_indic, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
    lv_style_set_bg_color(&bar_indic, LV_STATE_DEFAULT, theme.color_primary);
    lv_style_set_bg_color(&bar_indic, LV_STATE_DISABLED, lv_color_hex3(0x888));
    lv_style_set_value_color(&bar_indic, LV_STATE_DEFAULT, IS_LIGHT ? lv_color_hex(0x41404f) : LV_COLOR_WHITE);
#endif
}

static void img_init(void)
{
#if LV_USE_IMG != 0

#endif
}

static void line_init(void)
{
#if LV_USE_LINE != 0

#endif
}

static void led_init(void)
{
#if LV_USE_LED != 0
    style_init_reset(&led);
    lv_style_set_bg_opa(&led, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&led, LV_STATE_DEFAULT, theme.color_primary);
    lv_style_set_border_width(&led, LV_STATE_DEFAULT, 2);
    lv_style_set_border_opa(&led, LV_STATE_DEFAULT, LV_OPA_50);
    lv_style_set_border_color(&led, LV_STATE_DEFAULT, lv_color_lighten(theme.color_primary, LV_OPA_30));
    lv_style_set_radius(&led, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
    lv_style_set_shadow_width(&led, LV_STATE_DEFAULT, LV_DPX(15));
    lv_style_set_shadow_color(&led, LV_STATE_DEFAULT, theme.color_primary);
    lv_style_set_shadow_spread(&led, LV_STATE_DEFAULT, LV_DPX(5));
#endif
}

static void slider_init(void)
{
#if LV_USE_SLIDER != 0
    style_init_reset(&slider_knob);
    lv_style_set_bg_opa(&slider_knob, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&slider_knob, LV_STATE_DEFAULT, IS_LIGHT ? theme.color_primary : LV_COLOR_WHITE);
    lv_style_set_value_color(&slider_knob, LV_STATE_DEFAULT, IS_LIGHT ? lv_color_hex(0x31404f) : LV_COLOR_WHITE);
    lv_style_set_radius(&slider_knob, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
    lv_style_set_pad_left(&slider_knob, LV_STATE_DEFAULT, LV_DPX(7));
    lv_style_set_pad_right(&slider_knob, LV_STATE_DEFAULT, LV_DPX(7));
    lv_style_set_pad_top(&slider_knob, LV_STATE_DEFAULT, LV_DPX(7));
    lv_style_set_pad_bottom(&slider_knob, LV_STATE_DEFAULT, LV_DPX(7));

    style_init_reset(&slider_bg);
    lv_style_set_margin_left(&slider_bg, LV_STATE_DEFAULT, LV_DPX(10));
    lv_style_set_margin_right(&slider_bg, LV_STATE_DEFAULT, LV_DPX(10));
    lv_style_set_margin_top(&slider_bg, LV_STATE_DEFAULT, LV_DPX(10));
    lv_style_set_margin_bottom(&slider_bg, LV_STATE_DEFAULT, LV_DPX(10));

#endif
}

static void switch_init(void)
{
#if LV_USE_SWITCH != 0
    style_init_reset(&sw_knob);
    lv_style_set_bg_opa(&sw_knob, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&sw_knob, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_radius(&sw_knob, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
    lv_style_set_pad_top(&sw_knob, LV_STATE_DEFAULT,    - LV_DPX(4));
    lv_style_set_pad_bottom(&sw_knob, LV_STATE_DEFAULT, - LV_DPX(4));
    lv_style_set_pad_left(&sw_knob, LV_STATE_DEFAULT,   - LV_DPX(4));
    lv_style_set_pad_right(&sw_knob, LV_STATE_DEFAULT,  - LV_DPX(4));
#endif
}

static void linemeter_init(void)
{
#if LV_USE_LINEMETER != 0
    style_init_reset(&lmeter);
    lv_style_set_radius(&lmeter, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
    lv_style_set_pad_left(&lmeter, LV_STATE_DEFAULT, LV_DPX(20));
    lv_style_set_pad_right(&lmeter, LV_STATE_DEFAULT, LV_DPX(20));
    lv_style_set_pad_top(&lmeter, LV_STATE_DEFAULT, LV_DPX(20));
    lv_style_set_pad_inner(&lmeter, LV_STATE_DEFAULT, LV_DPX(30));
    lv_style_set_scale_width(&lmeter, LV_STATE_DEFAULT, LV_DPX(25));

    lv_style_set_line_color(&lmeter, LV_STATE_DEFAULT, theme.color_primary);
    lv_style_set_scale_grad_color(&lmeter, LV_STATE_DEFAULT, theme.color_primary);
    lv_style_set_scale_end_color(&lmeter, LV_STATE_DEFAULT, lv_color_hex3(0x888));
    lv_style_set_line_width(&lmeter, LV_STATE_DEFAULT, LV_DPX(10));
    lv_style_set_scale_end_line_width(&lmeter, LV_STATE_DEFAULT, LV_DPX(7));
#endif
}

static void gauge_init(void)
{
#if LV_USE_GAUGE != 0
    style_init_reset(&gauge_main);
    lv_style_set_line_color(&gauge_main, LV_STATE_DEFAULT, lv_color_hex3(0x888));
    lv_style_set_scale_grad_color(&gauge_main, LV_STATE_DEFAULT, lv_color_hex3(0x888));
    lv_style_set_scale_end_color(&gauge_main, LV_STATE_DEFAULT, theme.color_primary);
    lv_style_set_line_width(&gauge_main, LV_STATE_DEFAULT, LV_DPX(5));
    lv_style_set_scale_end_line_width(&gauge_main, LV_STATE_DEFAULT, LV_DPX(4));
    lv_style_set_scale_end_border_width(&gauge_main, LV_STATE_DEFAULT, LV_DPX(8));
    lv_style_set_pad_left(&gauge_main, LV_STATE_DEFAULT, LV_DPX(20));
    lv_style_set_pad_right(&gauge_main, LV_STATE_DEFAULT, LV_DPX(20));
    lv_style_set_pad_top(&gauge_main, LV_STATE_DEFAULT, LV_DPX(20));
    lv_style_set_pad_inner(&gauge_main, LV_STATE_DEFAULT, LV_DPX(20));
    lv_style_set_scale_width(&gauge_main, LV_STATE_DEFAULT, LV_DPX(15));
    lv_style_set_radius(&gauge_main, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);

    style_init_reset(&gauge_strong);
    lv_style_set_line_color(&gauge_strong, LV_STATE_DEFAULT, lv_color_hex3(0x888));
    lv_style_set_scale_grad_color(&gauge_strong, LV_STATE_DEFAULT, lv_color_hex3(0x888));
    lv_style_set_scale_end_color(&gauge_strong, LV_STATE_DEFAULT, theme.color_primary);
    lv_style_set_line_width(&gauge_strong, LV_STATE_DEFAULT, LV_DPX(8));
    lv_style_set_scale_end_line_width(&gauge_strong, LV_STATE_DEFAULT, LV_DPX(8));
    lv_style_set_scale_width(&gauge_strong, LV_STATE_DEFAULT, LV_DPX(25));

    style_init_reset(&gauge_needle);
    lv_style_set_line_color(&gauge_needle, LV_STATE_DEFAULT, IS_LIGHT ? lv_color_hex(0x464b5b) : LV_COLOR_WHITE);
    lv_style_set_line_width(&gauge_needle, LV_STATE_DEFAULT, LV_DPX(8));
    lv_style_set_bg_opa(&gauge_needle, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&gauge_needle, LV_STATE_DEFAULT, IS_LIGHT ? lv_color_hex(0x464b5b) : LV_COLOR_WHITE);
    lv_style_set_radius(&gauge_needle, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
    lv_style_set_size(&gauge_needle, LV_STATE_DEFAULT, LV_DPX(30));
    lv_style_set_pad_inner(&gauge_needle, LV_STATE_DEFAULT, LV_DPX(10));
#endif
}

static void arc_init(void)
{
#if LV_USE_ARC != 0
    style_init_reset(&arc_indic);
    lv_style_set_line_color(&arc_indic, LV_STATE_DEFAULT, theme.color_primary);
    lv_style_set_line_width(&arc_indic, LV_STATE_DEFAULT, LV_DPX(25));
    lv_style_set_line_rounded(&arc_indic, LV_STATE_DEFAULT, true);

    style_init_reset(&arc_bg);
    lv_style_set_line_color(&arc_bg, LV_STATE_DEFAULT, COLOR_BG_SEC);
    lv_style_set_line_width(&arc_bg, LV_STATE_DEFAULT, LV_DPX(25));
    lv_style_set_line_rounded(&arc_bg, LV_STATE_DEFAULT, true);
#endif
}

static void spinner_init(void)
{
#if LV_USE_SPINNER != 0
#endif
}

static void chart_init(void)
{
#if LV_USE_CHART
    style_init_reset(&chart_bg);
    lv_style_set_text_color(&chart_bg, LV_STATE_DEFAULT, IS_LIGHT ? COLOR_BG_TEXT_DIS : lv_color_hex(0xa1adbd));

    style_init_reset(&chart_series_bg);
    lv_style_set_line_width(&chart_series_bg, LV_STATE_DEFAULT, LV_DPX(1));
    lv_style_set_line_dash_width(&chart_series_bg, LV_STATE_DEFAULT, LV_DPX(10));
    lv_style_set_line_dash_gap(&chart_series_bg, LV_STATE_DEFAULT, LV_DPX(10));
    lv_style_set_line_color(&chart_series_bg, LV_STATE_DEFAULT, COLOR_BG_BORDER);

    style_init_reset(&chart_series);
    lv_style_set_line_width(&chart_series, LV_STATE_DEFAULT, LV_DPX(3));
    lv_style_set_size(&chart_series, LV_STATE_DEFAULT, LV_DPX(4));
    lv_style_set_pad_inner(&chart_series, LV_STATE_DEFAULT, LV_DPX(2));     /*Space between columns*/
    lv_style_set_radius(&chart_series, LV_STATE_DEFAULT, LV_DPX(1));

#endif
}

static void calendar_init(void)
{
#if LV_USE_CALENDAR

    style_init_reset(&calendar_header);
    lv_style_set_pad_top(&calendar_header, LV_STATE_DEFAULT, PAD_DEF);
    lv_style_set_pad_left(&calendar_header, LV_STATE_DEFAULT, PAD_DEF);
    lv_style_set_pad_right(&calendar_header, LV_STATE_DEFAULT, PAD_DEF);
    lv_style_set_pad_bottom(&calendar_header, LV_STATE_DEFAULT, PAD_DEF);
    lv_style_set_text_color(&calendar_header, LV_STATE_PRESSED, IS_LIGHT ? lv_color_hex(0x888888) : LV_COLOR_WHITE);

    style_init_reset(&calendar_daynames);
    lv_style_set_text_color(&calendar_daynames, LV_STATE_DEFAULT, IS_LIGHT ? lv_color_hex(0x31404f) : lv_color_hex3(0xeee));
    lv_style_set_pad_left(&calendar_daynames, LV_STATE_DEFAULT, PAD_DEF);
    lv_style_set_pad_right(&calendar_daynames, LV_STATE_DEFAULT, PAD_DEF);
    lv_style_set_pad_bottom(&calendar_daynames, LV_STATE_DEFAULT, PAD_DEF);

    style_init_reset(&calendar_date_nums);
    lv_style_set_radius(&calendar_date_nums, LV_STATE_DEFAULT, LV_DPX(4));
    lv_style_set_text_color(&calendar_date_nums, LV_STATE_CHECKED, IS_LIGHT ? lv_color_hex(0x31404f) : LV_COLOR_WHITE);

    lv_style_set_bg_opa(&calendar_date_nums, LV_STATE_CHECKED, IS_LIGHT ? LV_OPA_20 : LV_OPA_40);
    lv_style_set_bg_opa(&calendar_date_nums, LV_STATE_PRESSED, LV_OPA_20);
    lv_style_set_bg_opa(&calendar_date_nums, LV_STATE_FOCUSED, LV_OPA_COVER);
    lv_style_set_text_color(&calendar_date_nums, LV_STATE_FOCUSED, LV_COLOR_WHITE);
    lv_style_set_bg_color(&calendar_date_nums, LV_STATE_FOCUSED, theme.color_primary);
    lv_style_set_bg_color(&calendar_date_nums, LV_STATE_DEFAULT, IS_LIGHT ? lv_color_hex(0x666666) : LV_COLOR_WHITE);
    lv_style_set_bg_color(&calendar_date_nums, LV_STATE_CHECKED, theme.color_primary);
    lv_style_set_border_width(&calendar_date_nums, LV_STATE_CHECKED, 2);
    lv_style_set_border_side(&calendar_date_nums, LV_STATE_CHECKED, LV_BORDER_SIDE_LEFT);
    lv_style_set_border_color(&calendar_date_nums, LV_STATE_CHECKED, theme.color_primary);
    lv_style_set_pad_inner(&calendar_date_nums, LV_STATE_DEFAULT, LV_DPX(3));
    lv_style_set_pad_left(&calendar_date_nums, LV_STATE_DEFAULT, PAD_DEF);
    lv_style_set_pad_right(&calendar_date_nums, LV_STATE_DEFAULT, PAD_DEF);
    lv_style_set_pad_bottom(&calendar_date_nums, LV_STATE_DEFAULT, PAD_DEF);
#endif
}

static void cpicker_init(void)
{
#if LV_USE_CPICKER
    style_init_reset(&cpicker_bg);
    lv_style_set_scale_width(&cpicker_bg, LV_STATE_DEFAULT, LV_DPX(30));
    lv_style_set_bg_opa(&cpicker_bg, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&cpicker_bg, LV_STATE_DEFAULT, COLOR_SCR);
    lv_style_set_pad_inner(&cpicker_bg, LV_STATE_DEFAULT, LV_DPX(20));
    lv_style_set_radius(&cpicker_bg, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);

    style_init_reset(&cpicker_indic);
    lv_style_set_radius(&cpicker_indic, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
    lv_style_set_bg_color(&cpicker_indic, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_bg_opa(&cpicker_indic, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_border_width(&cpicker_indic, LV_STATE_DEFAULT, 2);
    lv_style_set_border_color(&cpicker_indic, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_style_set_border_color(&cpicker_indic, LV_STATE_FOCUSED, theme.color_primary);
    lv_style_set_border_color(&cpicker_indic, LV_STATE_EDITED, theme.color_secondary);
    lv_style_set_pad_left(&cpicker_indic, LV_STATE_DEFAULT, LV_DPX(13));
    lv_style_set_pad_right(&cpicker_indic, LV_STATE_DEFAULT, LV_DPX(13));
    lv_style_set_pad_top(&cpicker_indic, LV_STATE_DEFAULT, LV_DPX(13));
    lv_style_set_pad_bottom(&cpicker_indic, LV_STATE_DEFAULT, LV_DPX(13));
#endif
}

static void checkbox_init(void)
{
#if LV_USE_CHECKBOX != 0
    style_init_reset(&cb_bg);
    lv_style_set_radius(&cb_bg, LV_STATE_DEFAULT, LV_DPX(4));
    lv_style_set_pad_inner(&cb_bg, LV_STATE_DEFAULT, LV_DPX(10));
    lv_style_set_outline_color(&cb_bg, LV_STATE_DEFAULT, theme.color_primary);
    lv_style_set_outline_opa(&cb_bg, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_style_set_outline_opa(&cb_bg, LV_STATE_FOCUSED, LV_OPA_50);
    lv_style_set_outline_width(&cb_bg, LV_STATE_DEFAULT, LV_DPX(3));
    lv_style_set_outline_pad(&cb_bg, LV_STATE_DEFAULT, LV_DPX(10));
    lv_style_set_transition_time(&cb_bg, LV_STATE_DEFAULT, TRANSITION_TIME);
    lv_style_set_transition_prop_6(&cb_bg, LV_STATE_DEFAULT, LV_STYLE_OUTLINE_OPA);

    style_init_reset(&cb_bullet);
    lv_style_set_outline_opa(&cb_bullet, LV_STATE_FOCUSED, LV_OPA_TRANSP);
    lv_style_set_radius(&cb_bullet, LV_STATE_DEFAULT, LV_DPX(4));
    lv_style_set_pattern_image(&cb_bullet, LV_STATE_CHECKED, LV_SYMBOL_OK);
    lv_style_set_pattern_recolor(&cb_bullet, LV_STATE_CHECKED, LV_COLOR_WHITE);
    lv_style_set_text_font(&cb_bullet, LV_STATE_CHECKED, theme.font_small);
    lv_style_set_pad_left(&cb_bullet, LV_STATE_DEFAULT, LV_DPX(3));
    lv_style_set_pad_right(&cb_bullet, LV_STATE_DEFAULT, LV_DPX(3));
    lv_style_set_pad_top(&cb_bullet, LV_STATE_DEFAULT, LV_DPX(3));
    lv_style_set_pad_bottom(&cb_bullet, LV_STATE_DEFAULT, LV_DPX(3));
#endif
}

static void btnmatrix_init(void)
{
}

static void keyboard_init(void)
{
#if LV_USE_KEYBOARD
    style_init_reset(&kb_bg);
    lv_style_set_radius(&kb_bg, LV_STATE_DEFAULT, 0);
    lv_style_set_border_width(&kb_bg, LV_STATE_DEFAULT, LV_DPX(4));
    lv_style_set_border_side(&kb_bg, LV_STATE_DEFAULT, LV_BORDER_SIDE_TOP);
    lv_style_set_border_color(&kb_bg, LV_STATE_DEFAULT, IS_LIGHT ? COLOR_BG_TEXT : LV_COLOR_BLACK);
    lv_style_set_border_color(&kb_bg, LV_STATE_EDITED, theme.color_secondary);
    lv_style_set_pad_left(&kb_bg, LV_STATE_DEFAULT,  LV_DPX(5));
    lv_style_set_pad_right(&kb_bg, LV_STATE_DEFAULT, LV_DPX(5));
    lv_style_set_pad_top(&kb_bg, LV_STATE_DEFAULT,  LV_DPX(5));
    lv_style_set_pad_bottom(&kb_bg, LV_STATE_DEFAULT, LV_DPX(5));
    lv_style_set_pad_inner(&kb_bg, LV_STATE_DEFAULT, LV_DPX(3));
#endif
}

static void msgbox_init(void)
{
#if LV_USE_MSGBOX
    style_init_reset(&mbox_bg);
    lv_style_set_shadow_width(&mbox_bg, LV_STATE_DEFAULT, LV_DPX(50));
    lv_style_set_shadow_color(&mbox_bg, LV_STATE_DEFAULT, IS_LIGHT ? LV_COLOR_SILVER : lv_color_hex3(0x999));

#endif
}

static void page_init(void)
{
#if LV_USE_PAGE
    style_init_reset(&sb);
    lv_style_set_bg_opa(&sb, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&sb, LV_STATE_DEFAULT, (IS_LIGHT ? lv_color_hex(0xcccfd1) : lv_color_hex(0x777f85)));
    lv_style_set_radius(&sb, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
    lv_style_set_size(&sb, LV_STATE_DEFAULT, LV_DPX(7));
    lv_style_set_pad_right(&sb, LV_STATE_DEFAULT,  LV_DPX(7));
    lv_style_set_pad_bottom(&sb, LV_STATE_DEFAULT,  LV_DPX(7));

#if  LV_USE_ANIMATION
    style_init_reset(&edge_flash);
    lv_style_set_bg_opa(&edge_flash, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&edge_flash, LV_STATE_DEFAULT,  lv_color_hex3(0x888));
#endif
#endif
}

static void textarea_init(void)
{
#if LV_USE_TEXTAREA
    style_init_reset(&ta_cursor);
    lv_style_set_border_color(&ta_cursor, LV_STATE_DEFAULT, COLOR_BG_SEC_TEXT);
    lv_style_set_border_width(&ta_cursor, LV_STATE_DEFAULT, LV_DPX(2));
    lv_style_set_pad_left(&ta_cursor, LV_STATE_DEFAULT, LV_DPX(1));
    lv_style_set_border_side(&ta_cursor, LV_STATE_DEFAULT, LV_BORDER_SIDE_LEFT);

    style_init_reset(&ta_placeholder);
    lv_style_set_text_color(&ta_placeholder, LV_STATE_DEFAULT, IS_LIGHT ? COLOR_BG_TEXT_DIS : lv_color_hex(0xa1adbd));
#endif
}

static void spinbox_init(void)
{
#if LV_USE_SPINBOX

    style_init_reset(&spinbox_cursor);
    lv_style_set_bg_opa(&spinbox_cursor, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&spinbox_cursor, LV_STATE_DEFAULT, theme.color_primary);
    lv_style_set_text_color(&spinbox_cursor, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_pad_top(&spinbox_cursor, LV_STATE_DEFAULT, LV_DPX(100));
    lv_style_set_pad_bottom(&spinbox_cursor, LV_STATE_DEFAULT, LV_DPX(100));

#endif
}

static void list_init(void)
{
#if LV_USE_LIST != 0
    style_init_reset(&list_bg);
    lv_style_set_clip_corner(&list_bg, LV_STATE_DEFAULT, true);
    lv_style_set_pad_left(&list_bg, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_right(&list_bg, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_top(&list_bg, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_bottom(&list_bg, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_inner(&list_bg, LV_STATE_DEFAULT, 0);

    style_init_reset(&list_btn);
    lv_style_set_bg_opa(&list_btn, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&list_btn, LV_STATE_DEFAULT, COLOR_BG);
    lv_style_set_bg_color(&list_btn, LV_STATE_PRESSED, COLOR_BG_PR);
    lv_style_set_bg_color(&list_btn, LV_STATE_DISABLED, COLOR_BG_DIS);
    lv_style_set_bg_color(&list_btn, LV_STATE_CHECKED, COLOR_BG_CHK);
    lv_style_set_bg_color(&list_btn, LV_STATE_CHECKED | LV_STATE_PRESSED, COLOR_BG_PR_CHK);

    lv_style_set_text_color(&list_btn, LV_STATE_DEFAULT, COLOR_BG_TEXT);
    lv_style_set_text_color(&list_btn, LV_STATE_CHECKED, COLOR_BG_TEXT_CHK);
    lv_style_set_text_color(&list_btn, LV_STATE_DISABLED, COLOR_BG_TEXT_DIS);

    lv_style_set_image_recolor(&list_btn, LV_STATE_DEFAULT, COLOR_BG_TEXT);
    lv_style_set_image_recolor(&list_btn, LV_STATE_CHECKED, COLOR_BG_TEXT_CHK);
    lv_style_set_image_recolor(&list_btn, LV_STATE_DISABLED, COLOR_BG_TEXT_DIS);

    lv_style_set_border_side(&list_btn, LV_STATE_DEFAULT, LV_BORDER_SIDE_BOTTOM);
    lv_style_set_border_color(&list_btn, LV_STATE_DEFAULT, COLOR_BG_BORDER);
    lv_style_set_border_color(&list_btn, LV_STATE_FOCUSED, theme.color_primary);
    lv_style_set_border_width(&list_btn, LV_STATE_DEFAULT, 1);

    lv_style_set_pad_left(&list_btn, LV_STATE_DEFAULT, PAD_DEF);
    lv_style_set_pad_right(&list_btn, LV_STATE_DEFAULT, PAD_DEF);
    lv_style_set_pad_top(&list_btn, LV_STATE_DEFAULT, PAD_DEF);
    lv_style_set_pad_bottom(&list_btn, LV_STATE_DEFAULT, PAD_DEF);
    lv_style_set_pad_inner(&list_btn, LV_STATE_DEFAULT, PAD_DEF);

    lv_style_set_transform_width(&list_btn, LV_STATE_DEFAULT, - PAD_DEF);
    lv_style_set_transform_width(&list_btn, LV_STATE_PRESSED, 0);
    lv_style_set_transform_width(&list_btn, LV_STATE_CHECKED, 0);
    lv_style_set_transform_width(&list_btn, LV_STATE_DISABLED, 0);

    lv_style_set_transition_time(&list_btn, LV_STATE_DEFAULT, TRANSITION_TIME);
    lv_style_set_transition_prop_6(&list_btn, LV_STATE_DEFAULT, LV_STYLE_BG_COLOR);
    lv_style_set_transition_prop_5(&list_btn, LV_STATE_DEFAULT, LV_STYLE_TRANSFORM_WIDTH);
#endif
}

static void ddlist_init(void)
{
#if LV_USE_DROPDOWN != 0

    style_init_reset(&ddlist_page);
    lv_style_set_text_line_space(&ddlist_page, LV_STATE_DEFAULT, LV_DPX(20));
    lv_style_set_clip_corner(&ddlist_page, LV_STATE_DEFAULT, true);

    style_init_reset(&ddlist_sel);
    lv_style_set_bg_opa(&ddlist_sel, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&ddlist_sel, LV_STATE_DEFAULT, theme.color_primary);
    lv_style_set_text_color(&ddlist_sel, LV_STATE_DEFAULT, IS_LIGHT ? lv_color_hex3(0xfff) : lv_color_hex3(0xfff));
    lv_style_set_bg_color(&ddlist_sel, LV_STATE_PRESSED, COLOR_BG_PR);
    lv_style_set_text_color(&ddlist_sel, LV_STATE_PRESSED, COLOR_BG_TEXT_PR);
#endif
}

static void roller_init(void)
{
#if LV_USE_ROLLER != 0
    style_init_reset(&roller_bg);
    lv_style_set_text_line_space(&roller_bg, LV_STATE_DEFAULT, LV_DPX(25));

    style_init_reset(&roller_sel);
    lv_style_set_bg_opa(&roller_sel, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&roller_sel, LV_STATE_DEFAULT, theme.color_primary);
    lv_style_set_text_color(&roller_sel, LV_STATE_DEFAULT, LV_COLOR_WHITE);
#endif
}

static void tabview_init(void)
{
#if LV_USE_TABVIEW != 0
    style_init_reset(&tabview_btns_bg);
    lv_style_set_bg_opa(&tabview_btns_bg, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&tabview_btns_bg, LV_STATE_DEFAULT, COLOR_BG);
    lv_style_set_border_color(&tabview_btns_bg, LV_STATE_DEFAULT,
                              IS_LIGHT ? lv_color_hex(0xe4eaf0) : lv_color_hex(0x3b3e42));
    lv_style_set_border_width(&tabview_btns_bg, LV_STATE_DEFAULT, LV_DPX(5));
    lv_style_set_border_side(&tabview_btns_bg, LV_STATE_DEFAULT, LV_BORDER_SIDE_BOTTOM);
    lv_style_set_text_color(&tabview_btns_bg, LV_STATE_DEFAULT, COLOR_SCR_TEXT);
    lv_style_set_text_font(&tabview_btns_bg, LV_STATE_DEFAULT, theme.font_normal);
    lv_style_set_image_recolor(&tabview_btns_bg, LV_STATE_DEFAULT, lv_color_hex(0x979a9f));
    lv_style_set_pad_top(&tabview_btns_bg, LV_STATE_DEFAULT, LV_DPX(7));
    lv_style_set_pad_left(&tabview_btns_bg, LV_STATE_DEFAULT, LV_DPX(7));
    lv_style_set_pad_right(&tabview_btns_bg, LV_STATE_DEFAULT, LV_DPX(7));

    style_init_reset(&tabview_btns);
    lv_style_set_bg_opa(&tabview_btns, LV_STATE_PRESSED, LV_OPA_50);
    lv_style_set_bg_color(&tabview_btns, LV_STATE_PRESSED, lv_color_hex3(0x888));
    lv_style_set_text_color(&tabview_btns, LV_STATE_CHECKED, COLOR_SCR_TEXT);
    lv_style_set_pad_top(&tabview_btns, LV_STATE_DEFAULT, LV_DPX(20));
    lv_style_set_pad_bottom(&tabview_btns, LV_STATE_DEFAULT, LV_DPX(20));
    lv_style_set_text_color(&tabview_btns, LV_STATE_FOCUSED, theme.color_primary);
    lv_style_set_text_color(&tabview_btns, LV_STATE_EDITED, theme.color_secondary);

    style_init_reset(&tabview_indic);
    lv_style_set_bg_opa(&tabview_indic, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&tabview_indic, LV_STATE_DEFAULT, theme.color_primary);
    lv_style_set_bg_color(&tabview_indic, LV_STATE_EDITED, theme.color_secondary);
    lv_style_set_size(&tabview_indic, LV_STATE_DEFAULT, LV_DPX(5));
    lv_style_set_radius(&tabview_indic, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);

    style_init_reset(&tabview_page_scrl);
    lv_style_set_pad_top(&tabview_page_scrl, LV_STATE_DEFAULT, PAD_DEF);
    lv_style_set_pad_bottom(&tabview_page_scrl, LV_STATE_DEFAULT, PAD_DEF);
    lv_style_set_pad_left(&tabview_page_scrl, LV_STATE_DEFAULT, PAD_DEF);
    lv_style_set_pad_right(&tabview_page_scrl, LV_STATE_DEFAULT, PAD_DEF);
    lv_style_set_pad_inner(&tabview_page_scrl, LV_STATE_DEFAULT, PAD_DEF);
#endif
}

static void tileview_init(void)
{
#if LV_USE_TILEVIEW != 0
#endif
}

static void table_init(void)
{
#if LV_USE_TABLE != 0
    style_init_reset(&table_cell);
    lv_style_set_border_color(&table_cell, LV_STATE_DEFAULT, COLOR_BG_BORDER);
    lv_style_set_border_width(&table_cell, LV_STATE_DEFAULT, 1);
    lv_style_set_border_side(&table_cell, LV_STATE_DEFAULT, LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_BOTTOM);
    lv_style_set_pad_left(&table_cell, LV_STATE_DEFAULT, PAD_DEF);
    lv_style_set_pad_right(&table_cell, LV_STATE_DEFAULT, PAD_DEF);
    lv_style_set_pad_top(&table_cell, LV_STATE_DEFAULT, PAD_DEF);
    lv_style_set_pad_bottom(&table_cell, LV_STATE_DEFAULT, PAD_DEF);

#endif
}

static void win_init(void)
{
#if LV_USE_WIN != 0

#endif
}


/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize the default
 * @param color_primary the primary color of the theme
 * @param color_secondary the secondary color for the theme
 * @param flags ORed flags starting with `LV_THEME_DEF_FLAG_...`
 * @param font_small pointer to a small font
 * @param font_normal pointer to a normal font
 * @param font_subtitle pointer to a large font
 * @param font_title pointer to a extra large font
 * @return a pointer to reference this theme later
 */
lv_theme_t * lv_theme_material_init(lv_color_t color_primary, lv_color_t color_secondary, uint32_t flags,
                                    const lv_font_t * font_small, const lv_font_t * font_normal, const lv_font_t * font_subtitle,
                                    const lv_font_t * font_title)
{

    theme.color_primary = color_primary;
    theme.color_secondary = color_secondary;
    theme.font_small = font_small;
    theme.font_normal = font_normal;
    theme.font_subtitle = font_subtitle;
    theme.font_title = font_title;
    theme.flags = flags;

    basic_init();
    cont_init();
    btn_init();
    label_init();
    bar_init();
    img_init();
    line_init();
    led_init();
    slider_init();
    switch_init();
    linemeter_init();
    gauge_init();
    arc_init();
    spinner_init();
    chart_init();
    calendar_init();
    cpicker_init();
    checkbox_init();
    btnmatrix_init();
    keyboard_init();
    msgbox_init();
    page_init();
    textarea_init();
    spinbox_init();
    list_init();
    ddlist_init();
    roller_init();
    tabview_init();
    tileview_init();
    table_init();
    win_init();

    theme.apply_xcb = theme_apply;

    inited = true;

    lv_obj_report_style_mod(NULL);

    return &theme;
}


static void theme_apply(lv_obj_t * obj, lv_theme_style_t name)
{
    lv_style_list_t * list;

    switch(name) {
        case LV_THEME_NONE:
            break;

        case LV_THEME_SCR:
            lv_obj_clean_style_list(obj, LV_OBJ_PART_MAIN);
            list = lv_obj_get_style_list(obj, LV_OBJ_PART_MAIN);
            _lv_style_list_add_style(list, &scr);
            break;
        case LV_THEME_OBJ:
            lv_obj_clean_style_list(obj, LV_OBJ_PART_MAIN);
            list = lv_obj_get_style_list(obj, LV_OBJ_PART_MAIN);
            _lv_style_list_add_style(list, &bg);
            break;
#if LV_USE_CONT
        case LV_THEME_CONT:
            lv_obj_clean_style_list(obj, LV_OBJ_PART_MAIN);
            list = lv_obj_get_style_list(obj, LV_CONT_PART_MAIN);
            _lv_style_list_add_style(list, &bg);
            break;
#endif

#if LV_USE_BTN
        case LV_THEME_BTN:
            lv_obj_clean_style_list(obj, LV_BTN_PART_MAIN);
            list = lv_obj_get_style_list(obj, LV_BTN_PART_MAIN);
            _lv_style_list_add_style(list, &btn);
            break;
#endif

#if LV_USE_BTNMATRIX
        case LV_THEME_BTNMATRIX:
            lv_obj_clean_style_list(obj, LV_BTNMATRIX_PART_BG);
            list = lv_obj_get_style_list(obj, LV_BTNMATRIX_PART_BG);
            _lv_style_list_add_style(list, &bg);
            _lv_style_list_add_style(list, &pad_small);

            lv_obj_clean_style_list(obj, LV_BTNMATRIX_PART_BTN);
            list = lv_obj_get_style_list(obj, LV_BTNMATRIX_PART_BTN);
            _lv_style_list_add_style(list, &bg);
            _lv_style_list_add_style(list, &bg_click);
            break;
#endif

#if LV_USE_KEYBOARD
        case LV_THEME_KEYBOARD:
            lv_obj_clean_style_list(obj, LV_KEYBOARD_PART_BG);
            list = lv_obj_get_style_list(obj, LV_KEYBOARD_PART_BG);
            _lv_style_list_add_style(list, &scr);
            _lv_style_list_add_style(list, &kb_bg);

            lv_obj_clean_style_list(obj, LV_KEYBOARD_PART_BTN);
            list = lv_obj_get_style_list(obj, LV_KEYBOARD_PART_BTN);
            _lv_style_list_add_style(list, &bg);
            _lv_style_list_add_style(list, &bg_click);
            break;
#endif

#if LV_USE_BAR
        case LV_THEME_BAR:
            lv_obj_clean_style_list(obj, LV_BAR_PART_BG);
            list = lv_obj_get_style_list(obj, LV_BAR_PART_BG);
            _lv_style_list_add_style(list, &bar_bg);

            lv_obj_clean_style_list(obj, LV_BAR_PART_INDIC);
            list = lv_obj_get_style_list(obj, LV_BAR_PART_INDIC);
            _lv_style_list_add_style(list, &bar_indic);
            break;
#endif

#if LV_USE_SWITCH
        case LV_THEME_SWITCH:
            lv_obj_clean_style_list(obj, LV_SWITCH_PART_BG);
            list = lv_obj_get_style_list(obj, LV_SWITCH_PART_BG);
            _lv_style_list_add_style(list, &bar_bg);

            lv_obj_clean_style_list(obj, LV_SWITCH_PART_INDIC);
            list = lv_obj_get_style_list(obj, LV_SWITCH_PART_INDIC);
            _lv_style_list_add_style(list, &bar_indic);

            lv_obj_clean_style_list(obj, LV_SWITCH_PART_KNOB);
            list = lv_obj_get_style_list(obj, LV_SWITCH_PART_KNOB);
            _lv_style_list_add_style(list, &sw_knob);
            break;
#endif

#if LV_USE_CANVAS
        case LV_THEME_CANVAS:
            lv_obj_clean_style_list(obj, LV_CANVAS_PART_MAIN);
            break;
#endif

#if LV_USE_IMG
        case LV_THEME_IMAGE:
            lv_obj_clean_style_list(obj, LV_IMG_PART_MAIN);
            break;
#endif

#if LV_USE_IMGBTN
        case LV_THEME_IMGBTN:
            lv_obj_clean_style_list(obj, LV_IMG_PART_MAIN);
            break;
#endif

#if LV_USE_LABEL
        case LV_THEME_LABEL:
            lv_obj_clean_style_list(obj, LV_LABEL_PART_MAIN);
            break;
#endif

#if LV_USE_LINE
        case LV_THEME_LINE:
            lv_obj_clean_style_list(obj, LV_LABEL_PART_MAIN);
            break;
#endif

#if LV_USE_ARC
        case LV_THEME_ARC:
            lv_obj_clean_style_list(obj, LV_ARC_PART_BG);
            list = lv_obj_get_style_list(obj, LV_ARC_PART_BG);
            _lv_style_list_add_style(list, &bg);
            _lv_style_list_add_style(list, &arc_bg);

            lv_obj_clean_style_list(obj, LV_ARC_PART_INDIC);
            list = lv_obj_get_style_list(obj, LV_ARC_PART_INDIC);
            _lv_style_list_add_style(list, &arc_indic);
            break;
#endif

#if LV_USE_SPINNER
        case LV_THEME_SPINNER:
            lv_obj_clean_style_list(obj, LV_SPINNER_PART_BG);
            list = lv_obj_get_style_list(obj, LV_SPINNER_PART_BG);
            _lv_style_list_add_style(list, &arc_bg);

            lv_obj_clean_style_list(obj, LV_SPINNER_PART_INDIC);
            list = lv_obj_get_style_list(obj, LV_SPINNER_PART_INDIC);
            _lv_style_list_add_style(list, &arc_indic);
            break;
#endif

#if LV_USE_SLIDER
        case LV_THEME_SLIDER:
            lv_obj_clean_style_list(obj, LV_SLIDER_PART_BG);
            list = lv_obj_get_style_list(obj, LV_SLIDER_PART_BG);
            _lv_style_list_add_style(list, &bar_bg);
            _lv_style_list_add_style(list, &slider_bg);

            lv_obj_clean_style_list(obj, LV_SLIDER_PART_INDIC);
            list = lv_obj_get_style_list(obj, LV_SLIDER_PART_INDIC);
            _lv_style_list_add_style(list, &bar_indic);

            lv_obj_clean_style_list(obj, LV_SLIDER_PART_KNOB);
            list = lv_obj_get_style_list(obj, LV_SLIDER_PART_KNOB);
            _lv_style_list_add_style(list, &slider_knob);
            break;
#endif

#if LV_USE_CHECKBOX
        case LV_THEME_CHECKBOX:
            lv_obj_clean_style_list(obj, LV_CHECKBOX_PART_BG);
            list = lv_obj_get_style_list(obj, LV_CHECKBOX_PART_BG);
            _lv_style_list_add_style(list, &cb_bg);

            lv_obj_clean_style_list(obj, LV_CHECKBOX_PART_BULLET);
            list = lv_obj_get_style_list(obj, LV_CHECKBOX_PART_BULLET);
            _lv_style_list_add_style(list, &btn);
            _lv_style_list_add_style(list, &cb_bullet);
            break;
#endif

#if LV_USE_MSGBOX
        case LV_THEME_MSGBOX:
            lv_obj_clean_style_list(obj, LV_MSGBOX_PART_BG);
            list = lv_obj_get_style_list(obj, LV_MSGBOX_PART_BG);
            _lv_style_list_add_style(list, &bg);
            _lv_style_list_add_style(list, &mbox_bg);
            break;

        case LV_THEME_MSGBOX_BTNS:
            lv_obj_clean_style_list(obj, LV_MSGBOX_PART_BTN_BG);
            list = lv_obj_get_style_list(obj, LV_MSGBOX_PART_BTN_BG);
            _lv_style_list_add_style(list, &pad_small);

            lv_obj_clean_style_list(obj, LV_MSGBOX_PART_BTN);
            list = lv_obj_get_style_list(obj, LV_MSGBOX_PART_BTN);
            _lv_style_list_add_style(list, &btn);
            break;

#endif
#if LV_USE_LED
        case LV_THEME_LED:
            lv_obj_clean_style_list(obj, LV_LED_PART_MAIN);
            list = lv_obj_get_style_list(obj, LV_LED_PART_MAIN);
            _lv_style_list_add_style(list, &led);
            break;
#endif
#if LV_USE_PAGE
        case LV_THEME_PAGE:
            lv_obj_clean_style_list(obj, LV_PAGE_PART_BG);
            list = lv_obj_get_style_list(obj, LV_PAGE_PART_BG);
            _lv_style_list_add_style(list, &bg);

            lv_obj_clean_style_list(obj, LV_PAGE_PART_SCROLLABLE);
            list = lv_obj_get_style_list(obj, LV_PAGE_PART_SCROLLABLE);
            _lv_style_list_add_style(list, &pad_inner);

            lv_obj_clean_style_list(obj, LV_PAGE_PART_SCROLLBAR);
            list = lv_obj_get_style_list(obj, LV_PAGE_PART_SCROLLBAR);
            _lv_style_list_add_style(list, &sb);

#if LV_USE_ANIMATION
            lv_obj_clean_style_list(obj, LV_PAGE_PART_EDGE_FLASH);
            list = lv_obj_get_style_list(obj, LV_PAGE_PART_EDGE_FLASH);
            _lv_style_list_add_style(list, &edge_flash);
#endif
            break;
#endif
#if LV_USE_TABVIEW
        case LV_THEME_TABVIEW:
            lv_obj_clean_style_list(obj, LV_TABVIEW_PART_BG);
            list = lv_obj_get_style_list(obj, LV_TABVIEW_PART_BG);
            _lv_style_list_add_style(list, &scr);

            lv_obj_clean_style_list(obj, LV_TABVIEW_PART_BG_SCRLLABLE);

            lv_obj_clean_style_list(obj, LV_TABVIEW_PART_TAB_BG);
            list = lv_obj_get_style_list(obj, LV_TABVIEW_PART_TAB_BG);
            _lv_style_list_add_style(list, &tabview_btns_bg);

            lv_obj_clean_style_list(obj, LV_TABVIEW_PART_INDIC);
            list = lv_obj_get_style_list(obj, LV_TABVIEW_PART_INDIC);
            _lv_style_list_add_style(list, &tabview_indic);

            lv_obj_clean_style_list(obj, LV_TABVIEW_PART_TAB_BTN);
            list = lv_obj_get_style_list(obj, LV_TABVIEW_PART_TAB_BTN);
            _lv_style_list_add_style(list, &tabview_btns);
            break;

        case LV_THEME_TABVIEW_PAGE:
            lv_obj_clean_style_list(obj, LV_PAGE_PART_BG);

            lv_obj_clean_style_list(obj, LV_PAGE_PART_SCROLLABLE);
            list = lv_obj_get_style_list(obj, LV_PAGE_PART_SCROLLABLE);
            _lv_style_list_add_style(list, &tabview_page_scrl);

            break;
#endif

#if LV_USE_TILEVIEW
        case LV_THEME_TILEVIEW:
            lv_obj_clean_style_list(obj, LV_TILEVIEW_PART_BG);
            list = lv_obj_get_style_list(obj, LV_TILEVIEW_PART_BG);
            _lv_style_list_add_style(list, &scr);

            lv_obj_clean_style_list(obj, LV_TILEVIEW_PART_SCROLLBAR);
            list = lv_obj_get_style_list(obj, LV_TILEVIEW_PART_SCROLLBAR);
            _lv_style_list_add_style(list, &sb);

#if LV_USE_ANIMATION
            lv_obj_clean_style_list(obj, LV_TILEVIEW_PART_EDGE_FLASH);
            list = lv_obj_get_style_list(obj, LV_TILEVIEW_PART_EDGE_FLASH);
            _lv_style_list_add_style(list, &edge_flash);
#endif
            break;
#endif


#if LV_USE_ROLLER
        case LV_THEME_ROLLER:
            lv_obj_clean_style_list(obj, LV_ROLLER_PART_BG);
            list = lv_obj_get_style_list(obj, LV_ROLLER_PART_BG);
            _lv_style_list_add_style(list, &bg);
            _lv_style_list_add_style(list, &roller_bg);

            lv_obj_clean_style_list(obj, LV_ROLLER_PART_SELECTED);
            list = lv_obj_get_style_list(obj, LV_ROLLER_PART_SELECTED);
            _lv_style_list_add_style(list, &roller_sel);
            break;
#endif


#if LV_USE_OBJMASK
        case LV_THEME_OBJMASK:
            lv_obj_clean_style_list(obj, LV_OBJMASK_PART_MAIN);
            list = lv_obj_get_style_list(obj, LV_OBJMASK_PART_MAIN);
            break;
#endif

#if LV_USE_LIST
        case LV_THEME_LIST:
            lv_obj_clean_style_list(obj, LV_LIST_PART_BG);
            list = lv_obj_get_style_list(obj, LV_LIST_PART_BG);
            _lv_style_list_add_style(list, &bg);
            _lv_style_list_add_style(list, &list_bg);

            lv_obj_clean_style_list(obj, LV_LIST_PART_SCROLLABLE);

            lv_obj_clean_style_list(obj, LV_LIST_PART_SCROLLBAR);
            list = lv_obj_get_style_list(obj, LV_LIST_PART_SCROLLBAR);
            _lv_style_list_add_style(list, &sb);
            break;

        case LV_THEME_LIST_BTN:
            lv_obj_clean_style_list(obj, LV_BTN_PART_MAIN);
            list = lv_obj_get_style_list(obj, LV_BTN_PART_MAIN);
            _lv_style_list_add_style(list, &list_btn);
            break;
#endif

#if LV_USE_DROPDOWN
        case LV_THEME_DROPDOWN:
            lv_obj_clean_style_list(obj, LV_DROPDOWN_PART_MAIN);
            list = lv_obj_get_style_list(obj, LV_DROPDOWN_PART_MAIN);
            _lv_style_list_add_style(list, &bg);
            _lv_style_list_add_style(list, &bg_click);
            _lv_style_list_add_style(list, &pad_small);

            lv_obj_clean_style_list(obj, LV_DROPDOWN_PART_LIST);
            list = lv_obj_get_style_list(obj, LV_DROPDOWN_PART_LIST);
            _lv_style_list_add_style(list, &bg);
            _lv_style_list_add_style(list, &ddlist_page);

            lv_obj_clean_style_list(obj, LV_DROPDOWN_PART_SCROLLBAR);
            list = lv_obj_get_style_list(obj, LV_DROPDOWN_PART_SCROLLBAR);
            _lv_style_list_add_style(list, &sb);

            lv_obj_clean_style_list(obj, LV_DROPDOWN_PART_SELECTED);
            list = lv_obj_get_style_list(obj, LV_DROPDOWN_PART_SELECTED);
            _lv_style_list_add_style(list, &ddlist_sel);
            break;
#endif

#if LV_USE_CHART
        case LV_THEME_CHART:
            lv_obj_clean_style_list(obj, LV_CHART_PART_BG);
            list = lv_obj_get_style_list(obj, LV_CHART_PART_BG);
            _lv_style_list_add_style(list, &bg);
            _lv_style_list_add_style(list, &chart_bg);
            _lv_style_list_add_style(list, &pad_small);

            lv_obj_clean_style_list(obj, LV_CHART_PART_SERIES_BG);
            list = lv_obj_get_style_list(obj, LV_CHART_PART_SERIES_BG);
            _lv_style_list_add_style(list, &pad_small);
            _lv_style_list_add_style(list, &chart_series_bg);

            lv_obj_clean_style_list(obj, LV_CHART_PART_SERIES);
            list = lv_obj_get_style_list(obj, LV_CHART_PART_SERIES);
            _lv_style_list_add_style(list, &chart_series);
            break;
#endif
#if LV_USE_TABLE
        case LV_THEME_TABLE:
            lv_obj_clean_style_list(obj, LV_TABLE_PART_BG);
            list = lv_obj_get_style_list(obj, LV_TABLE_PART_BG);
            _lv_style_list_add_style(list, &bg);

            lv_obj_clean_style_list(obj, LV_TABLE_PART_CELL1);
            list = lv_obj_get_style_list(obj, LV_TABLE_PART_CELL1);
            _lv_style_list_add_style(list, &table_cell);

            lv_obj_clean_style_list(obj, LV_TABLE_PART_CELL2);
            list = lv_obj_get_style_list(obj, LV_TABLE_PART_CELL2);
            _lv_style_list_add_style(list, &table_cell);

            lv_obj_clean_style_list(obj, LV_TABLE_PART_CELL3);
            list = lv_obj_get_style_list(obj, LV_TABLE_PART_CELL3);
            _lv_style_list_add_style(list, &table_cell);

            lv_obj_clean_style_list(obj, LV_TABLE_PART_CELL4);
            list = lv_obj_get_style_list(obj, LV_TABLE_PART_CELL4);
            _lv_style_list_add_style(list, &table_cell);
            break;
#endif

#if LV_USE_WIN
        case LV_THEME_WIN:
            lv_obj_clean_style_list(obj, LV_WIN_PART_BG);
            list = lv_obj_get_style_list(obj, LV_WIN_PART_BG);
            _lv_style_list_add_style(list, &scr);

            lv_obj_clean_style_list(obj, LV_WIN_PART_SCROLLBAR);
            list = lv_obj_get_style_list(obj, LV_WIN_PART_SCROLLBAR);
            _lv_style_list_add_style(list, &sb);

            lv_obj_clean_style_list(obj, LV_WIN_PART_CONTENT_SCROLLABLE);
            list = lv_obj_get_style_list(obj, LV_WIN_PART_CONTENT_SCROLLABLE);
            _lv_style_list_add_style(list, &tabview_page_scrl);

            lv_obj_clean_style_list(obj, LV_WIN_PART_HEADER);
            list = lv_obj_get_style_list(obj, LV_WIN_PART_HEADER);
            _lv_style_list_add_style(list, &tabview_btns_bg);
            break;

        case LV_THEME_WIN_BTN:
            lv_obj_clean_style_list(obj, LV_BTN_PART_MAIN);
            list = lv_obj_get_style_list(obj, LV_BTN_PART_MAIN);
            _lv_style_list_add_style(list, &tabview_btns);
            break;
#endif

#if LV_USE_TEXTAREA
        case LV_THEME_TEXTAREA:
            lv_obj_clean_style_list(obj, LV_TEXTAREA_PART_BG);
            list = lv_obj_get_style_list(obj, LV_TEXTAREA_PART_BG);
            _lv_style_list_add_style(list, &bg);
            _lv_style_list_add_style(list, &pad_small);

            lv_obj_clean_style_list(obj, LV_TEXTAREA_PART_PLACEHOLDER);
            list = lv_obj_get_style_list(obj, LV_TEXTAREA_PART_PLACEHOLDER);
            _lv_style_list_add_style(list, &ta_placeholder);

            lv_obj_clean_style_list(obj, LV_TEXTAREA_PART_CURSOR);
            list = lv_obj_get_style_list(obj, LV_TEXTAREA_PART_CURSOR);
            _lv_style_list_add_style(list, &ta_cursor);

            lv_obj_clean_style_list(obj, LV_TEXTAREA_PART_SCROLLBAR);
            list = lv_obj_get_style_list(obj, LV_TEXTAREA_PART_SCROLLBAR);
            _lv_style_list_add_style(list, &sb);
            break;

#endif


#if LV_USE_SPINBOX
        case LV_THEME_SPINBOX:
            lv_obj_clean_style_list(obj, LV_SPINBOX_PART_BG);
            list = lv_obj_get_style_list(obj, LV_SPINBOX_PART_BG);
            _lv_style_list_add_style(list, &bg);
            _lv_style_list_add_style(list, &pad_small);

            lv_obj_clean_style_list(obj, LV_SPINBOX_PART_CURSOR);
            list = lv_obj_get_style_list(obj, LV_SPINBOX_PART_CURSOR);
            _lv_style_list_add_style(list, &spinbox_cursor);
            break;

        case LV_THEME_SPINBOX_BTN:
            lv_obj_clean_style_list(obj, LV_BTN_PART_MAIN);
            list = lv_obj_get_style_list(obj, LV_BTN_PART_MAIN);
            _lv_style_list_add_style(list, &bg);
            _lv_style_list_add_style(list, &bg_click);
            break;
#endif

#if LV_USE_CALENDAR
        case LV_THEME_CALENDAR:
            lv_obj_clean_style_list(obj, LV_CALENDAR_PART_BG);
            list = lv_obj_get_style_list(obj, LV_CALENDAR_PART_BG);
            _lv_style_list_add_style(list, &bg);

            lv_obj_clean_style_list(obj, LV_CALENDAR_PART_DATE);
            list = lv_obj_get_style_list(obj, LV_CALENDAR_PART_DATE);
            _lv_style_list_add_style(list, &calendar_date_nums);

            lv_obj_clean_style_list(obj, LV_CALENDAR_PART_HEADER);
            list = lv_obj_get_style_list(obj, LV_CALENDAR_PART_HEADER);
            _lv_style_list_add_style(list, &calendar_header);

            lv_obj_clean_style_list(obj, LV_CALENDAR_PART_DAY_NAMES);
            list = lv_obj_get_style_list(obj, LV_CALENDAR_PART_DAY_NAMES);
            _lv_style_list_add_style(list, &calendar_daynames);
            break;
#endif
#if LV_USE_CPICKER
        case LV_THEME_CPICKER:
            lv_obj_clean_style_list(obj, LV_CPICKER_PART_MAIN);
            list = lv_obj_get_style_list(obj, LV_CPICKER_PART_MAIN);
            _lv_style_list_add_style(list, &cpicker_bg);

            lv_obj_clean_style_list(obj, LV_CPICKER_PART_KNOB);
            list = lv_obj_get_style_list(obj, LV_CPICKER_PART_KNOB);
            _lv_style_list_add_style(list, &cpicker_indic);
            break;
#endif

#if LV_USE_LINEMETER
        case LV_THEME_LINEMETER:
            lv_obj_clean_style_list(obj, LV_LINEMETER_PART_MAIN);
            list = lv_obj_get_style_list(obj, LV_LINEMETER_PART_MAIN);
            _lv_style_list_add_style(list, &bg);
            _lv_style_list_add_style(list, &lmeter);
            break;
#endif
#if LV_USE_GAUGE
        case LV_THEME_GAUGE:
            lv_obj_clean_style_list(obj, LV_GAUGE_PART_MAIN);
            list = lv_obj_get_style_list(obj, LV_GAUGE_PART_MAIN);
            _lv_style_list_add_style(list, &bg);
            _lv_style_list_add_style(list, &gauge_main);

            lv_obj_clean_style_list(obj, LV_GAUGE_PART_MAJOR);
            list = lv_obj_get_style_list(obj, LV_GAUGE_PART_MAJOR);
            _lv_style_list_add_style(list, &gauge_strong);

            lv_obj_clean_style_list(obj, LV_GAUGE_PART_NEEDLE);
            list = lv_obj_get_style_list(obj, LV_GAUGE_PART_NEEDLE);
            _lv_style_list_add_style(list, &gauge_needle);
            break;
#endif
        default:
            break;
    }

    lv_obj_refresh_style(obj, LV_STYLE_PROP_ALL);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void style_init_reset(lv_style_t * style)
{
    if(inited) lv_style_reset(style);
    else lv_style_init(style);
}

#endif
