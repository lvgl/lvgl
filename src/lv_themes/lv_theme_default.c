/**
 * @file lv_theme_material.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lvgl.h" /*To see all the widgets*/

#if LV_USE_THEME_DEFAULT

#include "../lv_misc/lv_gc.h"

#if defined(LV_GC_INCLUDE)
#include LV_GC_INCLUDE
#endif /* LV_ENABLE_GC */

/*********************
 *      DEFINES
 *********************/

#define RADIUS_DEFAULT LV_DPX(8)

/*SCREEN*/
#define COLOR_SCR        (IS_LIGHT ? lv_color_hex(0xeaeff3) : lv_color_hex(0x444b5a))
#define COLOR_SCR_TEXT   (IS_LIGHT ? lv_color_hex(0x3b3e42) : lv_color_hex(0xe7e9ec))

/*BUTTON*/
#define BTN_COLOR           (theme.color_primary)
#define BTN_PR_COLOR        (lv_color_darken(theme.color_primary, LV_OPA_20))
#define BTN_CHK_DIS_COLOR   (lv_color_lighten(theme.color_primary, LV_OPA_40))

#define BTN_CHK_COLOR       (theme.color_secondary)
#define BTN_CHK_PR_COLOR    (lv_color_darken(theme.color_secondary, LV_OPA_20))
#define BTN_DIS_COLOR       (lv_color_lighten(theme.color_secondary, LV_OPA_40))

#define BTN_BORDER_COLOR        theme.color_primary
#define COLOR_BTN_BORDER_PR     theme.color_primary
#define COLOR_BTN_BORDER_CHK    theme.color_primary
#define COLOR_BTN_BORDER_CHK_PR theme.color_primary
#define COLOR_BTN_BORDER_INA    (IS_LIGHT ? lv_color_hex3(0x888) : lv_color_hex(0x404040))

/*BACKGROUND*/
#define CARD_COLOR            (IS_LIGHT ? lv_color_hex(0xffffff) : lv_color_hex(0x586273))
#define CARD_PR_COLOR         (IS_LIGHT ? lv_color_hex(0xeeeeee) : lv_color_hex(0x494f57))
#define COLOR_BG_CHK        theme.color_primary
#define COLOR_BG_PR_CHK     lv_color_darken(theme.color_primary, LV_OPA_20)
#define COLOR_BG_DIS        CARD_COLOR

#define CARD_BORDER_COLOR         (IS_LIGHT ? lv_color_hex(0xd6dde3) : lv_color_hex(0x808a97))   /*dfe7ed*/
#define COLOR_BG_BORDER_PR      (IS_LIGHT ? lv_color_hex3(0xccc) : lv_color_hex(0x5f656e))
#define COLOR_BG_BORDER_CHK     (IS_LIGHT ? lv_color_hex(0xd6dde3) : lv_color_hex(0x5f656e))
#define COLOR_BG_BORDER_CHK_PR  (IS_LIGHT ? lv_color_hex3(0xccc) : lv_color_hex(0x5f656e))
#define COLOR_BG_BORDER_DIS     (IS_LIGHT ? lv_color_hex(0xd6dde3) : lv_color_hex(0x5f656e))

#define CARD_TEXT_COLOR           (IS_LIGHT ? lv_color_hex(0x3b3e42) : lv_color_hex(0xffffff))
#define CARD_TEXT_PR_COLOR        (IS_LIGHT ? lv_color_hex(0x3b3e42) : lv_color_hex(0xffffff))
#define COLOR_BG_TEXT_CHK       (IS_LIGHT ? lv_color_hex(0xffffff) : lv_color_hex(0xffffff))
#define COLOR_BG_TEXT_CHK_PR    (IS_LIGHT ? lv_color_hex(0xffffff) : lv_color_hex(0xffffff))
#define COLOR_BG_TEXT_DIS       (IS_LIGHT ? lv_color_hex3(0xaaa) : lv_color_hex3(0x999))

/*SECONDARY BACKGROUND*/
#define COLOR_GRAY            (IS_LIGHT ? lv_color_hex(0xd4d7d9) : lv_color_hex(0x45494d))
#define COLOR_BG_SEC_BORDER     (IS_LIGHT ? lv_color_hex(0xdfe7ed) : lv_color_hex(0x404040))
#define COLOR_BG_SEC_TEXT       (IS_LIGHT ? lv_color_hex(0x31404f) : lv_color_hex(0xa5a8ad))
#define COLOR_BG_SEC_TEXT_DIS   (IS_LIGHT ? lv_color_hex(0xaaaaaa) : lv_color_hex(0xa5a8ad))

