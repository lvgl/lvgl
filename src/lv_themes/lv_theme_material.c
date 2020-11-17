/**
 * @file lv_theme_material.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lvgl.h" /*To see all the widgets*/

#if LV_USE_THEME_MATERIAL

#include "../lv_misc/lv_gc.h"

#if defined(LV_GC_INCLUDE)
    #include LV_GC_INCLUDE
#endif /* LV_ENABLE_GC */

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
#define COLOR_BG_BORDER_CHK     (IS_LIGHT ? lv_color_hex(0xd6dde3) : lv_color_hex(0x5f656e))
#define COLOR_BG_BORDER_CHK_PR  (IS_LIGHT ? lv_color_hex3(0xccc) : lv_color_hex(0x5f656e))
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

#define TRANSITION_TIME         ((theme.flags & LV_THEME_MATERIAL_FLAG_NO_TRANSITION) ? 0 : 150)
#define BORDER_WIDTH            LV_DPX(2)
#define OUTLINE_WIDTH           ((theme.flags & LV_THEME_MATERIAL_FLAG_NO_FOCUS) ? 0 : LV_DPX(2))
#define IS_LIGHT (theme.flags & LV_THEME_MATERIAL_FLAG_LIGHT)

#define PAD_DEF (lv_disp_get_size_category(NULL) <= LV_DISP_SIZE_MEDIUM ? LV_DPX(15) : (LV_DPX(30)))

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_style_t scr;
    lv_style_t sb;
    lv_style_t bg;
    lv_style_t bg_click;
    lv_style_t bg_sec;
    lv_style_t btn;
    lv_style_t pad_small;

#if LV_USE_ARC
    lv_style_t arc_indic;
    lv_style_t arc_bg;
    lv_style_t arc_knob;
#endif

#if LV_USE_BAR
    lv_style_t bar_bg;
    lv_style_t bar_indic;
#endif

#if LV_USE_BTNMATRIX
    lv_style_t btnmatrix_btn;
#endif

#if LV_USE_CHART
    lv_style_t chart_bg, chart_series_bg, chart_series;
#endif

#if LV_USE_CHECKBOX
    lv_style_t cb_bg, cb_bullet;
#endif

#if LV_USE_DROPDOWN
    lv_style_t ddlist_page, ddlist_sel;
#endif

#if LV_USE_GAUGE
    lv_style_t gauge_main, gauge_strong, gauge_needle;
#endif

#if LV_USE_LINEMETER
    lv_style_t lmeter;
#endif

#if LV_USE_ROLLER
    lv_style_t roller_bg, roller_sel;
#endif

#if LV_USE_SLIDER
    lv_style_t slider_knob, slider_bg;
#endif

#if LV_USE_SWITCH
    lv_style_t sw_knob;
#endif

#if LV_USE_TABLE
    lv_style_t table_cell;
#endif

#if LV_USE_TEXTAREA
    lv_style_t ta_cursor, ta_placeholder;
#endif

} theme_styles_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void theme_apply(lv_theme_t * th, lv_obj_t * obj, lv_theme_style_t name);
static void style_init_reset(lv_style_t * style);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_theme_t theme;
static theme_styles_t * styles;

