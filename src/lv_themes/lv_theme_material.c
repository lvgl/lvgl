/**
 * @file lv_theme_material.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include "lv_theme.h"
#include "../lv_widgets/lv_img.h"
#include "../lv_misc/lv_types.h"

#if LV_USE_THEME_MATERIAL

/*********************
 *      DEFINES
 *********************/
#define TRANSITION_TIME     200
#define BORDER_WIDTH        (LV_DPI / 50 > 0 ? LV_DPI / 50 : 1)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_theme_material_apply(lv_obj_t * obj, lv_theme_style_t name);

static inline lv_color_t color_scr(lv_state_t state);
static inline lv_color_t color_panel(lv_state_t state);
static inline lv_color_t color_bg(lv_state_t state);
static inline lv_color_t color_btn(lv_state_t state);
static inline lv_color_t color_scr_border(lv_state_t state);
static inline lv_color_t color_panel_border(lv_state_t state);
static inline lv_color_t color_bg_border(lv_state_t state);
static inline lv_color_t color_btn_border(lv_state_t state);
static inline lv_color_t color_scr_txt(lv_state_t state);
static inline lv_color_t color_panel_txt(lv_state_t state);
static inline lv_color_t color_bg_txt(lv_state_t state);
static inline lv_color_t color_btn_txt(lv_state_t state);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_theme_t theme;
static lv_color_t _color_primary;
static lv_color_t _color_secondary;
static lv_font_t * _font_small;
static lv_font_t * _font_normal;
static lv_font_t * _font_subtitle;
static lv_font_t * _font_title;

static lv_style_t scr;
static lv_style_t panel; /*General fancy panel like car to hold other objects*/
static lv_style_t bg;
static lv_style_t btn;


#if LV_USE_ARC
static lv_style_t arc;
static lv_style_t arc_bg;
#endif

#if LV_USE_BAR
static lv_style_t bar_bg;
static lv_style_t bar_indic;
#endif

#if LV_USE_BTNMATRIX
static lv_style_t btnm_bg, btnm_btn;
#endif

#if LV_USE_CALENDAR
static lv_style_t calendar_date_nums, calendar_header, calendar_daynames;
#endif

#if LV_USE_CPICKER
static lv_style_t cpicker_bg, cpicker_indic;
#endif

#if LV_USE_CHART
static lv_style_t chart_series_bg, chart_series;
#endif

#if LV_USE_CHECKBOX
static lv_style_t cb_bg, cb_bullet;
#endif

#if LV_USE_DROPDOWN
static lv_style_t ddlist_btn, ddlist_page, ddlist_sel;
#endif

#if LV_USE_GAUGE
static lv_style_t gauge_main, gauge_strong, gauge_needle;
#endif

#if LV_USE_KEYBOARD
static lv_style_t kb_bg, kb_btn;
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
static lv_style_t mbox_btn_bg;

#endif

#if LV_USE_PAGE
static lv_style_t sb;
#endif

#if LV_USE_ROLLER
static lv_style_t roller_bg, roller_sel;
#endif

#if LV_USE_SLIDER
static lv_style_t slider_knob;
#endif

#if LV_USE_SPINBOX
static lv_style_t spinbox_btn, spinbox_cursor;
#endif

#if LV_USE_SWITCH
static lv_style_t sw_knob;
#endif

#if LV_USE_TABLE
static lv_style_t table_cell;
#endif

#if LV_USE_TABVIEW
static lv_style_t tabview_btns, tabview_btns_bg, tabview_indic, tabview_page_scrl;
#endif

#if LV_USE_TEXTAREA
static lv_style_t ta_bg, ta_cursor, ta_oneline, ta_placeholder;
#endif

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/