#define TRANSITION_TIME         ((theme.flags & LV_THEME_MATERIAL_FLAG_NO_TRANSITION) ? 0 : 85)
#define BORDER_WIDTH            LV_DPX(2)
#define OUTLINE_WIDTH           ((theme.flags & LV_THEME_MATERIAL_FLAG_NO_FOCUS) ? 0 : LV_DPX(2))
#define IS_LIGHT (theme.flags & LV_THEME_MATERIAL_FLAG_LIGHT)

#define PAD_DEF (lv_disp_get_size_category(NULL) <= LV_DISP_SIZE_MEDIUM ? LV_DPX(15) : (LV_DPX(30)))

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_style_t scr;
    lv_style_t scrollbar;
    lv_style_t scrollbar_scrolled;
    lv_style_t card;
    lv_style_t btn;
    lv_style_t btn_disabled;
    lv_style_t btn_color;
    lv_style_t btn_color_checked;

    /*Utility*/
    lv_style_t bg_color_primary;
    lv_style_t bg_color_gray;
    lv_style_t bg_color_white;
    lv_style_t pressed;
    lv_style_t disabled;
    lv_style_t pad_zero;
    lv_style_t pad_small;
    lv_style_t pad_small_negative;
    lv_style_t focus_border;
    lv_style_t focus_outline;
    lv_style_t edit_outline;
    lv_style_t edit_border;
    lv_style_t circle;
    lv_style_t no_radius;
    lv_style_t clip_corner;
    lv_style_t grow;
    lv_style_t transition_delayed;
    lv_style_t transition_normal;

    /*Parts*/
    lv_style_t knob;
    lv_style_t indic;


#if LV_USE_ARC
    lv_style_t arc_indic;
    lv_style_t arc_bg;
    lv_style_t arc_knob;
#endif


#if LV_USE_BTNMATRIX
    lv_style_t btnmatrix_btn;
#endif

#if LV_USE_CHART
    lv_style_t chart_bg, chart_series;
#endif

#if LV_USE_CHECKBOX
    lv_style_t cb_marker, cb_marker_checked;
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