static bool inited;

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void basic_init(void)
{
    style_init_reset(&styles->sb);
    lv_style_set_scrollbar_bg_opa(&styles->sb, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_scrollbar_bg_color(&styles->sb, LV_STATE_DEFAULT, (IS_LIGHT ? lv_color_hex(0xcccfd1) : lv_color_hex(0x777f85)));
    lv_style_set_scrollbar_radius(&styles->sb, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
    lv_style_set_scrollbar_tickness(&styles->sb, LV_STATE_DEFAULT, LV_DPX(7));
    lv_style_set_scrollbar_space_side(&styles->sb, LV_STATE_DEFAULT,  LV_DPX(7));
    lv_style_set_scrollbar_space_end(&styles->sb, LV_STATE_DEFAULT,  LV_DPX(7));

    style_init_reset(&styles->scr);
    lv_style_set_bg_opa(&styles->scr, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&styles->scr, LV_STATE_DEFAULT, COLOR_SCR);
    lv_style_set_text_color(&styles->scr, LV_STATE_DEFAULT, COLOR_SCR_TEXT);
    lv_style_set_value_color(&styles->scr, LV_STATE_DEFAULT, COLOR_SCR_TEXT);
    lv_style_set_text_sel_color(&styles->scr, LV_STATE_DEFAULT, COLOR_SCR_TEXT);
    lv_style_set_text_sel_bg_color(&styles->scr, LV_STATE_DEFAULT, theme.color_primary);
    lv_style_set_value_font(&styles->scr, LV_STATE_DEFAULT, theme.font_normal);

    style_init_reset(&styles->bg);
    lv_style_set_radius(&styles->bg, LV_STATE_DEFAULT, LV_DPX(8));
    lv_style_set_bg_opa(&styles->bg, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&styles->bg, LV_STATE_DEFAULT, COLOR_BG);
    lv_style_set_border_color(&styles->bg, LV_STATE_DEFAULT, COLOR_BG_BORDER);
    if((theme.flags & LV_THEME_MATERIAL_FLAG_NO_FOCUS) == 0)lv_style_set_border_color(&styles->bg, LV_STATE_FOCUSED,
                                                                                          theme.color_primary);
    lv_style_set_border_color(&styles->bg, LV_STATE_EDITED, theme.color_secondary);
    lv_style_set_border_width(&styles->bg, LV_STATE_DEFAULT, BORDER_WIDTH);
    lv_style_set_border_post(&styles->bg, LV_STATE_DEFAULT, true);
    lv_style_set_text_color(&styles->bg, LV_STATE_DEFAULT, COLOR_BG_TEXT);
    lv_style_set_value_font(&styles->bg, LV_STATE_DEFAULT, theme.font_normal);
    lv_style_set_value_color(&styles->bg, LV_STATE_DEFAULT, COLOR_BG_TEXT);
    lv_style_set_image_recolor(&styles->bg, LV_STATE_DEFAULT, COLOR_BG_TEXT);
    lv_style_set_line_color(&styles->bg, LV_STATE_DEFAULT, COLOR_BG_TEXT);
    lv_style_set_line_width(&styles->bg, LV_STATE_DEFAULT, 1);

    lv_style_set_pad_left(&styles->bg, LV_STATE_DEFAULT, PAD_DEF + BORDER_WIDTH);
    lv_style_set_pad_right(&styles->bg, LV_STATE_DEFAULT, PAD_DEF + BORDER_WIDTH);
    lv_style_set_pad_top(&styles->bg, LV_STATE_DEFAULT, PAD_DEF + BORDER_WIDTH);
    lv_style_set_pad_bottom(&styles->bg, LV_STATE_DEFAULT, PAD_DEF + BORDER_WIDTH);
    lv_style_set_transition_time(&styles->bg, LV_STATE_DEFAULT, TRANSITION_TIME);
    lv_style_set_transition_prop_6(&styles->bg, LV_STATE_DEFAULT, LV_STYLE_BORDER_COLOR);

    style_init_reset(&styles->bg_sec);
    lv_style_copy(&styles->bg_sec, &styles->bg);
    lv_style_set_bg_color(&styles->bg_sec, LV_STATE_DEFAULT, COLOR_BG_SEC);
    lv_style_set_border_color(&styles->bg_sec, LV_STATE_DEFAULT, COLOR_BG_SEC_BORDER);
    lv_style_set_text_color(&styles->bg_sec, LV_STATE_DEFAULT, COLOR_BG_SEC_TEXT);
    lv_style_set_value_color(&styles->bg_sec, LV_STATE_DEFAULT, COLOR_BG_SEC_TEXT);
    lv_style_set_image_recolor(&styles->bg_sec, LV_STATE_DEFAULT, COLOR_BG_SEC_TEXT);
    lv_style_set_line_color(&styles->bg_sec, LV_STATE_DEFAULT, COLOR_BG_SEC_TEXT);

    style_init_reset(&styles->bg_click);
    lv_style_set_bg_color(&styles->bg_click, LV_STATE_PRESSED, COLOR_BG_PR);
    lv_style_set_bg_color(&styles->bg_click, LV_STATE_CHECKED, COLOR_BG_CHK);
    lv_style_set_bg_color(&styles->bg_click, LV_STATE_PRESSED | LV_STATE_CHECKED, COLOR_BG_PR_CHK);
    lv_style_set_bg_color(&styles->bg_click, LV_STATE_DISABLED, COLOR_BG_DIS);
    lv_style_set_border_width(&styles->bg_click, LV_STATE_CHECKED, 0);
    lv_style_set_border_color(&styles->bg_click, LV_STATE_FOCUSED | LV_STATE_PRESSED, lv_color_darken(theme.color_primary,
                                                                                                      LV_OPA_20));
    lv_style_set_border_color(&styles->bg_click, LV_STATE_PRESSED, COLOR_BG_BORDER_PR);
    lv_style_set_border_color(&styles->bg_click, LV_STATE_CHECKED, COLOR_BG_BORDER_CHK);
    lv_style_set_border_color(&styles->bg_click, LV_STATE_PRESSED | LV_STATE_CHECKED, COLOR_BG_BORDER_CHK_PR);
    lv_style_set_border_color(&styles->bg_click, LV_STATE_DISABLED, COLOR_BG_BORDER_DIS);
    lv_style_set_text_color(&styles->bg_click, LV_STATE_PRESSED, COLOR_BG_TEXT_PR);
    lv_style_set_text_color(&styles->bg_click, LV_STATE_CHECKED, COLOR_BG_TEXT_CHK);
    lv_style_set_text_color(&styles->bg_click, LV_STATE_PRESSED | LV_STATE_CHECKED, COLOR_BG_TEXT_CHK_PR);
    lv_style_set_text_color(&styles->bg_click, LV_STATE_DISABLED, COLOR_BG_TEXT_DIS);
    lv_style_set_image_recolor(&styles->bg_click, LV_STATE_PRESSED, COLOR_BG_TEXT_PR);
    lv_style_set_image_recolor(&styles->bg_click, LV_STATE_CHECKED, COLOR_BG_TEXT_CHK);
    lv_style_set_image_recolor(&styles->bg_click, LV_STATE_PRESSED | LV_STATE_CHECKED, COLOR_BG_TEXT_CHK_PR);
    lv_style_set_image_recolor(&styles->bg_click, LV_STATE_DISABLED, COLOR_BG_TEXT_DIS);
    lv_style_set_transition_prop_5(&styles->bg_click, LV_STATE_DEFAULT, LV_STYLE_BG_COLOR);

    style_init_reset(&styles->btn);
    lv_style_set_radius(&styles->btn, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
    lv_style_set_bg_opa(&styles->btn, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&styles->btn, LV_STATE_DEFAULT, COLOR_BTN);
    lv_style_set_bg_color(&styles->btn, LV_STATE_PRESSED, COLOR_BTN_PR);
    lv_style_set_bg_color(&styles->btn, LV_STATE_CHECKED, COLOR_BTN_CHK);
    lv_style_set_bg_color(&styles->btn, LV_STATE_CHECKED | LV_STATE_PRESSED, COLOR_BTN_CHK_PR);
    lv_style_set_bg_color(&styles->btn, LV_STATE_DISABLED, COLOR_BTN);
    lv_style_set_bg_color(&styles->btn, LV_STATE_DISABLED | LV_STATE_CHECKED, COLOR_BTN_DIS);
    lv_style_set_border_color(&styles->btn, LV_STATE_DEFAULT, COLOR_BTN_BORDER);
    lv_style_set_border_color(&styles->btn, LV_STATE_PRESSED, COLOR_BTN_BORDER_PR);
    lv_style_set_border_color(&styles->btn, LV_STATE_DISABLED, COLOR_BTN_BORDER_INA);
    lv_style_set_border_width(&styles->btn, LV_STATE_DEFAULT, BORDER_WIDTH);
    lv_style_set_border_opa(&styles->btn, LV_STATE_CHECKED, LV_OPA_TRANSP);

    lv_style_set_text_color(&styles->btn, LV_STATE_DEFAULT, IS_LIGHT ? lv_color_hex(0x31404f) : lv_color_hex(0xffffff));
    lv_style_set_text_color(&styles->btn, LV_STATE_PRESSED, IS_LIGHT ? lv_color_hex(0x31404f) : lv_color_hex(0xffffff));
    lv_style_set_text_color(&styles->btn, LV_STATE_CHECKED,  lv_color_hex(0xffffff));
    lv_style_set_text_color(&styles->btn, LV_STATE_CHECKED | LV_STATE_PRESSED, lv_color_hex(0xffffff));
    lv_style_set_text_color(&styles->btn, LV_STATE_DISABLED, IS_LIGHT ? lv_color_hex(0x888888) : lv_color_hex(0x888888));

    lv_style_set_image_recolor(&styles->btn, LV_STATE_DEFAULT, IS_LIGHT ? lv_color_hex(0x31404f) : lv_color_hex(0xffffff));
    lv_style_set_image_recolor(&styles->btn, LV_STATE_PRESSED, IS_LIGHT ? lv_color_hex(0x31404f) : lv_color_hex(0xffffff));
    lv_style_set_image_recolor(&styles->btn, LV_STATE_PRESSED, lv_color_hex(0xffffff));
    lv_style_set_image_recolor(&styles->btn, LV_STATE_CHECKED | LV_STATE_PRESSED, lv_color_hex(0xffffff));
    lv_style_set_image_recolor(&styles->btn, LV_STATE_DISABLED, IS_LIGHT ? lv_color_hex(0x888888) : lv_color_hex(0x888888));

    lv_style_set_value_color(&styles->btn, LV_STATE_DEFAULT, IS_LIGHT ? lv_color_hex(0x31404f) : lv_color_hex(0xffffff));
    lv_style_set_value_color(&styles->btn, LV_STATE_PRESSED, IS_LIGHT ? lv_color_hex(0x31404f) : lv_color_hex(0xffffff));
    lv_style_set_value_color(&styles->btn, LV_STATE_CHECKED,  lv_color_hex(0xffffff));
    lv_style_set_value_color(&styles->btn, LV_STATE_CHECKED | LV_STATE_PRESSED, lv_color_hex(0xffffff));
    lv_style_set_value_color(&styles->btn, LV_STATE_DISABLED, IS_LIGHT ? lv_color_hex(0x888888) : lv_color_hex(0x888888));

    lv_style_set_pad_left(&styles->btn, LV_STATE_DEFAULT, LV_DPX(40));
    lv_style_set_pad_right(&styles->btn, LV_STATE_DEFAULT, LV_DPX(40));
    lv_style_set_pad_top(&styles->btn, LV_STATE_DEFAULT, LV_DPX(15));
    lv_style_set_pad_bottom(&styles->btn, LV_STATE_DEFAULT, LV_DPX(15));
    lv_style_set_outline_width(&styles->btn, LV_STATE_DEFAULT, OUTLINE_WIDTH);
    lv_style_set_outline_opa(&styles->btn, LV_STATE_DEFAULT, LV_OPA_0);
    lv_style_set_outline_opa(&styles->btn, LV_STATE_FOCUSED, LV_OPA_50);
    lv_style_set_outline_color(&styles->btn, LV_STATE_DEFAULT, theme.color_primary);
    lv_style_set_outline_color(&styles->btn, LV_STATE_EDITED, theme.color_secondary);
    lv_style_set_transition_time(&styles->btn, LV_STATE_DEFAULT, TRANSITION_TIME);
    lv_style_set_transition_prop_4(&styles->btn, LV_STATE_DEFAULT, LV_STYLE_BORDER_OPA);
    lv_style_set_transition_prop_5(&styles->btn, LV_STATE_DEFAULT, LV_STYLE_BG_COLOR);
    lv_style_set_transition_prop_6(&styles->btn, LV_STATE_DEFAULT, LV_STYLE_OUTLINE_OPA);
    lv_style_set_transition_delay(&styles->btn, LV_STATE_DEFAULT, TRANSITION_TIME);
    lv_style_set_transition_delay(&styles->btn, LV_STATE_PRESSED, 0);

    style_init_reset(&styles->pad_small);
    lv_style_int_t pad_small_value = lv_disp_get_size_category(NULL) <= LV_DISP_MEDIUM_LIMIT ? LV_DPX(10) : LV_DPX(20);
    lv_style_set_pad_left(&styles->pad_small, LV_STATE_DEFAULT,  pad_small_value);
    lv_style_set_pad_right(&styles->pad_small, LV_STATE_DEFAULT, pad_small_value);
    lv_style_set_pad_top(&styles->pad_small, LV_STATE_DEFAULT,  pad_small_value);
    lv_style_set_pad_bottom(&styles->pad_small, LV_STATE_DEFAULT, pad_small_value);
}


static void btn_init(void)
{
#if LV_USE_BTN != 0

#endif
}

static void btnmatrix_init(void)
{
#if LV_USE_BTNMATRIX != 0
    style_init_reset(&styles->btnmatrix_btn);
    lv_style_set_margin_all(&styles->btnmatrix_btn, LV_STATE_DEFAULT, LV_DPX(10));
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
    style_init_reset(&styles->bar_bg);
    lv_style_set_radius(&styles->bar_bg, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
    lv_style_set_bg_opa(&styles->bar_bg, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&styles->bar_bg, LV_STATE_DEFAULT, COLOR_BG_SEC);
    lv_style_set_value_color(&styles->bar_bg, LV_STATE_DEFAULT, IS_LIGHT ? lv_color_hex(0x31404f) : LV_COLOR_WHITE);
    lv_style_set_outline_color(&styles->bar_bg, LV_STATE_DEFAULT, theme.color_primary);
    lv_style_set_outline_color(&styles->bar_bg, LV_STATE_EDITED, theme.color_secondary);
    lv_style_set_outline_opa(&styles->bar_bg, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_style_set_outline_opa(&styles->bar_bg, LV_STATE_FOCUSED, LV_OPA_50);
    lv_style_set_outline_width(&styles->bar_bg, LV_STATE_DEFAULT, OUTLINE_WIDTH);
    lv_style_set_transition_time(&styles->bar_bg, LV_STATE_DEFAULT, TRANSITION_TIME);
    lv_style_set_transition_prop_6(&styles->bar_bg, LV_STATE_DEFAULT, LV_STYLE_OUTLINE_OPA);

    style_init_reset(&styles->bar_indic);
    lv_style_set_bg_opa(&styles->bar_indic, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_radius(&styles->bar_indic, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
    lv_style_set_bg_color(&styles->bar_indic, LV_STATE_DEFAULT, theme.color_primary);
    lv_style_set_bg_color(&styles->bar_indic, LV_STATE_DISABLED, lv_color_hex3(0x888));
    lv_style_set_value_color(&styles->bar_indic, LV_STATE_DEFAULT, IS_LIGHT ? lv_color_hex(0x41404f) : LV_COLOR_WHITE);
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

static void slider_init(void)
{
#if LV_USE_SLIDER != 0
    style_init_reset(&styles->slider_knob);
    lv_style_set_bg_opa(&styles->slider_knob, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&styles->slider_knob, LV_STATE_DEFAULT, IS_LIGHT ? theme.color_primary : LV_COLOR_WHITE);
    lv_style_set_value_color(&styles->slider_knob, LV_STATE_DEFAULT, IS_LIGHT ? lv_color_hex(0x31404f) : LV_COLOR_WHITE);
    lv_style_set_radius(&styles->slider_knob, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
    lv_style_set_pad_left(&styles->slider_knob, LV_STATE_DEFAULT, LV_DPX(7));
    lv_style_set_pad_right(&styles->slider_knob, LV_STATE_DEFAULT, LV_DPX(7));
    lv_style_set_pad_top(&styles->slider_knob, LV_STATE_DEFAULT, LV_DPX(7));
    lv_style_set_pad_bottom(&styles->slider_knob, LV_STATE_DEFAULT, LV_DPX(7));

    style_init_reset(&styles->slider_bg);
    lv_style_set_margin_left(&styles->slider_bg, LV_STATE_DEFAULT, LV_DPX(10));
    lv_style_set_margin_right(&styles->slider_bg, LV_STATE_DEFAULT, LV_DPX(10));
    lv_style_set_margin_top(&styles->slider_bg, LV_STATE_DEFAULT, LV_DPX(10));
    lv_style_set_margin_bottom(&styles->slider_bg, LV_STATE_DEFAULT, LV_DPX(10));

#endif
}

static void switch_init(void)
{
#if LV_USE_SWITCH != 0
    style_init_reset(&styles->sw_knob);
    lv_style_set_bg_opa(&styles->sw_knob, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&styles->sw_knob, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_radius(&styles->sw_knob, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
    lv_style_set_pad_top(&styles->sw_knob, LV_STATE_DEFAULT,    - LV_DPX(4));
    lv_style_set_pad_bottom(&styles->sw_knob, LV_STATE_DEFAULT, - LV_DPX(4));
    lv_style_set_pad_left(&styles->sw_knob, LV_STATE_DEFAULT,   - LV_DPX(4));
    lv_style_set_pad_right(&styles->sw_knob, LV_STATE_DEFAULT,  - LV_DPX(4));
#endif
}

static void linemeter_init(void)
{
#if LV_USE_LINEMETER != 0
    style_init_reset(&styles->lmeter);
    lv_style_set_radius(&styles->lmeter, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
    lv_style_set_pad_left(&styles->lmeter, LV_STATE_DEFAULT, LV_DPX(20));
    lv_style_set_pad_right(&styles->lmeter, LV_STATE_DEFAULT, LV_DPX(20));
    lv_style_set_pad_top(&styles->lmeter, LV_STATE_DEFAULT, LV_DPX(20));
    lv_style_set_scale_width(&styles->lmeter, LV_STATE_DEFAULT, LV_DPX(25));

    lv_style_set_line_color(&styles->lmeter, LV_STATE_DEFAULT, theme.color_primary);
    lv_style_set_scale_grad_color(&styles->lmeter, LV_STATE_DEFAULT, theme.color_primary);
    lv_style_set_scale_end_color(&styles->lmeter, LV_STATE_DEFAULT, lv_color_hex3(0x888));
    lv_style_set_line_width(&styles->lmeter, LV_STATE_DEFAULT, LV_DPX(10));
    lv_style_set_scale_end_line_width(&styles->lmeter, LV_STATE_DEFAULT, LV_DPX(7));
#endif
}

static void gauge_init(void)
{
#if LV_USE_GAUGE != 0
    style_init_reset(&styles->gauge_main);
    lv_style_set_line_color(&styles->gauge_main, LV_STATE_DEFAULT, lv_color_hex3(0x888));
    lv_style_set_scale_grad_color(&styles->gauge_main, LV_STATE_DEFAULT, lv_color_hex3(0x888));
    lv_style_set_scale_end_color(&styles->gauge_main, LV_STATE_DEFAULT, theme.color_primary);
    lv_style_set_line_width(&styles->gauge_main, LV_STATE_DEFAULT, LV_DPX(5));
    lv_style_set_scale_end_line_width(&styles->gauge_main, LV_STATE_DEFAULT, LV_DPX(4));
    lv_style_set_scale_end_border_width(&styles->gauge_main, LV_STATE_DEFAULT, LV_DPX(8));
    lv_style_set_pad_left(&styles->gauge_main, LV_STATE_DEFAULT, LV_DPX(20));
    lv_style_set_pad_right(&styles->gauge_main, LV_STATE_DEFAULT, LV_DPX(20));
    lv_style_set_pad_top(&styles->gauge_main, LV_STATE_DEFAULT, LV_DPX(20));
    lv_style_set_pad_all(&styles->gauge_main, LV_STATE_DEFAULT, LV_DPX(20));
    lv_style_set_scale_width(&styles->gauge_main, LV_STATE_DEFAULT, LV_DPX(15));
    lv_style_set_radius(&styles->gauge_main, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);

    style_init_reset(&styles->gauge_strong);
    lv_style_set_line_color(&styles->gauge_strong, LV_STATE_DEFAULT, lv_color_hex3(0x888));
    lv_style_set_scale_grad_color(&styles->gauge_strong, LV_STATE_DEFAULT, lv_color_hex3(0x888));
    lv_style_set_scale_end_color(&styles->gauge_strong, LV_STATE_DEFAULT, theme.color_primary);
    lv_style_set_line_width(&styles->gauge_strong, LV_STATE_DEFAULT, LV_DPX(8));
    lv_style_set_scale_end_line_width(&styles->gauge_strong, LV_STATE_DEFAULT, LV_DPX(8));
    lv_style_set_scale_width(&styles->gauge_strong, LV_STATE_DEFAULT, LV_DPX(25));

    style_init_reset(&styles->gauge_needle);
    lv_style_set_line_color(&styles->gauge_needle, LV_STATE_DEFAULT, IS_LIGHT ? lv_color_hex(0x464b5b) : LV_COLOR_WHITE);
    lv_style_set_line_width(&styles->gauge_needle, LV_STATE_DEFAULT, LV_DPX(8));
    lv_style_set_bg_opa(&styles->gauge_needle, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&styles->gauge_needle, LV_STATE_DEFAULT, IS_LIGHT ? lv_color_hex(0x464b5b) : LV_COLOR_WHITE);
    lv_style_set_radius(&styles->gauge_needle, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
    lv_style_set_size(&styles->gauge_needle, LV_STATE_DEFAULT, LV_DPX(30));
    lv_style_set_pad_all(&styles->gauge_needle, LV_STATE_DEFAULT, LV_DPX(10));
#endif
}

static void arc_init(void)
{
#if LV_USE_ARC != 0
    style_init_reset(&styles->arc_indic);
    lv_style_set_line_color(&styles->arc_indic, LV_STATE_DEFAULT, theme.color_primary);
    lv_style_set_line_width(&styles->arc_indic, LV_STATE_DEFAULT, LV_DPX(25));
    lv_style_set_line_rounded(&styles->arc_indic, LV_STATE_DEFAULT, true);

    style_init_reset(&styles->arc_bg);
    lv_style_set_line_color(&styles->arc_bg, LV_STATE_DEFAULT, COLOR_BG_SEC);
    lv_style_set_line_width(&styles->arc_bg, LV_STATE_DEFAULT, LV_DPX(25));
    lv_style_set_line_rounded(&styles->arc_bg, LV_STATE_DEFAULT, true);

    style_init_reset(&styles->arc_knob);
    lv_style_set_radius(&styles->arc_knob, LV_STATE_DEFAULT,   LV_RADIUS_CIRCLE);
    lv_style_set_pad_top(&styles->arc_knob, LV_STATE_DEFAULT,  LV_DPX(0));
    lv_style_set_pad_bottom(&styles->arc_knob, LV_STATE_DEFAULT,  LV_DPX(0));
    lv_style_set_pad_left(&styles->arc_knob, LV_STATE_DEFAULT,    LV_DPX(0));
    lv_style_set_pad_right(&styles->arc_knob, LV_STATE_DEFAULT,   LV_DPX(0));

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
    style_init_reset(&styles->chart_bg);
    lv_style_set_text_color(&styles->chart_bg, LV_STATE_DEFAULT, IS_LIGHT ? COLOR_BG_TEXT_DIS : lv_color_hex(0xa1adbd));

    style_init_reset(&styles->chart_series_bg);
    lv_style_set_line_width(&styles->chart_series_bg, LV_STATE_DEFAULT, LV_DPX(1));
    lv_style_set_line_dash_width(&styles->chart_series_bg, LV_STATE_DEFAULT, LV_DPX(10));
    lv_style_set_line_dash_gap(&styles->chart_series_bg, LV_STATE_DEFAULT, LV_DPX(10));
    lv_style_set_line_color(&styles->chart_series_bg, LV_STATE_DEFAULT, COLOR_BG_BORDER);

    style_init_reset(&styles->chart_series);
    lv_style_set_line_width(&styles->chart_series, LV_STATE_DEFAULT, LV_DPX(3));
    lv_style_set_size(&styles->chart_series, LV_STATE_DEFAULT, LV_DPX(4));
    lv_style_set_pad_all(&styles->chart_series, LV_STATE_DEFAULT, LV_DPX(2));     /*Space between columns*/
    lv_style_set_radius(&styles->chart_series, LV_STATE_DEFAULT, LV_DPX(1));

#endif
}

static void checkbox_init(void)
{
#if LV_USE_CHECKBOX != 0
    style_init_reset(&styles->cb_bg);
    lv_style_set_radius(&styles->cb_bg, LV_STATE_DEFAULT, LV_DPX(4));
    lv_style_set_outline_color(&styles->cb_bg, LV_STATE_DEFAULT, theme.color_primary);
    lv_style_set_outline_opa(&styles->cb_bg, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_style_set_outline_opa(&styles->cb_bg, LV_STATE_FOCUSED, LV_OPA_50);
    lv_style_set_outline_width(&styles->cb_bg, LV_STATE_DEFAULT, OUTLINE_WIDTH);
    lv_style_set_outline_pad(&styles->cb_bg, LV_STATE_DEFAULT, LV_DPX(6));
    lv_style_set_transition_time(&styles->cb_bg, LV_STATE_DEFAULT, TRANSITION_TIME);
    lv_style_set_transition_prop_6(&styles->cb_bg, LV_STATE_DEFAULT, LV_STYLE_OUTLINE_OPA);

    style_init_reset(&styles->cb_bullet);
    lv_style_set_outline_opa(&styles->cb_bullet, LV_STATE_FOCUSED, LV_OPA_TRANSP);
    lv_style_set_radius(&styles->cb_bullet, LV_STATE_DEFAULT, LV_DPX(4));
    lv_style_set_pattern_image(&styles->cb_bullet, LV_STATE_CHECKED, LV_SYMBOL_OK);
    lv_style_set_pattern_recolor(&styles->cb_bullet, LV_STATE_CHECKED, LV_COLOR_WHITE);
    lv_style_set_pattern_opa(&styles->cb_bullet, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_style_set_pattern_opa(&styles->cb_bullet, LV_STATE_CHECKED, LV_OPA_COVER);
    lv_style_set_transition_prop_3(&styles->cb_bullet, LV_STATE_DEFAULT, LV_STYLE_PATTERN_OPA);
    lv_style_set_text_font(&styles->cb_bullet, LV_STATE_CHECKED, theme.font_small);
    lv_style_set_pad_left(&styles->cb_bullet, LV_STATE_DEFAULT, LV_DPX(3));
    lv_style_set_pad_right(&styles->cb_bullet, LV_STATE_DEFAULT, LV_DPX(3));
    lv_style_set_pad_top(&styles->cb_bullet, LV_STATE_DEFAULT, LV_DPX(3));
    lv_style_set_pad_bottom(&styles->cb_bullet, LV_STATE_DEFAULT, LV_DPX(3));
    lv_style_set_margin_right(&styles->cb_bullet, LV_STATE_DEFAULT, LV_DPX(6));
#endif
}

static void textarea_init(void)
{
#if LV_USE_TEXTAREA
    style_init_reset(&styles->ta_cursor);
    lv_style_set_border_color(&styles->ta_cursor, LV_STATE_DEFAULT, COLOR_BG_SEC_TEXT);
    lv_style_set_border_width(&styles->ta_cursor, LV_STATE_DEFAULT, LV_DPX(2));
    lv_style_set_pad_left(&styles->ta_cursor, LV_STATE_DEFAULT, LV_DPX(1));
    lv_style_set_border_side(&styles->ta_cursor, LV_STATE_DEFAULT, LV_BORDER_SIDE_LEFT);

    style_init_reset(&styles->ta_placeholder);
    lv_style_set_text_color(&styles->ta_placeholder, LV_STATE_DEFAULT,
                            IS_LIGHT ? COLOR_BG_TEXT_DIS : lv_color_hex(0xa1adbd));
#endif
}

static void ddlist_init(void)
{
#if LV_USE_DROPDOWN != 0

    style_init_reset(&styles->ddlist_page);
    lv_style_set_text_line_space(&styles->ddlist_page, LV_STATE_DEFAULT, LV_DPX(20));
    lv_style_set_clip_corner(&styles->ddlist_page, LV_STATE_DEFAULT, true);

    style_init_reset(&styles->ddlist_sel);
    lv_style_set_bg_opa(&styles->ddlist_sel, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&styles->ddlist_sel, LV_STATE_DEFAULT, theme.color_primary);
    lv_style_set_text_color(&styles->ddlist_sel, LV_STATE_DEFAULT, IS_LIGHT ? lv_color_hex3(0xfff) : lv_color_hex3(0xfff));
    lv_style_set_bg_color(&styles->ddlist_sel, LV_STATE_PRESSED, COLOR_BG_PR);
    lv_style_set_text_color(&styles->ddlist_sel, LV_STATE_PRESSED, COLOR_BG_TEXT_PR);
#endif
}

static void roller_init(void)
{
#if LV_USE_ROLLER != 0
    style_init_reset(&styles->roller_bg);
    lv_style_set_text_line_space(&styles->roller_bg, LV_STATE_DEFAULT, LV_DPX(25));

    style_init_reset(&styles->roller_sel);
    lv_style_set_bg_opa(&styles->roller_sel, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&styles->roller_sel, LV_STATE_DEFAULT, theme.color_primary);
    lv_style_set_text_color(&styles->roller_sel, LV_STATE_DEFAULT, LV_COLOR_WHITE);
#endif
}

static void tabview_init(void)
{
#if LV_USE_TABVIEW != 0
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
    style_init_reset(&styles->table_cell);
    lv_style_set_border_color(&styles->table_cell, LV_STATE_DEFAULT, COLOR_BG_BORDER);
    lv_style_set_border_width(&styles->table_cell, LV_STATE_DEFAULT, 1);
    lv_style_set_border_side(&styles->table_cell, LV_STATE_DEFAULT, LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_BOTTOM);
    lv_style_set_pad_left(&styles->table_cell, LV_STATE_DEFAULT, PAD_DEF);
    lv_style_set_pad_right(&styles->table_cell, LV_STATE_DEFAULT, PAD_DEF);
    lv_style_set_pad_top(&styles->table_cell, LV_STATE_DEFAULT, PAD_DEF);
    lv_style_set_pad_bottom(&styles->table_cell, LV_STATE_DEFAULT, PAD_DEF);

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

    /* This trick is required only to avoid the garbage collection of
     * styles' data if LVGL is used in a binding (e.g. Micropython)
     * In a general case styles could be simple `static lv_style_t my style` variables*/
    if(!inited) {
        LV_GC_ROOT(_lv_theme_material_styles) = lv_mem_alloc(sizeof(theme_styles_t));
        styles = (theme_styles_t *)LV_GC_ROOT(_lv_theme_material_styles);
    }

    theme.color_primary = color_primary;
    theme.color_secondary = color_secondary;
    theme.font_small = font_small;
    theme.font_normal = font_normal;
    theme.font_subtitle = font_subtitle;
    theme.font_title = font_title;
    theme.flags = flags;

    basic_init();
    btn_init();
    btnmatrix_init();
    label_init();
    bar_init();
    img_init();
    line_init();
    slider_init();
    switch_init();
    linemeter_init();
    gauge_init();
    arc_init();
    spinner_init();
    chart_init();
    checkbox_init();
    keyboard_init();
    textarea_init();
    ddlist_init();
    roller_init();
    tabview_init();
    tileview_init();
    table_init();
    win_init();

    theme.apply_xcb = NULL;
    theme.apply_cb = theme_apply;

    inited = true;

    lv_obj_report_style_change(NULL);

    return &theme;
}


static void theme_apply(lv_theme_t * th, lv_obj_t * obj, lv_theme_style_t name)
{
    LV_UNUSED(th);

    lv_style_list_t * list;

    switch(name) {
        case LV_THEME_NONE:
            break;

        case LV_THEME_SCR:
            list = _lv_obj_get_style_list(obj, LV_OBJ_PART_MAIN);
            _lv_style_list_add_style(list, &styles->scr);
            _lv_style_list_add_style(list, &styles->sb);
            break;
        case LV_THEME_OBJ:
            list = _lv_obj_get_style_list(obj, LV_OBJ_PART_MAIN);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->sb);
            break;

#if LV_USE_BTN
        case LV_THEME_BTN:
            list = _lv_obj_get_style_list(obj, LV_BTN_PART_MAIN);
            _lv_style_list_add_style(list, &styles->btn);
            break;
#endif

#if LV_USE_BTNMATRIX
        case LV_THEME_BTNMATRIX:
            list = _lv_obj_get_style_list(obj, LV_BTNMATRIX_PART_MAIN);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->pad_small);

            list = _lv_obj_get_style_list(obj, LV_BTNMATRIX_PART_BTN);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->bg_click);
            _lv_style_list_add_style(list, &styles->btnmatrix_btn);

            list = _lv_obj_get_style_list(obj, LV_BTNMATRIX_PART_BTN_2);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->bg_click);
            _lv_style_list_add_style(list, &styles->btnmatrix_btn);
            break;
#endif

#if LV_USE_BAR
        case LV_THEME_BAR:
            list = _lv_obj_get_style_list(obj, LV_BAR_PART_MAIN);
            _lv_style_list_add_style(list, &styles->bar_bg);

            list = _lv_obj_get_style_list(obj, LV_BAR_PART_INDIC);
            _lv_style_list_add_style(list, &styles->bar_indic);
            break;
#endif

#if LV_USE_SWITCH
        case LV_THEME_SWITCH:
            list = _lv_obj_get_style_list(obj, LV_SWITCH_PART_MAIN);
            _lv_style_list_add_style(list, &styles->bar_bg);

            list = _lv_obj_get_style_list(obj, LV_SWITCH_PART_INDIC);
            _lv_style_list_add_style(list, &styles->bar_indic);

            list = _lv_obj_get_style_list(obj, LV_SWITCH_PART_KNOB);
            _lv_style_list_add_style(list, &styles->sw_knob);
            break;
#endif

#if LV_USE_IMG
        case LV_THEME_IMAGE:
            break;
#endif

#if LV_USE_IMGBTN
        case LV_THEME_IMGBTN:
            break;
#endif

#if LV_USE_LABEL
        case LV_THEME_LABEL:
            break;
#endif

#if LV_USE_LINE
        case LV_THEME_LINE:
            break;
#endif

#if LV_USE_ARC
        case LV_THEME_ARC:
            list = _lv_obj_get_style_list(obj, LV_ARC_PART_BG);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->arc_bg);
            _lv_style_list_add_style(list, &styles->sb);

            list = _lv_obj_get_style_list(obj, LV_ARC_PART_INDIC);
            _lv_style_list_add_style(list, &styles->arc_indic);

            list = _lv_obj_get_style_list(obj, LV_ARC_PART_KNOB);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->bg_click);
            _lv_style_list_add_style(list, &styles->arc_knob);
            break;
#endif

#if LV_USE_SLIDER
        case LV_THEME_SLIDER:
            list = _lv_obj_get_style_list(obj, LV_SLIDER_PART_MAIN);
            _lv_style_list_add_style(list, &styles->bar_bg);
            _lv_style_list_add_style(list, &styles->slider_bg);

            list = _lv_obj_get_style_list(obj, LV_SLIDER_PART_INDIC);
            _lv_style_list_add_style(list, &styles->bar_indic);

            list = _lv_obj_get_style_list(obj, LV_SLIDER_PART_KNOB);
            _lv_style_list_add_style(list, &styles->slider_knob);
            break;
#endif

#if LV_USE_CHECKBOX
        case LV_THEME_CHECKBOX:
            list = _lv_obj_get_style_list(obj, LV_CHECKBOX_PART_MAIN);
            _lv_style_list_add_style(list, &styles->cb_bg);
            _lv_style_list_add_style(list, &styles->sb);

            list = _lv_obj_get_style_list(obj, LV_CHECKBOX_PART_BULLET);
            _lv_style_list_add_style(list, &styles->btn);
            _lv_style_list_add_style(list, &styles->cb_bullet);
            break;
#endif

#if LV_USE_ROLLER
        case LV_THEME_ROLLER:
            list = _lv_obj_get_style_list(obj, LV_ROLLER_PART_BG);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->roller_bg);
            _lv_style_list_add_style(list, &styles->sb);

            list = _lv_obj_get_style_list(obj, LV_ROLLER_PART_SELECTED);
            _lv_style_list_add_style(list, &styles->roller_sel);
            break;
#endif

#if LV_USE_OBJMASK
        case LV_THEME_OBJMASK:
            list = _lv_obj_get_style_list(obj, LV_OBJMASK_PART_MAIN);
            break;
#endif

#if LV_USE_DROPDOWN
        case LV_THEME_DROPDOWN:
            list = _lv_obj_get_style_list(obj, LV_DROPDOWN_PART_MAIN);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->bg_click);
            _lv_style_list_add_style(list, &styles->pad_small);

            list = _lv_obj_get_style_list(obj, LV_DROPDOWN_PART_LIST);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->ddlist_page);
            _lv_style_list_add_style(list, &styles->sb);

            list = _lv_obj_get_style_list(obj, LV_DROPDOWN_PART_SELECTED);
            _lv_style_list_add_style(list, &styles->ddlist_sel);
            break;
#endif

#if LV_USE_CHART
        case LV_THEME_CHART:
            list = _lv_obj_get_style_list(obj, LV_CHART_PART_BG);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->chart_bg);
            _lv_style_list_add_style(list, &styles->pad_small);

            list = _lv_obj_get_style_list(obj, LV_CHART_PART_SERIES_BG);
            _lv_style_list_add_style(list, &styles->pad_small);
            _lv_style_list_add_style(list, &styles->chart_series_bg);

            list = _lv_obj_get_style_list(obj, LV_CHART_PART_CURSOR);
            _lv_style_list_add_style(list, &styles->chart_series_bg);

            list = _lv_obj_get_style_list(obj, LV_CHART_PART_SERIES);
            _lv_style_list_add_style(list, &styles->chart_series);
            break;
#endif
#if LV_USE_TABLE
        case LV_THEME_TABLE: {
                list = _lv_obj_get_style_list(obj, LV_TABLE_PART_BG);
                _lv_style_list_add_style(list, &styles->bg);

                int idx = 1; /* start value should be 1, not zero, since cell styles
                            start at 1 due to presence of LV_TABLE_PART_BG=0
                            in the enum (lv_table.h) */
                /* declaring idx outside loop to work with older compilers */
                for(; idx <= LV_TABLE_CELL_STYLE_CNT; idx ++) {
                    list = _lv_obj_get_style_list(obj, idx);
                    _lv_style_list_add_style(list, &styles->table_cell);
                }
                break;
            }
#endif

#if LV_USE_TEXTAREA
        case LV_THEME_TEXTAREA:
            list = _lv_obj_get_style_list(obj, LV_TEXTAREA_PART_MAIN);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->pad_small);
            _lv_style_list_add_style(list, &styles->sb);

            list = _lv_obj_get_style_list(obj, LV_TEXTAREA_PART_PLACEHOLDER);
            _lv_style_list_add_style(list, &styles->ta_placeholder);

            list = _lv_obj_get_style_list(obj, LV_TEXTAREA_PART_CURSOR);
            _lv_style_list_add_style(list, &styles->ta_cursor);
            break;

#endif

#if LV_USE_LINEMETER
        case LV_THEME_LINEMETER:
            list = _lv_obj_get_style_list(obj, LV_LINEMETER_PART_MAIN);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->lmeter);
            break;
#endif
#if LV_USE_GAUGE
        case LV_THEME_GAUGE:
            list = _lv_obj_get_style_list(obj, LV_GAUGE_PART_MAIN);
            _lv_style_list_add_style(list, &styles->bg);
            _lv_style_list_add_style(list, &styles->gauge_main);

            list = _lv_obj_get_style_list(obj, LV_GAUGE_PART_MAJOR);
            _lv_style_list_add_style(list, &styles->gauge_strong);

            list = _lv_obj_get_style_list(obj, LV_GAUGE_PART_NEEDLE);
            _lv_style_list_add_style(list, &styles->gauge_needle);
            break;
#endif
        default:
            break;
    }

    _lv_obj_refresh_style(obj, LV_OBJ_PART_ALL, LV_STYLE_PROP_ALL);
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