static void basic_init(void)
{
    lv_style_init(&scr);
    lv_style_set_bg_opa(&scr, LV_STATE_NORMAL, LV_OPA_COVER);
    lv_style_set_bg_color(&scr, LV_STATE_NORMAL, color_scr(LV_STATE_NORMAL));
    lv_style_set_text_color(&scr, LV_STATE_NORMAL, color_scr_txt(LV_STATE_NORMAL));
    lv_style_set_value_color(&scr, LV_STATE_NORMAL, color_scr_txt(LV_STATE_NORMAL));
    lv_style_set_border_width(&scr, LV_STATE_NORMAL, 1);
    lv_style_set_border_color(&scr, LV_STATE_NORMAL, color_scr_border(LV_STATE_NORMAL));
    lv_style_set_border_post(&scr, LV_STATE_NORMAL, true);

    lv_style_init(&panel);
    lv_style_set_radius(&panel, LV_STATE_NORMAL, LV_DPI / 25);
    lv_style_set_bg_opa(&panel, LV_STATE_NORMAL, LV_OPA_COVER);
    lv_style_set_bg_color(&panel, LV_STATE_NORMAL, color_panel(LV_STATE_NORMAL));
    lv_style_set_border_color(&panel, LV_STATE_NORMAL, color_panel_border(LV_STATE_NORMAL));
    lv_style_set_border_color(&panel, LV_STATE_FOCUSED, color_panel_border(LV_STATE_FOCUSED));
    lv_style_set_border_width(&panel, LV_STATE_NORMAL, BORDER_WIDTH);
    lv_style_set_border_side(&panel, LV_STATE_NORMAL , LV_BORDER_SIDE_TOP);
    lv_style_set_text_font(&panel, LV_STATE_NORMAL, _font_normal);
    lv_style_set_text_color(&panel, LV_STATE_NORMAL, color_panel_txt(LV_STATE_NORMAL));
    lv_style_set_value_color(&panel, LV_STATE_NORMAL, color_panel_txt(LV_STATE_NORMAL));
    lv_style_set_image_recolor(&panel, LV_STATE_NORMAL, color_panel_txt(LV_STATE_NORMAL));
    lv_style_set_line_color(&panel, LV_STATE_NORMAL, color_panel_txt(LV_STATE_NORMAL));
    lv_style_set_line_width(&panel, LV_STATE_NORMAL, 1);
    lv_style_set_trans_time(&panel, LV_STATE_NORMAL, TRANSITION_TIME);
    lv_style_set_pad_left(&panel, LV_STATE_NORMAL, LV_DPI / 5);
    lv_style_set_pad_right(&panel, LV_STATE_NORMAL, LV_DPI / 5);
    lv_style_set_pad_top(&panel, LV_STATE_NORMAL, LV_DPI / 5);
    lv_style_set_pad_bottom(&panel, LV_STATE_NORMAL, LV_DPI / 5);
    lv_style_set_pad_inner(&panel, LV_STATE_NORMAL, LV_DPI / 5);
    lv_style_set_trans_prop_1(&panel, LV_STATE_NORMAL, LV_STYLE_BORDER_COLOR);

    lv_style_init(&bg);
    lv_style_set_radius(&bg, LV_STATE_NORMAL, LV_DPI / 25);
    lv_style_set_bg_opa(&bg, LV_STATE_NORMAL, LV_OPA_COVER);
    lv_style_set_bg_color(&bg, LV_STATE_NORMAL, color_bg(LV_STATE_NORMAL));
    lv_style_set_border_color(&bg, LV_STATE_NORMAL, color_bg_border(LV_STATE_NORMAL));
    lv_style_set_border_color(&bg, LV_STATE_FOCUSED, color_bg_border(LV_STATE_FOCUSED));
    lv_style_set_border_width(&bg, LV_STATE_NORMAL, BORDER_WIDTH);
    lv_style_set_text_font(&bg, LV_STATE_NORMAL, _font_normal);
    lv_style_set_text_color(&bg, LV_STATE_NORMAL, color_bg_txt(LV_STATE_NORMAL));
    lv_style_set_value_color(&bg, LV_STATE_NORMAL, color_bg_txt(LV_STATE_NORMAL));
    lv_style_set_image_recolor(&bg, LV_STATE_NORMAL, color_bg_txt(LV_STATE_NORMAL));
    lv_style_set_line_color(&bg, LV_STATE_NORMAL, color_bg_txt(LV_STATE_NORMAL));
    lv_style_set_line_width(&bg, LV_STATE_NORMAL, 1);
    lv_style_set_pad_left(&bg, LV_STATE_NORMAL, LV_DPI / 5);
    lv_style_set_pad_right(&bg, LV_STATE_NORMAL, LV_DPI / 5);
    lv_style_set_pad_top(&bg, LV_STATE_NORMAL, LV_DPI / 5);
    lv_style_set_pad_bottom(&bg, LV_STATE_NORMAL, LV_DPI / 5);
    lv_style_set_pad_inner(&bg, LV_STATE_NORMAL, LV_DPI / 5);
    lv_style_set_trans_time(&panel, LV_STATE_NORMAL, TRANSITION_TIME);

    lv_style_init(&btn);
    lv_style_set_radius(&btn, LV_STATE_NORMAL, LV_RADIUS_CIRCLE);
    lv_style_set_bg_opa(&btn, LV_STATE_NORMAL, LV_OPA_COVER);
    lv_style_set_bg_color(&btn, LV_STATE_NORMAL, color_btn(LV_STATE_NORMAL));
    lv_style_set_bg_color(&btn, LV_STATE_PRESSED, color_btn(LV_STATE_PRESSED));
    lv_style_set_bg_color(&btn, LV_STATE_CHECKED, color_btn(LV_STATE_CHECKED));
    lv_style_set_bg_color(&btn, LV_STATE_CHECKED | LV_STATE_PRESSED, color_btn(LV_STATE_CHECKED | LV_STATE_PRESSED));
    lv_style_set_bg_color(&btn, LV_STATE_DISABLED, color_btn(LV_STATE_DISABLED));
    lv_style_set_border_color(&btn, LV_STATE_NORMAL, color_btn_border(LV_STATE_NORMAL));
    lv_style_set_border_color(&btn, LV_STATE_PRESSED, color_btn_border(LV_STATE_PRESSED));
    lv_style_set_border_width(&btn, LV_STATE_NORMAL, BORDER_WIDTH);
    lv_style_set_border_width(&btn, LV_STATE_CHECKED, 0);
    lv_style_set_text_color(&btn, LV_STATE_NORMAL, color_btn_txt(LV_STATE_NORMAL));
    lv_style_set_text_color(&btn, LV_STATE_PRESSED, color_btn_txt(LV_STATE_PRESSED));
    lv_style_set_text_color(&btn, LV_STATE_PRESSED, color_btn_txt(LV_STATE_CHECKED));
    lv_style_set_text_color(&btn, LV_STATE_CHECKED | LV_STATE_PRESSED, color_btn_txt(LV_STATE_CHECKED | LV_STATE_PRESSED));
    lv_style_set_text_color(&btn, LV_STATE_DISABLED,color_btn_txt(LV_STATE_DISABLED));
    lv_style_set_image_recolor(&btn, LV_STATE_NORMAL, color_btn_txt(LV_STATE_NORMAL));
    lv_style_set_image_recolor(&btn, LV_STATE_PRESSED, color_btn_txt(LV_STATE_PRESSED));
    lv_style_set_image_recolor(&btn, LV_STATE_PRESSED, color_btn_txt(LV_STATE_CHECKED));
    lv_style_set_image_recolor(&btn, LV_STATE_CHECKED | LV_STATE_PRESSED, color_btn_txt(LV_STATE_CHECKED | LV_STATE_PRESSED));
    lv_style_set_image_recolor(&btn, LV_STATE_DISABLED,color_btn_txt(LV_STATE_DISABLED));
    lv_style_set_value_color(&btn, LV_STATE_NORMAL, color_btn_txt(LV_STATE_NORMAL));

    lv_style_set_pad_left(&btn, LV_STATE_NORMAL, LV_DPI / 5);
    lv_style_set_pad_right(&btn, LV_STATE_NORMAL, LV_DPI / 5);
    lv_style_set_pad_top(&btn, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_pad_bottom(&btn, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_pad_inner(&btn, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_outline_width(&btn, LV_STATE_NORMAL, 3);
    lv_style_set_outline_opa(&btn, LV_STATE_NORMAL, LV_OPA_0);
    lv_style_set_outline_opa(&btn, LV_STATE_FOCUSED, LV_OPA_50);
    lv_style_set_outline_color(&btn, LV_STATE_NORMAL, _color_primary);
    lv_style_set_trans_time(&btn, LV_STATE_NORMAL, TRANSITION_TIME);
    lv_style_set_trans_prop_5(&btn, LV_STATE_NORMAL, LV_STYLE_BG_COLOR);
    lv_style_set_trans_prop_6(&btn, LV_STATE_NORMAL, LV_STYLE_OUTLINE_OPA);
    lv_style_set_trans_delay(&btn, LV_STATE_NORMAL, 100);
    lv_style_set_trans_delay(&btn, LV_STATE_PRESSED, 0);
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
    lv_style_init(&bar_bg);
    lv_style_set_radius(&bar_bg, LV_STATE_NORMAL, LV_RADIUS_CIRCLE);
    lv_style_set_bg_opa(&bar_bg, LV_STATE_NORMAL, LV_OPA_COVER);
    lv_style_set_bg_color(&bar_bg, LV_STATE_NORMAL, color_bg(LV_STATE_NORMAL));
    lv_style_set_value_color(&bar_bg, LV_STATE_NORMAL, LV_COLOR_WHITE);
    lv_style_set_outline_color(&bar_bg, LV_STATE_NORMAL, _color_primary);
    lv_style_set_outline_opa(&bar_bg, LV_STATE_NORMAL, LV_OPA_TRANSP);
    lv_style_set_outline_opa(&bar_bg, LV_STATE_FOCUSED, LV_OPA_50);
    lv_style_set_outline_width(&bar_bg, LV_STATE_NORMAL, 3);
    lv_style_set_trans_time(&bar_bg, LV_STATE_NORMAL, TRANSITION_TIME);
    lv_style_set_trans_prop_6(&bar_bg, LV_STATE_NORMAL, LV_STYLE_OUTLINE_OPA);

    lv_style_init(&bar_indic);
    lv_style_set_bg_opa(&bar_indic, LV_STATE_NORMAL, LV_OPA_COVER);
    lv_style_set_radius(&bar_indic, LV_STATE_NORMAL, LV_RADIUS_CIRCLE);
    lv_style_set_bg_color(&bar_indic, LV_STATE_NORMAL, _color_primary);
    lv_style_set_bg_color(&bar_indic, LV_STATE_DISABLED, lv_color_hex3(0x888));
    lv_style_set_value_color(&bar_indic, LV_STATE_NORMAL, LV_COLOR_WHITE);
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
    lv_style_init(&led);
    lv_style_set_bg_opa(&led, LV_STATE_NORMAL, LV_OPA_COVER);
    lv_style_set_bg_color(&led, LV_STATE_NORMAL, _color_primary);
    lv_style_set_border_width(&led, LV_STATE_NORMAL, 2);
    lv_style_set_border_opa(&led, LV_STATE_NORMAL, LV_OPA_50);
    lv_style_set_border_color(&led, LV_STATE_NORMAL, lv_color_lighten(_color_primary, LV_OPA_30));
    lv_style_set_radius(&led, LV_STATE_NORMAL, LV_RADIUS_CIRCLE);
    lv_style_set_shadow_width(&led, LV_STATE_NORMAL, LV_DPI / 4);
    lv_style_set_shadow_color(&led, LV_STATE_NORMAL, _color_primary);
    lv_style_set_shadow_spread(&led, LV_STATE_NORMAL, LV_DPI / 16);
#endif
}

static void slider_init(void)
{
#if LV_USE_SLIDER != 0
    lv_style_init(&slider_knob);
    lv_style_set_bg_opa(&slider_knob, LV_STATE_NORMAL, LV_OPA_COVER);
    lv_style_set_bg_color(&slider_knob, LV_STATE_NORMAL, LV_COLOR_WHITE);
    lv_style_set_value_color(&slider_knob, LV_STATE_NORMAL, LV_COLOR_WHITE);
    lv_style_set_radius(&slider_knob, LV_STATE_NORMAL, LV_RADIUS_CIRCLE);
    lv_style_set_pad_left(&slider_knob, LV_STATE_NORMAL, LV_DPI/20);
    lv_style_set_pad_right(&slider_knob, LV_STATE_NORMAL, LV_DPI/20);
    lv_style_set_pad_top(&slider_knob, LV_STATE_NORMAL, LV_DPI/20);
    lv_style_set_pad_bottom(&slider_knob, LV_STATE_NORMAL, LV_DPI/20);

#endif
}

static void switch_init(void)
{
#if LV_USE_SWITCH != 0
    lv_style_init(&sw_knob);
    lv_style_set_bg_opa(&sw_knob, LV_STATE_NORMAL, LV_OPA_COVER);
    lv_style_set_bg_color(&sw_knob, LV_STATE_NORMAL, LV_COLOR_WHITE);
    lv_style_set_radius(&sw_knob, LV_STATE_NORMAL, LV_RADIUS_CIRCLE);
    lv_style_set_pad_top(&sw_knob, LV_STATE_NORMAL,    - LV_DPI/20);
    lv_style_set_pad_bottom(&sw_knob, LV_STATE_NORMAL, - LV_DPI/20);
    lv_style_set_pad_left(&sw_knob, LV_STATE_NORMAL,   - LV_DPI/20);
    lv_style_set_pad_right(&sw_knob, LV_STATE_NORMAL,  - LV_DPI/20);
#endif
}

static void linemeter_init(void)
{
#if LV_USE_LINEMETER != 0
    lv_style_init(&lmeter);
    lv_style_copy(&lmeter, &panel);
    lv_style_set_radius(&lmeter, LV_STATE_NORMAL, LV_RADIUS_CIRCLE);
    lv_style_set_border_side(&lmeter, LV_STATE_NORMAL, LV_BORDER_SIDE_FULL);
    lv_style_set_bg_opa(&lmeter, LV_STATE_NORMAL, LV_OPA_COVER);
    lv_style_set_pad_left(&lmeter, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_pad_right(&lmeter, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_pad_top(&lmeter, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_pad_inner(&lmeter, LV_STATE_NORMAL, LV_DPI / 6);
    lv_style_set_scale_width(&lmeter, LV_STATE_NORMAL, LV_DPI/12);

    lv_style_set_line_color(&lmeter, LV_STATE_NORMAL, _color_primary);
    lv_style_set_scale_grad_color(&lmeter, LV_STATE_NORMAL, _color_primary);
    lv_style_set_scale_end_color(&lmeter, LV_STATE_NORMAL, lv_color_hex3(0x888));
    lv_style_set_line_rounded(&lmeter, LV_STATE_NORMAL, true);
    lv_style_set_line_width(&lmeter, LV_STATE_NORMAL, LV_DPI / 25);
    lv_style_set_scale_end_line_width(&lmeter, LV_STATE_NORMAL, LV_DPI / 50);
#endif
}

static void gauge_init(void)
{
#if LV_USE_GAUGE != 0
    lv_style_init(&gauge_main);
    lv_style_copy(&gauge_main, &panel);
    lv_style_set_radius(&gauge_main, LV_STATE_NORMAL, LV_RADIUS_CIRCLE);
    lv_style_set_border_side(&gauge_main, LV_STATE_NORMAL, LV_BORDER_SIDE_FULL);
    lv_style_set_line_color(&gauge_main, LV_STATE_NORMAL, lv_color_hex3(0x888));
    lv_style_set_scale_grad_color(&gauge_main, LV_STATE_NORMAL, lv_color_hex3(0x888));
    lv_style_set_scale_end_color(&gauge_main, LV_STATE_NORMAL, _color_primary);
    lv_style_set_line_width(&gauge_main, LV_STATE_NORMAL, LV_DPI / 50);
    lv_style_set_scale_end_line_width(&gauge_main, LV_STATE_NORMAL, LV_DPI / 25);
    lv_style_set_scale_end_border_width(&gauge_main, LV_STATE_NORMAL, LV_DPI / 50);
    lv_style_set_bg_opa(&gauge_main, LV_STATE_NORMAL, LV_OPA_COVER);
    lv_style_set_pad_left(&gauge_main, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_pad_right(&gauge_main, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_pad_top(&gauge_main, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_pad_inner(&gauge_main, LV_STATE_NORMAL, LV_DPI / 6);
    lv_style_set_scale_width(&gauge_main, LV_STATE_NORMAL, LV_DPI/12);

    lv_style_init(&gauge_strong);
    lv_style_set_line_color(&gauge_strong, LV_STATE_NORMAL, lv_color_hex3(0x888));
    lv_style_set_scale_grad_color(&gauge_strong, LV_STATE_NORMAL, lv_color_hex3(0x888));
    lv_style_set_scale_end_color(&gauge_strong, LV_STATE_NORMAL, _color_primary);
    lv_style_set_line_width(&gauge_strong, LV_STATE_NORMAL, LV_DPI / 25);
    lv_style_set_scale_end_line_width(&gauge_strong, LV_STATE_NORMAL, LV_DPI / 25);
    lv_style_set_scale_width(&gauge_strong, LV_STATE_NORMAL, LV_DPI/7);

    lv_style_init(&gauge_needle);
    lv_style_set_line_color(&gauge_needle, LV_STATE_NORMAL, LV_COLOR_WHITE);
    lv_style_set_line_width(&gauge_needle, LV_STATE_NORMAL, LV_DPI / 20);
    lv_style_set_bg_opa(&gauge_needle, LV_STATE_NORMAL, LV_OPA_COVER);
    lv_style_set_bg_color(&gauge_needle, LV_STATE_NORMAL, LV_COLOR_WHITE);
    lv_style_set_radius(&gauge_needle, LV_STATE_NORMAL, LV_RADIUS_CIRCLE);
    lv_style_set_size(&gauge_needle, LV_STATE_NORMAL, LV_DPI / 7);
    lv_style_set_pad_inner(&gauge_needle, LV_STATE_NORMAL, LV_DPI / 10);
#endif
}

static void arc_init(void)
{
#if LV_USE_ARC != 0
    lv_style_init(&arc);
    lv_style_set_line_color(&arc, LV_STATE_NORMAL, _color_primary);
    lv_style_set_line_width(&arc, LV_STATE_NORMAL, LV_DPI / 8);
    lv_style_set_line_rounded(&arc, LV_STATE_NORMAL, true);

    lv_style_init(&arc_bg);
    lv_style_set_line_color(&arc_bg, LV_STATE_NORMAL, LV_COLOR_GRAY);
    lv_style_set_line_width(&arc_bg, LV_STATE_NORMAL, LV_DPI / 8);
    lv_style_set_line_rounded(&arc_bg, LV_STATE_NORMAL, true);

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
    lv_style_init(&chart_series_bg);
    lv_style_set_line_width(&chart_series_bg, LV_STATE_NORMAL , 1);
    lv_style_set_line_dash_width(&chart_series_bg, LV_STATE_NORMAL, 4);
    lv_style_set_line_dash_gap(&chart_series_bg, LV_STATE_NORMAL, 4);
    lv_style_set_pad_bottom(&chart_series_bg, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_pad_left(&chart_series_bg, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_pad_right(&chart_series_bg, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_line_color(&chart_series_bg, LV_STATE_NORMAL, lv_color_hex3(0x555));

    lv_style_init(&chart_series);
    lv_style_set_line_width(&chart_series, LV_STATE_NORMAL, 1);
    lv_style_set_size(&chart_series, LV_STATE_NORMAL, 2);
    lv_style_set_pad_inner(&chart_series, LV_STATE_NORMAL, 2);
    lv_style_set_radius(&chart_series, LV_STATE_NORMAL, 1);
    lv_style_set_bg_opa(&chart_series, LV_STATE_NORMAL, LV_OPA_COVER);
    lv_style_set_bg_grad_dir(&chart_series, LV_STATE_NORMAL, LV_GRAD_DIR_VER);
    lv_style_set_bg_main_stop(&chart_series, LV_STATE_NORMAL, 96);
    lv_style_set_bg_grad_stop(&chart_series, LV_STATE_NORMAL, 0);

#endif
}

static void calendar_init(void)
{
#if LV_USE_CALENDAR

    lv_style_init(&calendar_header);
    lv_style_set_pad_top(&calendar_header, LV_STATE_NORMAL, LV_DPI / 7);
    lv_style_set_pad_left(&calendar_header, LV_STATE_NORMAL , LV_DPI / 7);
    lv_style_set_pad_right(&calendar_header, LV_STATE_NORMAL , LV_DPI / 7);
    lv_style_set_pad_bottom(&calendar_header, LV_STATE_NORMAL , LV_DPI / 7);
    lv_style_set_text_color(&calendar_header, LV_STATE_PRESSED, LV_COLOR_WHITE);

    lv_style_init(&calendar_daynames);
    lv_style_set_text_color(&calendar_daynames, LV_STATE_NORMAL, lv_color_hex3(0xeee));
    lv_style_set_pad_left(&calendar_daynames, LV_STATE_NORMAL , LV_DPI / 7);
    lv_style_set_pad_right(&calendar_daynames, LV_STATE_NORMAL , LV_DPI / 7);
    lv_style_set_pad_bottom(&calendar_daynames, LV_STATE_NORMAL , LV_DPI / 7);

    lv_style_init(&calendar_date_nums);
    lv_style_set_radius(&calendar_date_nums, LV_STATE_NORMAL, LV_DPI / 50);
    lv_style_set_text_color(&calendar_date_nums, LV_STATE_CHECKED, LV_COLOR_WHITE);
    lv_style_set_text_color(&calendar_date_nums, LV_STATE_FOCUSED, _color_primary);

    lv_style_set_bg_opa(&calendar_date_nums, LV_STATE_CHECKED , LV_OPA_20);
    lv_style_set_bg_opa(&calendar_date_nums, LV_STATE_PRESSED , LV_OPA_20);
    lv_style_set_bg_opa(&calendar_date_nums, LV_STATE_PRESSED | LV_STATE_CHECKED , LV_OPA_40);
    lv_style_set_bg_color(&calendar_date_nums, LV_STATE_CHECKED, LV_COLOR_WHITE);
    lv_style_set_border_width(&calendar_date_nums, LV_STATE_CHECKED , 2);
    lv_style_set_border_side(&calendar_date_nums, LV_STATE_CHECKED , LV_BORDER_SIDE_LEFT);
    lv_style_set_border_color(&calendar_date_nums,LV_STATE_CHECKED, _color_primary);
    lv_style_set_pad_inner(&calendar_date_nums, LV_STATE_NORMAL, LV_DPI / 30);
    lv_style_set_pad_left(&calendar_date_nums, LV_STATE_NORMAL , LV_DPI / 7);
    lv_style_set_pad_right(&calendar_date_nums, LV_STATE_NORMAL , LV_DPI / 7);
    lv_style_set_pad_bottom(&calendar_date_nums, LV_STATE_NORMAL , LV_DPI / 7);
#endif
}

static void cpicker_init(void)
{
#if LV_USE_CPICKER

    lv_style_init(&cpicker_bg);
    lv_style_set_scale_width(&cpicker_bg, LV_STATE_NORMAL, LV_DPI / 4);
    lv_style_set_bg_opa(&cpicker_bg, LV_STATE_NORMAL, LV_OPA_COVER);
    lv_style_set_bg_color(&cpicker_bg, LV_STATE_NORMAL, color_scr(LV_STATE_NORMAL));
    lv_style_set_pad_inner(&cpicker_bg, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_radius(&cpicker_bg, LV_STATE_NORMAL, LV_RADIUS_CIRCLE);

    lv_style_init(&cpicker_indic);
    lv_style_set_radius(&cpicker_indic, LV_STATE_NORMAL, LV_RADIUS_CIRCLE);
    lv_style_set_bg_color(&cpicker_indic, LV_STATE_NORMAL, LV_COLOR_WHITE);
    lv_style_set_bg_opa(&cpicker_indic, LV_STATE_NORMAL, LV_OPA_COVER);
    lv_style_set_border_width(&cpicker_indic, LV_STATE_NORMAL, 2);
    lv_style_set_border_color(&cpicker_indic, LV_STATE_NORMAL, LV_COLOR_GRAY);
    lv_style_set_pad_left(&cpicker_indic, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_pad_right(&cpicker_indic, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_pad_top(&cpicker_indic, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_pad_bottom(&cpicker_indic, LV_STATE_NORMAL, LV_DPI / 10);
#endif
}

static void checkbox_init(void)
{
#if LV_USE_CHECKBOX != 0
    lv_style_init(&cb_bg);
    lv_style_set_radius(&cb_bg, LV_STATE_NORMAL, LV_DPI / 50);
    lv_style_set_pad_inner(&cb_bg, LV_STATE_NORMAL , LV_DPI / 20);
    lv_style_set_outline_color(&cb_bg, LV_STATE_NORMAL, _color_primary);
    lv_style_set_outline_opa(&cb_bg, LV_STATE_NORMAL, LV_OPA_TRANSP);
    lv_style_set_outline_opa(&cb_bg, LV_STATE_FOCUSED, LV_OPA_50);
    lv_style_set_outline_width(&cb_bg, LV_STATE_NORMAL, 3);
    lv_style_set_outline_pad(&cb_bg, LV_STATE_NORMAL, LV_DPI/20);
    lv_style_set_trans_time(&cb_bg, LV_STATE_NORMAL, TRANSITION_TIME);
    lv_style_set_trans_prop_6(&cb_bg, LV_STATE_NORMAL, LV_STYLE_OUTLINE_OPA);

    lv_style_init(&cb_bullet);
    lv_style_set_radius(&cb_bullet, LV_STATE_NORMAL, LV_DPI / 50);
    lv_style_set_bg_color(&cb_bullet, LV_STATE_NORMAL, _color_primary);
    lv_style_set_bg_opa(&cb_bullet, LV_STATE_CHECKED, LV_OPA_COVER);
    lv_style_set_border_width(&cb_bullet, LV_STATE_NORMAL, BORDER_WIDTH);
    lv_style_set_border_color(&cb_bullet, LV_STATE_NORMAL, color_bg_border(LV_STATE_NORMAL));
    lv_style_set_border_color(&cb_bullet, LV_STATE_PRESSED, color_bg_border(LV_STATE_PRESSED));
    lv_style_set_border_opa(&cb_bullet, LV_STATE_CHECKED, LV_OPA_80);
    lv_style_set_pattern_image(&cb_bullet, LV_STATE_CHECKED, LV_SYMBOL_OK);
    lv_style_set_pattern_recolor(&cb_bullet, LV_STATE_CHECKED, LV_COLOR_WHITE);
    lv_style_set_text_font(&cb_bullet, LV_STATE_CHECKED, _font_small);
    lv_style_set_trans_time(&cb_bullet, LV_STATE_NORMAL , TRANSITION_TIME);
    lv_style_set_trans_prop_5(&cb_bullet, LV_STATE_NORMAL, LV_STYLE_BG_OPA);
    lv_style_set_trans_prop_6(&cb_bullet, LV_STATE_NORMAL, LV_STYLE_BORDER_COLOR);
#endif
}

static void btnmatrix_init(void)
{
#if LV_USE_BTNMATRIX
    lv_style_init(&btnm_bg);
    lv_style_set_bg_opa(&btnm_bg, LV_STATE_NORMAL, LV_OPA_COVER);
    lv_style_set_border_width(&btnm_bg, LV_STATE_NORMAL, LV_DPI / 50);
    lv_style_set_border_color(&btnm_bg, LV_STATE_NORMAL, lv_color_hex(0x373a3f));
    lv_style_set_radius(&btnm_bg, LV_STATE_NORMAL, LV_DPI / 20);
    lv_style_set_clip_corner(&btnm_bg, LV_STATE_NORMAL, true);
    lv_style_set_border_post(&btnm_bg, LV_STATE_NORMAL, true);
    lv_style_set_bg_color(&btnm_bg, LV_STATE_NORMAL, color_bg(LV_STATE_NORMAL));

    lv_style_init(&btnm_btn);
    lv_style_set_radius(&btnm_btn, LV_STATE_NORMAL, 0);

#endif
}

static void keyboard_init(void)
{
#if LV_USE_KEYBOARD
    lv_style_init(&kb_btn);
    lv_style_set_radius(&kb_btn, LV_STATE_NORMAL, LV_DPI / 50);
    lv_style_set_border_width(&kb_btn, LV_STATE_NORMAL, 1);

    lv_style_init(&kb_bg);
    lv_style_copy(&kb_bg, &panel);
    lv_style_set_pad_inner(&kb_bg, LV_STATE_NORMAL, LV_DPI/20);
    lv_style_set_pad_left(&kb_bg, LV_STATE_NORMAL, LV_DPI/20);
    lv_style_set_pad_right(&kb_bg, LV_STATE_NORMAL, LV_DPI/20);
    lv_style_set_pad_top(&kb_bg, LV_STATE_NORMAL, LV_DPI/20);
    lv_style_set_pad_bottom(&kb_bg, LV_STATE_NORMAL, LV_DPI/20);

#endif
}

static void msgbox_init(void)
{
#if LV_USE_MSGBOX
    lv_style_init(&mbox_btn_bg);
    lv_style_set_pad_inner(&mbox_btn_bg, LV_STATE_NORMAL,  LV_DPI / 10);
#endif
}

static void page_init(void)
{
#if LV_USE_PAGE
    lv_style_init(&sb);
    lv_style_set_bg_opa(&sb, LV_STATE_NORMAL, LV_OPA_COVER);
    lv_style_set_bg_color(&sb, LV_STATE_NORMAL, lv_color_hex(0x3a3d42));
    lv_style_set_radius(&sb, LV_STATE_NORMAL, LV_RADIUS_CIRCLE);
    lv_style_set_size(&sb,LV_STATE_NORMAL, LV_DPI / 30);
    lv_style_set_pad_right(&sb,LV_STATE_NORMAL,  LV_DPI / 20);
    lv_style_set_pad_bottom(&sb,LV_STATE_NORMAL,  LV_DPI / 20);

#endif
}

static void textarea_init(void)
{
#if LV_USE_TEXTAREA
    lv_style_init(&ta_bg);
    lv_style_copy(&ta_bg, &panel);
    lv_style_set_border_side(&ta_bg, LV_STATE_NORMAL, LV_BORDER_SIDE_FULL);
    lv_style_set_pad_left(&ta_bg, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_pad_right(&ta_bg, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_pad_top(&ta_bg, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_pad_bottom(&ta_bg, LV_STATE_NORMAL, LV_DPI / 10);

    lv_style_init(&ta_cursor);
    lv_style_set_border_color(&ta_cursor, LV_STATE_NORMAL, lv_color_hex(0x6c737b));
    lv_style_set_border_width(&ta_cursor, LV_STATE_NORMAL, 2);
    lv_style_set_pad_left(&ta_cursor, LV_STATE_NORMAL, 1);
    lv_style_set_border_side(&ta_cursor, LV_STATE_NORMAL, LV_BORDER_SIDE_LEFT);

    lv_style_init(&ta_oneline);
    lv_style_set_border_width(&ta_oneline, LV_STATE_NORMAL, 1);
    lv_style_set_radius(&ta_oneline, LV_STATE_NORMAL, LV_RADIUS_CIRCLE);
    lv_style_set_border_color(&ta_oneline, LV_STATE_NORMAL, lv_color_hex(0x3b3e43));
    lv_style_set_border_color(&ta_oneline, LV_STATE_FOCUSED, _color_primary);
    lv_style_set_text_color(&ta_oneline, LV_STATE_NORMAL, lv_color_hex(0x6c737b));
    lv_style_set_pad_left(&ta_oneline, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_pad_right(&ta_oneline, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_pad_top(&ta_oneline, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_pad_bottom(&ta_oneline, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_trans_time(&ta_oneline, LV_STATE_NORMAL, TRANSITION_TIME);
    lv_style_set_trans_prop_6(&ta_oneline, LV_STATE_NORMAL, LV_STYLE_BORDER_COLOR);

    lv_style_init(&ta_placeholder);
    lv_style_set_text_color(&ta_placeholder, LV_STATE_NORMAL, lv_color_hex(0x3b3e43));
#endif
}

static void spinbox_init(void)
{
#if LV_USE_SPINBOX
    lv_style_init(&spinbox_btn);
    lv_style_copy(&spinbox_btn, &ta_bg);
    lv_style_set_bg_color(&spinbox_btn, LV_STATE_PRESSED, lv_color_hex3(0xaaa));

    lv_style_init(&spinbox_cursor);
    lv_style_set_bg_opa(&spinbox_cursor, LV_STATE_NORMAL, LV_OPA_COVER);
    lv_style_set_bg_color(&spinbox_cursor, LV_STATE_NORMAL, _color_primary);
    lv_style_set_text_color(&spinbox_cursor, LV_STATE_NORMAL, LV_COLOR_WHITE);
    lv_style_set_pad_top(&spinbox_cursor, LV_STATE_NORMAL, LV_DPI);
    lv_style_set_pad_bottom(&spinbox_cursor, LV_STATE_NORMAL, LV_DPI);

#endif
}

static void list_init(void)
{
#if LV_USE_LIST != 0
    lv_style_init(&list_bg);
    lv_style_copy(&list_bg, &panel);
    lv_style_set_pad_left(&list_bg, LV_STATE_NORMAL, 0);
    lv_style_set_pad_right(&list_bg, LV_STATE_NORMAL, 0);
    lv_style_set_pad_top(&list_bg, LV_STATE_NORMAL, 0);
    lv_style_set_pad_bottom(&list_bg, LV_STATE_NORMAL, 0);
    lv_style_set_pad_inner(&list_bg, LV_STATE_NORMAL, 0);

    lv_style_init(&list_btn);
    lv_style_set_radius(&list_btn, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_radius(&list_btn, LV_STATE_PRESSED, 0);
    lv_style_set_radius(&list_btn, LV_STATE_CHECKED, 0);
    lv_style_set_bg_opa(&list_btn, LV_STATE_PRESSED, LV_OPA_COVER);
    lv_style_set_bg_opa(&list_btn, LV_STATE_CHECKED , LV_OPA_COVER);
    lv_style_set_bg_color(&list_btn, LV_STATE_NORMAL, color_btn(LV_STATE_NORMAL));
    lv_style_set_bg_color(&list_btn, LV_STATE_PRESSED, color_btn(LV_STATE_PRESSED));
    lv_style_set_bg_color(&list_btn, LV_STATE_DISABLED, color_btn(LV_STATE_DISABLED));
    lv_style_set_bg_color(&list_btn, LV_STATE_CHECKED, color_btn(LV_STATE_CHECKED));
    lv_style_set_bg_color(&list_btn, LV_STATE_CHECKED | LV_STATE_PRESSED, color_btn(LV_STATE_CHECKED | LV_STATE_PRESSED));
    lv_style_set_text_color(&list_btn, LV_STATE_NORMAL, color_btn_txt(LV_STATE_NORMAL));
    lv_style_set_text_color(&list_btn, LV_STATE_PRESSED, color_btn_txt(LV_STATE_PRESSED));
    lv_style_set_text_color(&list_btn, LV_STATE_DISABLED, color_btn_txt(LV_STATE_DISABLED));
    lv_style_set_image_recolor(&list_btn, LV_STATE_NORMAL, color_btn_txt(LV_STATE_NORMAL));
    lv_style_set_image_recolor(&list_btn, LV_STATE_PRESSED, color_btn_txt(LV_STATE_PRESSED));
    lv_style_set_image_recolor(&list_btn, LV_STATE_DISABLED, color_btn_txt(LV_STATE_DISABLED));

    lv_style_set_border_width(&list_btn, LV_STATE_NORMAL, 1);
    lv_style_set_border_side(&list_btn, LV_STATE_NORMAL, LV_BORDER_SIDE_BOTTOM);
    lv_style_set_border_color(&list_btn, LV_STATE_NORMAL, lv_color_hex(0x979a9f));
    lv_style_set_border_color(&list_btn, LV_STATE_FOCUSED, _color_primary);
    lv_style_set_border_color(&list_btn, LV_STATE_FOCUSED | LV_STATE_CHECKED, lv_color_hex(0x979a9f));

    lv_style_set_pad_left(&list_btn, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_pad_right(&list_btn, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_pad_top(&list_btn, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_pad_bottom(&list_btn, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_pad_inner(&list_btn, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_trans_time(&list_btn, LV_STATE_NORMAL, TRANSITION_TIME);
    lv_style_set_trans_prop_4(&list_btn, LV_STATE_NORMAL, LV_STYLE_RADIUS);
    lv_style_set_trans_prop_5(&list_btn, LV_STATE_NORMAL, LV_STYLE_BG_OPA);
    lv_style_set_trans_prop_6(&list_btn, LV_STATE_NORMAL, LV_STYLE_BG_OPA);
#endif
}

static void ddlist_init(void)
{
#if LV_USE_DROPDOWN != 0

    lv_style_init(&ddlist_btn);
    lv_style_copy(&ddlist_btn, &panel);
    lv_style_set_border_side(&ddlist_btn, LV_STATE_NORMAL, LV_BORDER_SIDE_FULL);
    lv_style_set_bg_color(&ddlist_btn, LV_STATE_PRESSED, lv_color_hex3(0x666));
    lv_style_set_pad_top(&ddlist_btn, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_pad_bottom(&ddlist_btn, LV_STATE_NORMAL, LV_DPI / 10);

    lv_style_init(&ddlist_page);
    lv_style_copy(&ddlist_page, &panel);
    lv_style_set_border_side(&ddlist_page, LV_STATE_NORMAL, LV_BORDER_SIDE_FULL);
    lv_style_set_bg_color(&ddlist_page, LV_STATE_NORMAL, lv_color_hex(0xf0f0f0));
    lv_style_set_text_color(&ddlist_page, LV_STATE_NORMAL, lv_color_hex3(0x333));
    lv_style_set_text_line_space(&ddlist_page, LV_STATE_NORMAL, LV_DPI / 5);
    lv_style_set_border_post(&ddlist_page, LV_STATE_NORMAL, true);

    lv_style_init(&ddlist_sel);
    lv_style_set_bg_opa(&ddlist_sel, LV_STATE_NORMAL, LV_OPA_COVER);
    lv_style_set_bg_color(&ddlist_sel, LV_STATE_NORMAL, _color_primary);

    lv_style_set_bg_color(&ddlist_sel, LV_STATE_PRESSED, LV_COLOR_SILVER);

    lv_style_set_text_color(&ddlist_sel, LV_STATE_NORMAL, lv_color_hex3(0x333));
#endif
}

static void roller_init(void)
{
#if LV_USE_ROLLER != 0
    lv_style_init(&roller_bg);
    lv_style_copy(&roller_bg, &ta_bg);
    lv_style_set_text_line_space(&roller_bg, LV_STATE_NORMAL, LV_DPI / 8);

    lv_style_init(&roller_sel);
    lv_style_set_bg_opa(&roller_sel, LV_STATE_NORMAL, LV_OPA_COVER);
    lv_style_set_bg_color(&roller_sel, LV_STATE_NORMAL, _color_primary);
    lv_style_set_text_color(&roller_sel, LV_STATE_NORMAL, LV_COLOR_WHITE);
#endif
}

static void tabview_init(void)
{
#if LV_USE_TABVIEW != 0
    lv_style_init(&tabview_btns_bg);
    lv_style_set_bg_opa(&tabview_btns_bg, LV_STATE_NORMAL, LV_OPA_COVER);
    lv_style_set_bg_color(&tabview_btns_bg, LV_STATE_NORMAL, color_panel(LV_STATE_NORMAL));
    lv_style_set_border_color(&tabview_btns_bg, LV_STATE_NORMAL, color_panel_border(LV_STATE_NORMAL));
    lv_style_set_border_width(&tabview_btns_bg, LV_STATE_NORMAL, LV_DPI / 30 > 0 ? LV_DPI / 30 : 1);
    lv_style_set_border_side(&tabview_btns_bg, LV_STATE_NORMAL , LV_BORDER_SIDE_BOTTOM);
    lv_style_set_text_color(&tabview_btns_bg, LV_STATE_NORMAL, lv_color_hex(0x979a9f));
    lv_style_set_text_font(&tabview_btns_bg, LV_STATE_NORMAL, _font_normal);
    lv_style_set_image_recolor(&tabview_btns_bg, LV_STATE_NORMAL, lv_color_hex(0x979a9f));


    lv_style_init(&tabview_btns);
    lv_style_set_bg_opa(&tabview_btns, LV_STATE_PRESSED, LV_OPA_50);
    lv_style_set_bg_color(&tabview_btns, LV_STATE_PRESSED, lv_color_hex3(0x888));
    lv_style_set_text_color(&tabview_btns, LV_STATE_CHECKED, color_panel_txt(LV_STATE_CHECKED));
    lv_style_set_pad_top(&tabview_btns, LV_STATE_NORMAL, LV_DPI / 5);
    lv_style_set_pad_bottom(&tabview_btns, LV_STATE_NORMAL, LV_DPI / 5);

    lv_style_init(&tabview_indic);
    lv_style_set_bg_opa(&tabview_indic, LV_STATE_NORMAL, LV_OPA_COVER);
    lv_style_set_bg_color(&tabview_indic, LV_STATE_NORMAL, _color_primary);
    lv_style_set_size(&tabview_indic, LV_STATE_NORMAL, LV_DPI / 20 > 0 ? LV_DPI / 20 : 1);
    lv_style_set_radius(&tabview_indic, LV_STATE_NORMAL, LV_RADIUS_CIRCLE);


    lv_style_init(&tabview_page_scrl);
    lv_style_set_pad_top(&tabview_page_scrl, LV_STATE_NORMAL, LV_DPI / 5);
    lv_style_set_pad_bottom(&tabview_page_scrl, LV_STATE_NORMAL, LV_DPI / 5);
    lv_style_set_pad_left(&tabview_page_scrl, LV_STATE_NORMAL, LV_DPI / 3);
    lv_style_set_pad_right(&tabview_page_scrl, LV_STATE_NORMAL, LV_DPI / 3);
    lv_style_set_pad_inner(&tabview_page_scrl, LV_STATE_NORMAL, LV_DPI / 5);
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
    lv_style_init(&table_cell);
    lv_style_set_border_color(&table_cell, LV_STATE_NORMAL, lv_color_hex(0x303338));
    lv_style_set_border_width(&table_cell, LV_STATE_NORMAL, 1);
    lv_style_set_border_side(&table_cell, LV_STATE_NORMAL, LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_BOTTOM);
    lv_style_set_pad_left(&table_cell, LV_STATE_NORMAL, LV_DPI/7);
    lv_style_set_pad_right(&table_cell, LV_STATE_NORMAL, LV_DPI/7);
    lv_style_set_pad_top(&table_cell, LV_STATE_NORMAL, LV_DPI/7);
    lv_style_set_pad_bottom(&table_cell, LV_STATE_NORMAL, LV_DPI/7);

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
                                   lv_font_t * font_small, lv_font_t * font_normal, lv_font_t * font_subtitle, lv_font_t * font_title)
{

    _color_primary = color_primary;
    _color_secondary = color_secondary;
    _font_small = font_small;
    _font_normal = font_normal;
    _font_subtitle = font_subtitle;
    _font_title = font_title;

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

    theme.apply_cb = lv_theme_material_apply;

    return &theme;
}


void lv_theme_material_apply(lv_obj_t * obj, lv_theme_style_t name)
{
    lv_style_list_t * list;

    switch(name) {
    case LV_THEME_NONE:
        break;

    case LV_THEME_SCR:
        lv_obj_clean_style_list(obj, LV_OBJ_PART_MAIN);
        list = lv_obj_get_style_list(obj, LV_OBJ_PART_MAIN);
lv_style_list_add_style(list, &scr);
        break;
    case LV_THEME_OBJ:
        lv_obj_clean_style_list(obj, LV_OBJ_PART_MAIN);
        list = lv_obj_get_style_list(obj, LV_OBJ_PART_MAIN);
lv_style_list_add_style(list, &panel);
        break;
#if LV_USE_CONT
    case LV_THEME_CONT:
        lv_obj_clean_style_list(obj, LV_OBJ_PART_MAIN);
        list = lv_obj_get_style_list(obj, LV_CONT_PART_MAIN);
lv_style_list_add_style(list, &panel);
        break;
#endif

#if LV_USE_BTN
    case LV_THEME_BTN:
        lv_obj_clean_style_list(obj, LV_BTN_PART_MAIN);
        list = lv_obj_get_style_list(obj, LV_BTN_PART_MAIN);
lv_style_list_add_style(list, &btn);
        break;
#endif

#if LV_USE_BTNMATRIX
    case LV_THEME_BTNMATRIX:
        lv_obj_clean_style_list(obj, LV_BTNMATRIX_PART_BG);
        list = lv_obj_get_style_list(obj, LV_BTNMATRIX_PART_BG);

        lv_obj_clean_style_list(obj, LV_BTNMATRIX_PART_BTN);
        list = lv_obj_get_style_list(obj, LV_BTNMATRIX_PART_BTN);
lv_style_list_add_style(list, &btn);
lv_style_list_add_style(list, &btnm_btn);
        break;
#endif

#if LV_USE_KEYBOARD
    case LV_THEME_KEYBOARD:
        lv_obj_clean_style_list(obj, LV_KEYBOARD_PART_BG);
        list = lv_obj_get_style_list(obj, LV_KEYBOARD_PART_BG);
lv_style_list_add_style(list, &kb_bg);

        lv_obj_clean_style_list(obj, LV_KEYBOARD_PART_BTN);
        list = lv_obj_get_style_list(obj, LV_KEYBOARD_PART_BTN);
lv_style_list_add_style(list, &btn);
lv_style_list_add_style(list, &kb_btn);
        break;
#endif

#if LV_USE_BAR
    case LV_THEME_BAR:
        lv_obj_clean_style_list(obj, LV_BAR_PART_BG);
        list = lv_obj_get_style_list(obj, LV_BAR_PART_BG);

lv_style_list_add_style(list, &bar_bg);

        lv_obj_clean_style_list(obj, LV_BAR_PART_INDIC);
        list = lv_obj_get_style_list(obj, LV_BAR_PART_INDIC);
        lv_style_list_add_style(list, &bar_indic);
        break;
#endif

#if LV_USE_SWITCH
    case LV_THEME_SWITCH:
        lv_obj_clean_style_list(obj, LV_SWITCH_PART_BG);
        list = lv_obj_get_style_list(obj, LV_SWITCH_PART_BG);
        lv_style_list_add_style(list, &bar_bg);

        lv_obj_clean_style_list(obj, LV_SWITCH_PART_INDIC);
        list = lv_obj_get_style_list(obj, LV_SWITCH_PART_INDIC);
        lv_style_list_add_style(list, &bar_indic);

        lv_obj_clean_style_list(obj, LV_SWITCH_PART_KNOB);
        list = lv_obj_get_style_list(obj, LV_SWITCH_PART_KNOB);
        lv_style_list_add_style(list, &sw_knob);
        break;
#endif

#if LV_USE_CANVAS
    case LV_THEME_CANVAS:
        lv_obj_clean_style_list(obj, LV_CANVAS_PART_MAIN);
        list = lv_obj_get_style_list(obj, LV_CANVAS_PART_MAIN);
        break;
#endif

#if LV_USE_IMG
    case LV_THEME_IMAGE:
        lv_obj_clean_style_list(obj, LV_IMG_PART_MAIN);
        list = lv_obj_get_style_list(obj, LV_IMG_PART_MAIN);
        break;
#endif

#if LV_USE_IMGBTN
    case LV_THEME_IMGBTN:
        lv_obj_clean_style_list(obj, LV_IMG_PART_MAIN);
        list = lv_obj_get_style_list(obj, LV_IMG_PART_MAIN);
        break;
#endif

#if LV_USE_LABEL
    case LV_THEME_LABEL:
        lv_obj_clean_style_list(obj, LV_LABEL_PART_MAIN);
        list = lv_obj_get_style_list(obj, LV_LABEL_PART_MAIN);
        break;
#endif

#if LV_USE_LINE
    case LV_THEME_LINE:
        lv_obj_clean_style_list(obj, LV_LABEL_PART_MAIN);
        list = lv_obj_get_style_list(obj, LV_LABEL_PART_MAIN);
        break;
#endif

#if LV_USE_ARC
    case LV_THEME_ARC:
        lv_obj_clean_style_list(obj, LV_ARC_PART_BG);
        list = lv_obj_get_style_list(obj, LV_ARC_PART_BG);
        lv_style_list_add_style(list, &arc_bg);

        lv_obj_clean_style_list(obj, LV_ARC_PART_ARC);
        list = lv_obj_get_style_list(obj, LV_ARC_PART_ARC);
        lv_style_list_add_style(list, &arc);
        break;
#endif

#if LV_USE_SPINNER
    case LV_THEME_SPINNER:
        lv_obj_clean_style_list(obj, LV_SPINNER_PART_BG);
        list = lv_obj_get_style_list(obj, LV_SPINNER_PART_BG);
        lv_style_list_add_style(list, &arc_bg);

        lv_obj_clean_style_list(obj, LV_SPINNER_PART_ARC);
        list = lv_obj_get_style_list(obj, LV_SPINNER_PART_ARC);
        lv_style_list_add_style(list, &arc);
        break;
#endif

#if LV_USE_SLIDER
    case LV_THEME_SLIDER:
        lv_obj_clean_style_list(obj, LV_SLIDER_PART_BG);
        list = lv_obj_get_style_list(obj, LV_SLIDER_PART_BG);
        lv_style_list_add_style(list, &bar_bg);

        lv_obj_clean_style_list(obj, LV_SLIDER_PART_INDIC);
        list = lv_obj_get_style_list(obj, LV_SLIDER_PART_INDIC);
        lv_style_list_add_style(list, &bar_indic);

        lv_obj_clean_style_list(obj, LV_SLIDER_PART_KNOB);
        list = lv_obj_get_style_list(obj, LV_SLIDER_PART_KNOB);
        lv_style_list_add_style(list, &slider_knob);
        break;
#endif

#if LV_USE_CHECKBOX
    case LV_THEME_CHECKBOX:
        lv_obj_clean_style_list(obj, LV_CHECKBOX_PART_BG);
        list = lv_obj_get_style_list(obj, LV_CHECKBOX_PART_BG);
        lv_style_list_add_style(list, &cb_bg);

        lv_obj_clean_style_list(obj, LV_CHECKBOX_PART_BULLET);
        list = lv_obj_get_style_list(obj, LV_CHECKBOX_PART_BULLET);
        lv_style_list_add_style(list, &cb_bullet);
        break;
#endif

#if LV_USE_MSGBOX
    case LV_THEME_MSGBOX:
        lv_obj_clean_style_list(obj, LV_MSGBOX_PART_BG);
        list = lv_obj_get_style_list(obj, LV_MSGBOX_PART_BG);
        lv_style_list_add_style(list, &bg);
        break;

    case LV_THEME_MSGBOX_BTNS:
        lv_obj_clean_style_list(obj, LV_MSGBOX_PART_BTN_BG);
        list = lv_obj_get_style_list(obj, LV_MSGBOX_PART_BTN_BG);
        lv_style_list_add_style(list, &mbox_btn_bg);

        lv_obj_clean_style_list(obj, LV_MSGBOX_PART_BTN);
        list = lv_obj_get_style_list(obj, LV_MSGBOX_PART_BTN);
        lv_style_list_add_style(list, &btn);
        break;

#endif
#if LV_USE_LED
    case LV_THEME_LED:
        lv_obj_clean_style_list(obj, LV_LED_PART_MAIN);
        list = lv_obj_get_style_list(obj, LV_LED_PART_MAIN);
        lv_style_list_add_style(list, &led);
        break;
#endif
#if LV_USE_PAGE
    case LV_THEME_PAGE:
        lv_obj_clean_style_list(obj, LV_PAGE_PART_BG);
        list = lv_obj_get_style_list(obj, LV_PAGE_PART_BG);
        lv_style_list_add_style(list, &panel);

        lv_obj_clean_style_list(obj, LV_PAGE_PART_SCRL);
        list = lv_obj_get_style_list(obj, LV_PAGE_PART_SCRL);

        lv_obj_clean_style_list(obj, LV_PAGE_PART_SCRLBAR);
        list = lv_obj_get_style_list(obj, LV_PAGE_PART_SCRLBAR);
        lv_style_list_add_style(list, &sb);
        break;
#endif
#if LV_USE_TABVIEW
    case LV_THEME_TABVIEW:
        lv_obj_clean_style_list(obj, LV_TABVIEW_PART_BG);
        list = lv_obj_get_style_list(obj, LV_TABVIEW_PART_BG);
        lv_style_list_add_style(list, &scr);

        lv_obj_clean_style_list(obj, LV_TABVIEW_PART_BG_SCRL);
        list = lv_obj_get_style_list(obj, LV_TABVIEW_PART_BG_SCRL);

        lv_obj_clean_style_list(obj, LV_TABVIEW_PART_TAB_BG);
        list = lv_obj_get_style_list(obj, LV_TABVIEW_PART_TAB_BG);
        lv_style_list_add_style(list, &tabview_btns_bg);

        lv_obj_clean_style_list(obj, LV_TABVIEW_PART_INDIC);
        list = lv_obj_get_style_list(obj, LV_TABVIEW_PART_INDIC);
        lv_style_list_add_style(list, &tabview_indic);

        lv_obj_clean_style_list(obj, LV_TABVIEW_PART_TAB);
        list = lv_obj_get_style_list(obj, LV_TABVIEW_PART_TAB);
        lv_style_list_add_style(list, &tabview_btns);
        break;

    case LV_THEME_TABVIEW_PAGE:
        lv_obj_clean_style_list(obj, LV_PAGE_PART_BG);
        list = lv_obj_get_style_list(obj, LV_PAGE_PART_BG);

        lv_obj_clean_style_list(obj, LV_PAGE_PART_SCRL);
        list = lv_obj_get_style_list(obj, LV_PAGE_PART_SCRL);
        lv_style_list_add_style(list, &tabview_page_scrl);

        break;
#endif

#if LV_USE_TILEVIEW
    case LV_THEME_TILEVIEW:
        lv_obj_clean_style_list(obj, LV_TILEVIEW_PART_BG);
        list = lv_obj_get_style_list(obj, LV_TILEVIEW_PART_BG);
        lv_style_list_add_style(list, &scr);

        lv_obj_clean_style_list(obj, LV_TILEVIEW_PART_SCRLBAR);
        list = lv_obj_get_style_list(obj, LV_TILEVIEW_PART_SCRLBAR);
        lv_style_list_add_style(list, &sb);

        lv_obj_clean_style_list(obj, LV_TILEVIEW_PART_EDGE_FLASH);
        list = lv_obj_get_style_list(obj, LV_TILEVIEW_PART_EDGE_FLASH);
        lv_style_list_add_style(list, &btn);
        break;
#endif


#if LV_USE_ROLLER
    case LV_THEME_ROLLER:
        lv_obj_clean_style_list(obj, LV_ROLLER_PART_BG);
        list = lv_obj_get_style_list(obj, LV_ROLLER_PART_BG);
        lv_style_list_add_style(list, &roller_bg);

        lv_obj_clean_style_list(obj, LV_ROLLER_PART_SEL);
        list = lv_obj_get_style_list(obj, LV_ROLLER_PART_SEL);
        lv_style_list_add_style(list, &roller_sel);
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
        lv_style_list_add_style(list, &list_bg);

        lv_obj_clean_style_list(obj, LV_LIST_PART_SCRL);
        list = lv_obj_get_style_list(obj, LV_LIST_PART_SCRL);

        lv_obj_clean_style_list(obj, LV_LIST_PART_SCRLBAR);
        list = lv_obj_get_style_list(obj, LV_LIST_PART_SCRLBAR);
        lv_style_list_add_style(list, &sb);
        break;

    case LV_THEME_LIST_BTN:
        lv_obj_clean_style_list(obj, LV_BTN_PART_MAIN);
        list = lv_obj_get_style_list(obj, LV_BTN_PART_MAIN);
        lv_style_list_add_style(list, &list_btn);
        break;
#endif

#if LV_USE_DROPDOWN
    case LV_THEME_DROPDOWN:
        lv_obj_clean_style_list(obj, LV_DROPDOWN_PART_BTN);
        list = lv_obj_get_style_list(obj, LV_DROPDOWN_PART_BTN);
        lv_style_list_add_style(list, &ddlist_btn);

        lv_obj_clean_style_list(obj, LV_DROPDOWN_PART_LIST);
        list = lv_obj_get_style_list(obj, LV_DROPDOWN_PART_LIST);
        lv_style_list_add_style(list, &ddlist_page);

        lv_obj_clean_style_list(obj, LV_DROPDOWN_PART_SCRLBAR);
        list = lv_obj_get_style_list(obj, LV_DROPDOWN_PART_SCRLBAR);
        lv_style_list_add_style(list, &sb);

        lv_obj_clean_style_list(obj, LV_DROPDOWN_PART_SELECTED);
        list = lv_obj_get_style_list(obj, LV_DROPDOWN_PART_SELECTED);
        lv_style_list_add_style(list, &ddlist_sel);
        break;
#endif

#if LV_USE_CHART
    case LV_THEME_CHART:
        lv_obj_clean_style_list(obj, LV_CHART_PART_BG);
        list = lv_obj_get_style_list(obj, LV_CHART_PART_BG);
        lv_style_list_add_style(list, &panel);

        lv_obj_clean_style_list(obj, LV_CHART_PART_SERIES_BG);
        list = lv_obj_get_style_list(obj, LV_CHART_PART_SERIES_BG);
        lv_style_list_add_style(list, &chart_series_bg);

        lv_obj_clean_style_list(obj, LV_CHART_PART_SERIES);
        list = lv_obj_get_style_list(obj, LV_CHART_PART_SERIES);
        lv_style_list_add_style(list, &chart_series);
        break;
#endif
#if LV_USE_TABLE
    case LV_THEME_TABLE:
        lv_obj_clean_style_list(obj, LV_TABLE_PART_BG);
        list = lv_obj_get_style_list(obj, LV_TABLE_PART_BG);

        lv_obj_clean_style_list(obj, LV_TABLE_PART_CELL1);
        list = lv_obj_get_style_list(obj, LV_TABLE_PART_CELL1);
        lv_style_list_add_style(list, &table_cell);

        lv_obj_clean_style_list(obj, LV_TABLE_PART_CELL2);
        list = lv_obj_get_style_list(obj, LV_TABLE_PART_CELL2);
        lv_style_list_add_style(list, &table_cell);

        lv_obj_clean_style_list(obj, LV_TABLE_PART_CELL3);
        list = lv_obj_get_style_list(obj, LV_TABLE_PART_CELL3);
        lv_style_list_add_style(list, &table_cell);

        lv_obj_clean_style_list(obj, LV_TABLE_PART_CELL4);
        list = lv_obj_get_style_list(obj, LV_TABLE_PART_CELL4);
        lv_style_list_add_style(list, &table_cell);
        break;
#endif

#if LV_USE_WIN
    case LV_THEME_WIN:
        lv_obj_clean_style_list(obj, LV_WIN_PART_BG);
        list = lv_obj_get_style_list(obj, LV_WIN_PART_BG);
        lv_style_list_add_style(list, &scr);

        lv_obj_clean_style_list(obj, LV_WIN_PART_SCRLBAR);
        list = lv_obj_get_style_list(obj, LV_WIN_PART_SCRLBAR);
        lv_style_list_add_style(list, &sb);

        lv_obj_clean_style_list(obj, LV_WIN_PART_CONTENT_SCRL);
        list = lv_obj_get_style_list(obj, LV_WIN_PART_CONTENT_SCRL);
        lv_style_list_add_style(list, &tabview_page_scrl);

        lv_obj_clean_style_list(obj, LV_WIN_PART_HEADER);
        list = lv_obj_get_style_list(obj, LV_WIN_PART_HEADER);
        lv_style_list_add_style(list, &tabview_btns_bg);
        break;

    case LV_THEME_WIN_BTN:
        lv_obj_clean_style_list(obj, LV_BTN_PART_MAIN);
        list = lv_obj_get_style_list(obj, LV_BTN_PART_MAIN);
        lv_style_list_add_style(list, &tabview_btns);
        break;
#endif

#if LV_USE_TEXTAREA
    case LV_THEME_TEXTAREA:
        lv_obj_clean_style_list(obj, LV_TEXTAREA_PART_BG);
        list = lv_obj_get_style_list(obj, LV_TEXTAREA_PART_BG);
        lv_style_list_add_style(list, &ta_bg);

        lv_obj_clean_style_list(obj, LV_TEXTAREA_PART_PLACEHOLDER);
        list = lv_obj_get_style_list(obj, LV_TEXTAREA_PART_PLACEHOLDER);
        lv_style_list_add_style(list, &ta_placeholder);

        lv_obj_clean_style_list(obj, LV_TEXTAREA_PART_CURSOR);
        list = lv_obj_get_style_list(obj, LV_TEXTAREA_PART_CURSOR);
        lv_style_list_add_style(list, &ta_cursor);

        lv_obj_clean_style_list(obj, LV_TEXTAREA_PART_SCRLBAR);
        list = lv_obj_get_style_list(obj, LV_TEXTAREA_PART_SCRLBAR);
        lv_style_list_add_style(list, &sb);
        break;

    case LV_THEME_TEXTAREA_ONELINE:
        lv_obj_clean_style_list(obj, LV_TEXTAREA_PART_BG);
        list = lv_obj_get_style_list(obj, LV_TEXTAREA_PART_BG);
        lv_style_list_add_style(list, &ta_oneline);

        lv_obj_clean_style_list(obj, LV_TEXTAREA_PART_PLACEHOLDER);
        list = lv_obj_get_style_list(obj, LV_TEXTAREA_PART_PLACEHOLDER);
        lv_style_list_add_style(list, &ta_placeholder);

        lv_obj_clean_style_list(obj, LV_TEXTAREA_PART_CURSOR);
        list = lv_obj_get_style_list(obj, LV_TEXTAREA_PART_CURSOR);
        lv_style_list_add_style(list, &ta_cursor);

        lv_obj_clean_style_list(obj, LV_TEXTAREA_PART_SCRLBAR);
        list = lv_obj_get_style_list(obj, LV_TEXTAREA_PART_SCRLBAR);
        lv_style_list_add_style(list, &sb);
        break;
#endif


#if LV_USE_SPINBOX
    case LV_THEME_SPINBOX:
        lv_obj_clean_style_list(obj, LV_SPINBOX_PART_BG);
        list = lv_obj_get_style_list(obj, LV_SPINBOX_PART_BG);
        lv_style_list_add_style(list, &ta_bg);

        lv_obj_clean_style_list(obj, LV_SPINBOX_PART_CURSOR);
        list = lv_obj_get_style_list(obj, LV_SPINBOX_PART_CURSOR);
        lv_style_list_add_style(list, &spinbox_cursor);
        break;

    case LV_THEME_SPINBOX_BTN:
        lv_obj_clean_style_list(obj, LV_BTN_PART_MAIN);
        list = lv_obj_get_style_list(obj, LV_BTN_PART_MAIN);
        lv_style_list_add_style(list, &spinbox_btn);
        break;
#endif

#if LV_USE_CALENDAR
    case LV_THEME_CALENDAR:
        lv_obj_clean_style_list(obj, LV_CALENDAR_PART_BG);
        list = lv_obj_get_style_list(obj, LV_CALENDAR_PART_BG);
        lv_style_list_add_style(list, &panel);

        lv_obj_clean_style_list(obj, LV_CALENDAR_PART_DATE);
        list = lv_obj_get_style_list(obj, LV_CALENDAR_PART_DATE);
        lv_style_list_add_style(list, &calendar_date_nums);

        lv_obj_clean_style_list(obj, LV_CALENDAR_PART_HEADER);
        list = lv_obj_get_style_list(obj, LV_CALENDAR_PART_HEADER);
        lv_style_list_add_style(list, &calendar_header);

        lv_obj_clean_style_list(obj, LV_CALENDAR_PART_DAY_NAMES);
        list = lv_obj_get_style_list(obj, LV_CALENDAR_PART_DAY_NAMES);
        lv_style_list_add_style(list, &calendar_daynames);
        break;
#endif
#if LV_USE_CPICKER
    case LV_THEME_CPICKER:
        lv_obj_clean_style_list(obj, LV_CPICKER_PART_MAIN);
        list = lv_obj_get_style_list(obj, LV_CPICKER_PART_MAIN);
        lv_style_list_add_style(list, &cpicker_bg);

        lv_obj_clean_style_list(obj, LV_CPICKER_PART_INDIC);
        list = lv_obj_get_style_list(obj, LV_CPICKER_PART_INDIC);
        lv_style_list_add_style(list, &cpicker_indic);
        break;
#endif

#if LV_USE_LINEMETER
    case LV_THEME_LINEMETER:
        lv_obj_clean_style_list(obj, LV_LINEMETER_PART_MAIN);
        list = lv_obj_get_style_list(obj, LV_LINEMETER_PART_MAIN);
        lv_style_list_add_style(list, &lmeter);
        break;
#endif
#if LV_USE_GAUGE
    case LV_THEME_GAUGE:
        lv_obj_clean_style_list(obj, LV_GAUGE_PART_MAIN);
        list = lv_obj_get_style_list(obj, LV_GAUGE_PART_MAIN);
        lv_style_list_add_style(list, &gauge_main);

        lv_obj_clean_style_list(obj, LV_GAUGE_PART_STRONG);
        list = lv_obj_get_style_list(obj, LV_GAUGE_PART_STRONG);
        lv_style_list_add_style(list, &gauge_strong);

        lv_obj_clean_style_list(obj, LV_GAUGE_PART_NEEDLE);
        list = lv_obj_get_style_list(obj, LV_GAUGE_PART_NEEDLE);
        lv_style_list_add_style(list, &gauge_needle);
        break;
#endif
        default:
            break;
    }


    lv_obj_refresh_style(obj);


}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static inline lv_color_t color_scr(lv_state_t state)
{
    switch(state) {
    case LV_STATE_NORMAL:
    case LV_STATE_PRESSED:
    case LV_STATE_CHECKED:
    case LV_STATE_FOCUSED:
    case LV_STATE_EDITED:
    case LV_STATE_HOVERED:
    case LV_STATE_DISABLED:
    default:
        return lv_color_hex(0x22252a);
    }
}

static inline lv_color_t color_panel(lv_state_t state)
{
    switch(state) {
    case LV_STATE_NORMAL:
    case LV_STATE_PRESSED:
    case LV_STATE_CHECKED:
    case LV_STATE_FOCUSED:
    case LV_STATE_EDITED:
    case LV_STATE_HOVERED:
    case LV_STATE_DISABLED:
    default:
        return lv_color_hex(0x282b30);
    }
}

static inline lv_color_t color_bg(lv_state_t state)
{
    switch(state) {
    case LV_STATE_NORMAL:
    case LV_STATE_PRESSED:
    case LV_STATE_CHECKED:
    case LV_STATE_FOCUSED:
    case LV_STATE_EDITED:
    case LV_STATE_HOVERED:
    case LV_STATE_DISABLED:
    default:
        return lv_color_hex(0x2f3237);
    }
}

static inline lv_color_t color_btn(lv_state_t state)
{
    switch(state) {
    case LV_STATE_NORMAL:
        return lv_color_hex(0x303439);
    case LV_STATE_PRESSED:
        return lv_color_hex(0x43484f);
    case LV_STATE_CHECKED:
        return _color_primary;
    case LV_STATE_CHECKED | LV_STATE_PRESSED:
        return lv_color_lighten(_color_primary, LV_OPA_20);
    case LV_STATE_FOCUSED:
    case LV_STATE_EDITED:
    case LV_STATE_HOVERED:
    case LV_STATE_DISABLED:
    default:
        return _color_primary;
    }
}

static inline lv_color_t color_scr_border(lv_state_t state)
{
    switch(state) {
    case LV_STATE_NORMAL:
    case LV_STATE_PRESSED:
    case LV_STATE_CHECKED:
    case LV_STATE_FOCUSED:
    case LV_STATE_EDITED:
    case LV_STATE_HOVERED:
    case LV_STATE_DISABLED:
    default:
        return lv_color_hex(0x555555);
    }
}

static inline lv_color_t color_panel_border(lv_state_t state)
{
    switch(state) {
    case LV_STATE_NORMAL:
    case LV_STATE_PRESSED:
    case LV_STATE_CHECKED:
    case LV_STATE_EDITED:
    case LV_STATE_HOVERED:
    case LV_STATE_DISABLED:
    default:
        return lv_color_hex(0x3b3e42);
    case LV_STATE_FOCUSED:
        return _color_primary;
    }
}


static inline lv_color_t color_bg_border(lv_state_t state)
{
    switch(state) {
    case LV_STATE_NORMAL:
        return lv_color_hex(0x606060);
    case LV_STATE_PRESSED:
        return lv_color_hex(0x404040);
    case LV_STATE_CHECKED:
    case LV_STATE_FOCUSED:
    case LV_STATE_EDITED:
    case LV_STATE_HOVERED:
    case LV_STATE_DISABLED:
    default:
        return lv_color_hex(0x606060);
    }
}

static inline lv_color_t color_btn_border(lv_state_t state)
{
    switch(state) {
    case LV_STATE_NORMAL:
    case LV_STATE_PRESSED:
        return lv_color_hex(0x606060);
    case LV_STATE_CHECKED:
    case LV_STATE_FOCUSED:
    case LV_STATE_EDITED:
    case LV_STATE_HOVERED:
    case LV_STATE_DISABLED:
    default:
        return LV_COLOR_RED;
    }
}


static inline lv_color_t color_scr_txt(lv_state_t state)
{
    switch(state) {
    case LV_STATE_NORMAL:
    case LV_STATE_PRESSED:
    case LV_STATE_CHECKED:
    case LV_STATE_FOCUSED:
    case LV_STATE_EDITED:
    case LV_STATE_HOVERED:
    case LV_STATE_DISABLED:
    default:
        return lv_color_hex(0xbababa);
    }
}

static inline lv_color_t color_panel_txt(lv_state_t state)
{
    switch(state) {
    case LV_STATE_NORMAL:
    case LV_STATE_PRESSED:
    case LV_STATE_CHECKED:
    case LV_STATE_FOCUSED:
    case LV_STATE_EDITED:
    case LV_STATE_HOVERED:
    case LV_STATE_DISABLED:
    default:
        return lv_color_hex(0xa5a8ad);
    }
}


static inline lv_color_t color_bg_txt(lv_state_t state)
{
    switch(state) {
    case LV_STATE_NORMAL:
    case LV_STATE_PRESSED:
    case LV_STATE_CHECKED:
    case LV_STATE_FOCUSED:
    case LV_STATE_EDITED:
    case LV_STATE_HOVERED:
    case LV_STATE_DISABLED:
    default:
        return lv_color_hex(0xa5a8ad);
    }
}

static inline lv_color_t color_btn_txt(lv_state_t state)
{
    switch(state) {
    case LV_STATE_NORMAL:
    case LV_STATE_PRESSED:
    case LV_STATE_CHECKED:
    case LV_STATE_FOCUSED:
    case LV_STATE_EDITED:
    case LV_STATE_HOVERED:
        return lv_color_hex(0xffffff);
    case LV_STATE_DISABLED:
        return lv_color_hex(0x888888);
    default:
        return lv_color_hex(0xffffff);
    }
}

#endif