//#if LV_USE_ROLLER
//    lv_style_t roller_bg, roller_sel;
//#endif


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
static void theme_apply(lv_theme_t * th, lv_obj_t * obj);
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
    const static lv_style_prop_t trans_props[] = {
            LV_STYLE_BG_OPA, LV_STYLE_BG_COLOR,
            LV_STYLE_TRANSFORM_WIDTH, LV_STYLE_TRANSFORM_HEIGHT,
            LV_STYLE_COLOR_FILTER_OPA, LV_STYLE_COLOR_FILTER_CB, 0
    };

    static lv_style_transiton_t trans_delayed;
    lv_style_transition_init(&trans_delayed, trans_props, &lv_anim_path_def, TRANSITION_TIME, 70);

    static lv_style_transiton_t trans_slow;
    lv_style_transition_init(&trans_slow, trans_props, &lv_anim_path_def, TRANSITION_TIME * 2, 0);

    static lv_style_transiton_t trans_normal;
    lv_style_transition_init(&trans_normal, trans_props, &lv_anim_path_def, TRANSITION_TIME, 0);

    style_init_reset(&styles->transition_delayed);
    lv_style_set_transition(&styles->transition_delayed, &trans_delayed); /*Go back to default state with delay*/

    style_init_reset(&styles->transition_normal);
    lv_style_set_transition(&styles->transition_normal, &trans_normal); /*Go back to default state with delay*/


    style_init_reset(&styles->scrollbar);
    lv_style_set_bg_opa(&styles->scrollbar, LV_OPA_COVER);
    lv_style_set_bg_color(&styles->scrollbar, (IS_LIGHT ? lv_color_hex(0xcccfd1) : lv_color_hex(0x777f85)));
    lv_style_set_radius(&styles->scrollbar, LV_RADIUS_CIRCLE);
    lv_style_set_pad_left(&styles->scrollbar, LV_DPX(7));
    lv_style_set_margin_right(&styles->scrollbar,  LV_DPX(7));
    lv_style_set_margin_top(&styles->scrollbar,  LV_DPX(7));
    lv_style_set_bg_opa(&styles->scrollbar,  LV_OPA_50);
    lv_style_set_transition(&styles->scrollbar, &trans_slow);

    style_init_reset(&styles->scrollbar_scrolled);
    lv_style_set_bg_opa(&styles->scrollbar_scrolled,  LV_OPA_COVER);

    style_init_reset(&styles->scr);
    lv_style_set_bg_opa(&styles->scr, LV_OPA_COVER);
    lv_style_set_bg_color(&styles->scr, COLOR_SCR);
    lv_style_set_text_color(&styles->scr, COLOR_SCR_TEXT);

    style_init_reset(&styles->card);
    lv_style_set_radius(&styles->card, RADIUS_DEFAULT);
    lv_style_set_bg_opa(&styles->card, LV_OPA_COVER);
    lv_style_set_bg_color(&styles->card, CARD_COLOR);
    lv_style_set_border_color(&styles->card, CARD_BORDER_COLOR);
    lv_style_set_border_width(&styles->card, BORDER_WIDTH);
    lv_style_set_border_post(&styles->card, true);
    lv_style_set_text_color(&styles->card, CARD_TEXT_COLOR);
    lv_style_set_pad_all(&styles->card, PAD_DEF);

    style_init_reset(&styles->focus_border);
    lv_style_set_border_color(&styles->focus_border, theme.color_primary);

    style_init_reset(&styles->edit_border);
    lv_style_set_border_color(&styles->edit_border, theme.color_secondary);

    style_init_reset(&styles->focus_outline);
    lv_style_set_outline_color(&styles->focus_outline, theme.color_primary);
    lv_style_set_outline_width(&styles->focus_outline, OUTLINE_WIDTH);
    lv_style_set_outline_opa(&styles->focus_outline, LV_OPA_50);

    style_init_reset(&styles->edit_outline);
    lv_style_set_outline_color(&styles->edit_outline, theme.color_secondary);
    lv_style_set_outline_opa(&styles->edit_outline, LV_OPA_50);

    style_init_reset(&styles->btn);
    lv_style_set_radius(&styles->btn, RADIUS_DEFAULT);
    lv_style_set_bg_opa(&styles->btn, LV_OPA_COVER);
    lv_style_set_bg_color(&styles->btn, CARD_COLOR);
    lv_style_set_border_color(&styles->btn, CARD_BORDER_COLOR);
    lv_style_set_border_width(&styles->btn, BORDER_WIDTH);
    lv_style_set_text_color(&styles->btn, CARD_TEXT_COLOR);
    lv_style_set_pad_hor(&styles->btn, LV_DPX(40));
    lv_style_set_pad_ver(&styles->btn, LV_DPX(15));


    style_init_reset(&styles->btn_color);
    lv_style_set_border_width(&styles->btn_color, 0);
    lv_style_set_bg_color(&styles->btn_color, BTN_COLOR);
    lv_style_set_text_color(&styles->btn_color, LV_COLOR_WHITE);

    style_init_reset(&styles->btn_color_checked);
    lv_style_set_bg_color(&styles->btn_color_checked, BTN_CHK_PR_COLOR);

    style_init_reset(&styles->pressed);
    lv_style_set_color_filter_cb(&styles->pressed, lv_color_darken);
    lv_style_set_color_filter_opa(&styles->pressed, LV_OPA_20);

    style_init_reset(&styles->disabled);
    lv_style_set_color_filter_cb(&styles->disabled, lv_color_lighten);
    lv_style_set_color_filter_opa(&styles->disabled, LV_OPA_40);

    style_init_reset(&styles->clip_corner);
    lv_style_set_clip_corner(&styles->clip_corner, true);

    style_init_reset(&styles->pad_small);
    lv_coord_t pad_small_value = LV_DPX(10);
    lv_style_set_pad_all(&styles->pad_small,  pad_small_value);

    style_init_reset(&styles->pad_small_negative);
    lv_style_set_pad_all(&styles->pad_small_negative, - LV_DPX(4));

    style_init_reset(&styles->pad_zero);
    lv_style_set_pad_all(&styles->pad_zero, 0);

    style_init_reset(&styles->bg_color_primary);
    lv_style_set_bg_color(&styles->bg_color_primary, theme.color_primary);
    lv_style_set_text_color(&styles->bg_color_primary, LV_COLOR_WHITE);
    lv_style_set_bg_opa(&styles->bg_color_primary, LV_OPA_COVER);

    style_init_reset(&styles->bg_color_gray);
    lv_style_set_bg_color(&styles->bg_color_gray, COLOR_GRAY);
    lv_style_set_bg_opa(&styles->bg_color_gray, LV_OPA_COVER);
    lv_style_set_text_color(&styles->bg_color_gray, CARD_TEXT_COLOR);

    style_init_reset(&styles->bg_color_white);
    lv_style_set_bg_color(&styles->bg_color_white, LV_COLOR_WHITE);
    lv_style_set_bg_opa(&styles->bg_color_white, LV_OPA_COVER);

    style_init_reset(&styles->circle);
    lv_style_set_radius(&styles->circle, LV_RADIUS_CIRCLE);

    style_init_reset(&styles->grow);
    lv_style_set_transform_width(&styles->grow, LV_DPX(3));
    lv_style_set_transform_height(&styles->grow, LV_DPX(3));

    style_init_reset(&styles->knob);
    lv_style_set_bg_color(&styles->knob, IS_LIGHT ? theme.color_primary : LV_COLOR_WHITE);
    lv_style_set_bg_opa(&styles->knob, LV_OPA_COVER);
    lv_style_set_pad_all(&styles->knob, LV_DPX(5));
    lv_style_set_radius(&styles->knob, LV_RADIUS_CIRCLE);

#if LV_USE_CHECKBOX
    style_init_reset(&styles->cb_marker);
    lv_style_set_pad_all(&styles->cb_marker, LV_DPX(3));
    lv_style_set_border_width(&styles->cb_marker, BORDER_WIDTH);
    lv_style_set_border_color(&styles->cb_marker, theme.color_primary);
    lv_style_set_bg_color(&styles->cb_marker, LV_COLOR_WHITE);
    lv_style_set_bg_opa(&styles->cb_marker, LV_OPA_COVER);
    lv_style_set_margin_right(&styles->cb_marker,  LV_DPX(6));
    lv_style_set_radius(&styles->cb_marker, RADIUS_DEFAULT / 2);

    style_init_reset(&styles->cb_marker_checked);
    lv_style_set_content_src(&styles->cb_marker_checked, LV_SYMBOL_OK);
    lv_style_set_text_color(&styles->cb_marker_checked, LV_COLOR_WHITE);
    lv_style_set_text_font(&styles->cb_marker_checked, theme.font_small);
#endif

#if LV_USE_CHART
    style_init_reset(&styles->chart_bg);
    lv_style_set_line_width(&styles->chart_bg, LV_DPX(1));
    lv_style_set_line_dash_width(&styles->chart_bg, LV_DPX(10));
    lv_style_set_line_dash_gap(&styles->chart_bg, LV_DPX(10));
    lv_style_set_line_color(&styles->chart_bg, LV_COLOR_RED);//CARD_BORDER_COLOR);
    lv_style_set_text_color(&styles->chart_bg, LV_COLOR_RED);//CARD_BORDER_COLOR);

    style_init_reset(&styles->chart_series);
    lv_style_set_line_width(&styles->chart_series, LV_DPX(3));
//    lv_style_set_size(&styles->chart_series, LV_DPX(4));
    lv_style_set_pad_all(&styles->chart_series, LV_DPX(2));     /*Space between columns*/
    lv_style_set_radius(&styles->chart_series, LV_DPX(1));
#endif



}

static void btnmatrix_init(void)
{
#if LV_USE_BTNMATRIX != 0
    style_init_reset(&styles->btnmatrix_btn);
    lv_style_set_margin_all(&styles->btnmatrix_btn, LV_STATE_DEFAULT, LV_DPX(10));
#endif
}

static void line_init(void)
{
#if LV_USE_LINE != 0

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
    lv_style_set_line_color(&styles->arc_bg, LV_STATE_DEFAULT, COLOR_GRAY);
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
//
//static void ddlist_init(void)
//{
//#if LV_USE_DROPDOWN != 0
//
//    style_init_reset(&styles->ddlist_page);
//    lv_style_set_text_line_space(&styles->ddlist_page, LV_STATE_DEFAULT, LV_DPX(20));
//    lv_style_set_clip_corner(&styles->ddlist_page, LV_STATE_DEFAULT, true);
//
//    style_init_reset(&styles->ddlist_sel);
//    lv_style_set_bg_opa(&styles->ddlist_sel, LV_STATE_DEFAULT, LV_OPA_COVER);
//    lv_style_set_bg_color(&styles->ddlist_sel, LV_STATE_DEFAULT, theme.color_primary);
//    lv_style_set_text_color(&styles->ddlist_sel, LV_STATE_DEFAULT, IS_LIGHT ? lv_color_hex3(0xfff) : lv_color_hex3(0xfff));
//    lv_style_set_bg_color(&styles->ddlist_sel, LV_STATE_PRESSED, CARD_PR_COLOR);
//    lv_style_set_text_color(&styles->ddlist_sel, LV_STATE_PRESSED, CARD_TEXT_PR_COLOR);
//#endif
//}

//static void roller_init(void)
//{
//#if LV_USE_ROLLER != 0
//    style_init_reset(&styles->roller_bg);
//    lv_style_set_text_line_space(&styles->roller_bg, LV_STATE_DEFAULT, LV_DPX(25));
//
//    style_init_reset(&styles->roller_sel);
//    lv_style_set_bg_opa(&styles->roller_sel, LV_STATE_DEFAULT, LV_OPA_COVER);
//    lv_style_set_bg_color(&styles->roller_sel, LV_STATE_DEFAULT, theme.color_primary);
//    lv_style_set_text_color(&styles->roller_sel, LV_STATE_DEFAULT, LV_COLOR_WHITE);
//#endif
//}

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
    lv_style_set_border_color(&styles->table_cell, LV_STATE_DEFAULT, CARD_BORDER_COLOR);
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
lv_theme_t * lv_theme_default_init(lv_color_t color_primary, lv_color_t color_secondary, uint32_t flags,
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
    btnmatrix_init();
    line_init();
    linemeter_init();
    gauge_init();
    arc_init();
    spinner_init();
    chart_init();
    textarea_init();
    table_init();

    theme.apply_cb = theme_apply;

    inited = true;

    lv_obj_report_style_change(NULL);

    return &theme;
}


static void theme_apply(lv_theme_t * th, lv_obj_t * obj)
{
    LV_UNUSED(th);

    if(lv_obj_get_parent(obj) == NULL) {
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->scr);
        lv_obj_add_style(obj, LV_PART_SCROLLBAR, LV_STATE_DEFAULT, &styles->scrollbar);
        lv_obj_add_style(obj, LV_PART_SCROLLBAR, LV_STATE_SCROLLED, &styles->scrollbar_scrolled);
        return;
    }

    if(lv_obj_check_type(obj, &lv_obj)) {
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->card);
        lv_obj_add_style(obj, LV_PART_SCROLLBAR, LV_STATE_DEFAULT, &styles->scrollbar);
        lv_obj_add_style(obj, LV_PART_SCROLLBAR, LV_STATE_SCROLLED, &styles->scrollbar_scrolled);
    }
#if LV_USE_BTN
    else if(lv_obj_check_type(obj, &lv_btn)) {
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->btn);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->btn_color);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->transition_delayed);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_PRESSED, &styles->pressed);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_PRESSED, &styles->transition_normal);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_PRESSED, &styles->grow);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_CHECKED, &styles->btn_color_checked);
    }
#endif

#if LV_USE_BAR
    else if(lv_obj_check_type(obj, &lv_bar)) {
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->bg_color_gray);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->circle);
        lv_obj_add_style(obj, LV_PART_INDICATOR, LV_STATE_DEFAULT, &styles->bg_color_primary);
        lv_obj_add_style(obj, LV_PART_INDICATOR, LV_STATE_DEFAULT, &styles->circle);
    }
#endif

#if LV_USE_SLIDER
    else if(lv_obj_check_type(obj, &lv_slider)) {
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->bg_color_gray);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->circle);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->transition_delayed);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_PRESSED, &styles->transition_normal);
        lv_obj_add_style(obj, LV_PART_INDICATOR, LV_STATE_DEFAULT, &styles->bg_color_primary);
        lv_obj_add_style(obj, LV_PART_INDICATOR, LV_STATE_DEFAULT, &styles->circle);
        lv_obj_add_style(obj, LV_PART_KNOB, LV_STATE_DEFAULT, &styles->knob);
        lv_obj_add_style(obj, LV_PART_KNOB, LV_STATE_PRESSED, &styles->grow);
        lv_obj_add_style(obj, LV_PART_KNOB, LV_STATE_DEFAULT, &styles->transition_delayed);
        lv_obj_add_style(obj, LV_PART_KNOB, LV_STATE_PRESSED, &styles->transition_normal);
    }
#endif

#if LV_USE_CHECKBOX
    else if(lv_obj_check_type(obj, &lv_checkbox)) {
        lv_obj_add_style(obj, LV_PART_MARKER, LV_STATE_DEFAULT, &styles->cb_marker);
        lv_obj_add_style(obj, LV_PART_MARKER, LV_STATE_CHECKED, &styles->bg_color_primary);
        lv_obj_add_style(obj, LV_PART_MARKER, LV_STATE_CHECKED, &styles->cb_marker_checked);
        lv_obj_add_style(obj, LV_PART_MARKER, LV_STATE_PRESSED, &styles->pressed);
        lv_obj_add_style(obj, LV_PART_MARKER, LV_STATE_PRESSED, &styles->grow);
        lv_obj_add_style(obj, LV_PART_MARKER, LV_STATE_PRESSED, &styles->transition_normal);
        lv_obj_add_style(obj, LV_PART_MARKER, LV_STATE_DEFAULT, &styles->transition_delayed);
    }
#endif

#if LV_USE_SWITCH
    else if(lv_obj_check_type(obj, &lv_switch)) {
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->bg_color_gray);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->circle);
        lv_obj_add_style(obj, LV_PART_INDICATOR, LV_STATE_DEFAULT, &styles->bg_color_primary);
        lv_obj_add_style(obj, LV_PART_INDICATOR, LV_STATE_DEFAULT, &styles->circle);
        lv_obj_add_style(obj, LV_PART_KNOB, LV_STATE_DEFAULT, &styles->knob);
        lv_obj_add_style(obj, LV_PART_KNOB, LV_STATE_DEFAULT, &styles->bg_color_white);
        lv_obj_add_style(obj, LV_PART_KNOB, LV_STATE_DEFAULT, &styles->pad_small_negative);
    }
#endif

#if LV_USE_SWITCH
    else if(lv_obj_check_type(obj, &lv_chart)) {
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->card);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->chart_bg);
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->pad_zero);
        lv_obj_add_style(obj, LV_PART_SCROLLBAR, LV_STATE_DEFAULT, &styles->scrollbar);
        lv_obj_add_style(obj, LV_PART_SCROLLBAR, LV_STATE_SCROLLED, &styles->scrollbar_scrolled);
        lv_obj_add_style(obj, LV_PART_SERIES, LV_STATE_DEFAULT, &styles->chart_series);
    }
#endif
#if LV_USE_ROLLER
    else if(lv_obj_check_type(obj, &lv_roller)) {
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->card);
//        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->pad_zero);
        lv_obj_add_style(obj, LV_PART_HIGHLIGHT, LV_STATE_DEFAULT, &styles->bg_color_primary);
//        lv_obj_add_style(obj, LV_PART_SCROLLBAR, LV_STATE_DEFAULT, &styles->scrollbar);
//        lv_obj_add_style(obj, LV_PART_SCROLLBAR, LV_STATE_SCROLLED, &styles->scrollbar_scrolled);
//        lv_obj_add_style(obj, LV_PART_SERIES, LV_STATE_DEFAULT, &styles->chart_series);
    }
#endif

#if LV_USE_DROPDOWN
    else if(lv_obj_check_type(obj, &lv_dropdown)) {
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->card);
    }
    else if(lv_obj_check_type(obj, &lv_dropdown_list)) {
        lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &styles->card);
        lv_obj_add_style(obj, LV_PART_HIGHLIGHT, LV_STATE_DEFAULT, &styles->bg_color_primary);
        lv_obj_add_style(obj, LV_PART_HIGHLIGHT, LV_STATE_PRESSED, &styles->bg_color_gray);
    }
#endif

#if LV_USE_BTNMATRIX
case LV_THEME_BTNMATRIX:
    list = _lv_obj_get_style_list(obj, LV_BTNMATRIX_PART_MAIN);
    _lv_style_list_add_style(list, &styles->card);
    _lv_style_list_add_style(list, &styles->pad_small);

    list = _lv_obj_get_style_list(obj, LV_BTNMATRIX_PART_BTN);
    _lv_style_list_add_style(list, &styles->card);
    _lv_style_list_add_style(list, &styles->bg_click);
    _lv_style_list_add_style(list, &styles->btnmatrix_btn);

    list = _lv_obj_get_style_list(obj, LV_BTNMATRIX_PART_BTN_2);
    _lv_style_list_add_style(list, &styles->card);
    _lv_style_list_add_style(list, &styles->bg_click);
    _lv_style_list_add_style(list, &styles->btnmatrix_btn);
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

#if LV_USE_LINE
case LV_THEME_LINE:
    break;
#endif

#if LV_USE_ARC
case LV_THEME_ARC:
    list = _lv_obj_get_style_list(obj, LV_ARC_PART_BG);
    _lv_style_list_add_style(list, &styles->card);
    _lv_style_list_add_style(list, &styles->arc_bg);
    _lv_style_list_add_style(list, &styles->sb);

    list = _lv_obj_get_style_list(obj, LV_ARC_PART_INDIC);
    _lv_style_list_add_style(list, &styles->arc_indic);

    list = _lv_obj_get_style_list(obj, LV_ARC_PART_KNOB);
    _lv_style_list_add_style(list, &styles->card);
    _lv_style_list_add_style(list, &styles->bg_click);
    _lv_style_list_add_style(list, &styles->arc_knob);
    break;
#endif

#if LV_USE_OBJMASK
case LV_THEME_OBJMASK:
    list = _lv_obj_get_style_list(obj, LV_OBJMASK_PART_MAIN);
    break;
#endif

//#if LV_USE_DROPDOWN
//case LV_THEME_DROPDOWN:
//    list = _lv_obj_get_style_list(obj, LV_DROPDOWN_PART_MAIN);
//    _lv_style_list_add_style(list, &styles->card);
//    _lv_style_list_add_style(list, &styles->bg_click);
//    _lv_style_list_add_style(list, &styles->pad_small);
//
//    list = _lv_obj_get_style_list(obj, LV_DROPDOWN_PART_LIST);
//    _lv_style_list_add_style(list, &styles->card);
//    _lv_style_list_add_style(list, &styles->ddlist_page);
//    _lv_style_list_add_style(list, &styles->sb);
//
//    list = _lv_obj_get_style_list(obj, LV_DROPDOWN_PART_SELECTED);
//    _lv_style_list_add_style(list, &styles->ddlist_sel);
//    break;
//#endif

//#if LV_USE_CHART
//case LV_THEME_CHART:
//    list = _lv_obj_get_style_list(obj, LV_CHART_PART_BG);
//    _lv_style_list_add_style(list, &styles->card);
//    _lv_style_list_add_style(list, &styles->chart_bg);
//    _lv_style_list_add_style(list, &styles->pad_small);
//
//    list = _lv_obj_get_style_list(obj, LV_CHART_PART_SERIES_BG);
//    _lv_style_list_add_style(list, &styles->pad_small);
//    _lv_style_list_add_style(list, &styles->chart_series_bg);
//
//    list = _lv_obj_get_style_list(obj, LV_CHART_PART_CURSOR);
//    _lv_style_list_add_style(list, &styles->chart_series_bg);
//
//    list = _lv_obj_get_style_list(obj, LV_CHART_PART_SERIES);
//    _lv_style_list_add_style(list, &styles->chart_series);
//    break;
//#endif
#if LV_USE_TABLE
case LV_THEME_TABLE: {
    list = _lv_obj_get_style_list(obj, LV_TABLE_PART_BG);
    _lv_style_list_add_style(list, &styles->card);

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
    _lv_style_list_add_style(list, &styles->card);
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
    _lv_style_list_add_style(list, &styles->card);
    _lv_style_list_add_style(list, &styles->lmeter);
    break;
#endif
#if LV_USE_GAUGE
case LV_THEME_GAUGE:
    list = _lv_obj_get_style_list(obj, LV_GAUGE_PART_MAIN);
    _lv_style_list_add_style(list, &styles->card);
    _lv_style_list_add_style(list, &styles->gauge_main);

    list = _lv_obj_get_style_list(obj, LV_GAUGE_PART_MAJOR);
    _lv_style_list_add_style(list, &styles->gauge_strong);

    list = _lv_obj_get_style_list(obj, LV_GAUGE_PART_NEEDLE);
    _lv_style_list_add_style(list, &styles->gauge_needle);
    break;
#endif
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
