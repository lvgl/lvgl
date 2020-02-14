/**
 * @file lv_theme_alien.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include "lv_theme.h"
#include "../lv_objx/lv_img.h"
#include "../lv_misc/lv_types.h"

#if LV_USE_THEME_ALIEN

/*********************
 *      DEFINES
 *********************/
//#define COLOR_SCREEN        lv_color_hex3(0x34a)
//#define COLOR_CONTAINER     lv_color_hex3(0x888)
//#define COLOR_BACKGROUND    lv_color_hex3(0x0f0)
//#define COLOR_ACCENT        lv_color_hex3(0xf00)
//#define COLOR_DISABLED      lv_color_hex3(0x999)

#define COLOR_SCREEN        lv_color_hex(0x22252a)
#define COLOR_CONTAINER     lv_color_hex(0x282b30)
#define COLOR_BACKGROUND     lv_color_hex(0x2e3136)
#define COLOR_ACCENT        lv_color_hex(0x007aff)
#define COLOR_DISABLED      lv_color_hex(0x444444)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
void lv_theme_alien_apply(lv_obj_t * obj, lv_theme_style_t name);

/**********************
 *  STATIC VARIABLES
 **********************/

static lv_theme_t theme;


static lv_style_t scr;
static lv_style_t panel; /*General fancy background (e.g. to chart or ta)*/
static lv_style_t btn;
static lv_style_t sb;


#if LV_USE_BTNMATRIX
static lv_style_t btnm_bg, btnm_btn;
#endif

#if LV_USE_BTNMATRIX
static lv_style_t kb_bg, kb_btn;
#endif

#if LV_USE_BAR
static lv_style_t bar_bg;
static lv_style_t bar_indic;
#endif

#if LV_USE_SLIDER
static lv_style_t slider_knob;
#endif

#if LV_USE_SWITCH
static lv_style_t sw_knob;
#endif

#if LV_USE_CHECKBOX
static lv_style_t cb_bg, cb_bullet;
#endif

#if LV_USE_LMETER
static lv_style_t lmeter;
#endif

#if LV_USE_GAUGE
static lv_style_t gauge_main, gauge_strong, gauge_needle;
#endif


#if LV_USE_LIST
static lv_style_t list_bg, list_btn;
#endif

#if LV_USE_DROPDOWN
static lv_style_t ddlist_btn, ddlist_page, ddlist_sel;
#endif

#if LV_USE_TEXTAREA
static lv_style_t ta_cursor, ta_oneline, ta_placeholder;
#endif

#if LV_USE_LED
static lv_style_t led;
#endif

#if LV_USE_ARC
static lv_style_t arc;
static lv_style_t arc_bg;
#endif

#if LV_USE_CALENDAR
static lv_style_t calendar_date_nums, calendar_header, calendar_daynames;
#endif

#if LV_USE_CPICKER
static lv_style_t cpicker_bg, cpicker_indic;
#endif


#if LV_USE_MSGBOX
static lv_style_t mbox_btn, mbox_btn_bg;
#endif

#if LV_USE_TEXTAREABVIEW
static lv_style_t tabview_btns, tabview_btns_bg, tabview_indic, tabview_page_scrl;
#endif

#if LV_USE_TEXTAREABLE
static lv_style_t table_cell;
#endif

#if LV_USE_CHART
static lv_style_t chart_series_bg, chart_series;
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
    lv_style_set_bg_color(&scr, LV_STATE_NORMAL, COLOR_SCREEN);
    lv_style_set_text_color(&scr, LV_STATE_NORMAL, lv_color_hex(0xb8b8b9));
    lv_style_set_value_color(&scr, LV_STATE_NORMAL, lv_color_hex(0xb8b8b9));

    lv_style_init(&panel);
    lv_style_set_radius(&panel, LV_STATE_NORMAL, LV_DPI / 25);
    lv_style_set_bg_opa(&panel, LV_STATE_NORMAL, LV_OPA_COVER);
    lv_style_set_bg_color(&panel, LV_STATE_NORMAL, COLOR_CONTAINER);
    lv_style_set_border_color(&panel, LV_STATE_NORMAL, lv_color_lighten(COLOR_CONTAINER, LV_OPA_10));
    lv_style_set_border_width(&panel, LV_STATE_NORMAL, LV_DPI / 50 > 0 ? LV_DPI / 50 : 1);
    lv_style_set_border_side(&panel, LV_STATE_NORMAL , LV_BORDER_SIDE_TOP);
    lv_style_set_pad_left(&panel, LV_STATE_NORMAL, LV_DPI / 5);
    lv_style_set_pad_right(&panel, LV_STATE_NORMAL, LV_DPI / 5);
    lv_style_set_pad_top(&panel, LV_STATE_NORMAL, LV_DPI / 5);
    lv_style_set_pad_bottom(&panel, LV_STATE_NORMAL, LV_DPI / 5);
    lv_style_set_pad_inner(&panel, LV_STATE_NORMAL, LV_DPI / 5);
    lv_style_set_text_color(&panel, LV_STATE_NORMAL, lv_color_hex(0x979a9f));
    lv_style_set_value_color(&panel, LV_STATE_NORMAL, lv_color_hex(0x979a9f));
    lv_style_set_font(&panel, LV_STATE_NORMAL, &lv_font_roboto_16);
    lv_style_set_image_recolor(&panel, LV_STATE_NORMAL, lv_color_hex(0x979a9f));
    lv_style_set_line_color(&panel, LV_STATE_NORMAL, lv_color_hex(0x979a9f));
    lv_style_set_line_width(&panel, LV_STATE_NORMAL, 1);
    lv_style_set_border_color(&panel, LV_STATE_FOCUS, COLOR_ACCENT);
    lv_style_set_transition_time(&panel, LV_STATE_NORMAL, 500);

    lv_style_init(&btn);
    lv_style_set_radius(&btn, LV_STATE_NORMAL, LV_RADIUS_CIRCLE);
    lv_style_set_bg_opa(&btn, LV_STATE_NORMAL, LV_OPA_COVER);
    lv_style_set_bg_color(&btn, LV_STATE_NORMAL, COLOR_ACCENT);
    lv_style_set_bg_color(&btn, LV_STATE_PRESSED, lv_color_darken(COLOR_ACCENT, LV_OPA_20));
    lv_style_set_bg_color(&btn, LV_STATE_DISABLED, COLOR_DISABLED);
    lv_style_set_text_color(&btn, LV_STATE_NORMAL, lv_color_hex(0xffffff));
    lv_style_set_text_color(&btn, LV_STATE_PRESSED, lv_color_darken(lv_color_hex(0xffffff), LV_OPA_20));
    lv_style_set_text_color(&btn, LV_STATE_DISABLED, lv_color_hex(0x686b70));
    lv_style_set_image_recolor(&btn, LV_STATE_NORMAL, LV_COLOR_WHITE);
    lv_style_set_image_recolor(&btn, LV_STATE_PRESSED, lv_color_darken(lv_color_hex(0xffffff), LV_OPA_20));
    lv_style_set_pad_left(&btn, LV_STATE_NORMAL, LV_DPI / 5);
    lv_style_set_pad_right(&btn, LV_STATE_NORMAL, LV_DPI / 5);
    lv_style_set_pad_top(&btn, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_pad_bottom(&btn, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_pad_inner(&btn, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_transition_time(&btn, LV_STATE_NORMAL, 300);

    lv_style_set_outline_width(&btn, LV_STATE_FOCUS, 3);
    lv_style_set_outline_opa(&btn, LV_STATE_FOCUS, LV_OPA_50);
    lv_style_set_outline_color(&btn, LV_STATE_FOCUS, COLOR_ACCENT);
}

static void cont_init(void)
{
#if LV_USE_CONT != 0
    //    theme.style.cont = &panel;
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
    lv_style_set_bg_color(&bar_bg, LV_STATE_NORMAL, COLOR_BACKGROUND);
    lv_style_set_outline_color(&bar_bg, LV_STATE_FOCUS, COLOR_ACCENT);
    lv_style_set_outline_opa(&bar_bg, LV_STATE_FOCUS, LV_OPA_50);
    lv_style_set_outline_width(&bar_bg, LV_STATE_FOCUS, 3);

    lv_style_init(&bar_indic);
    lv_style_set_bg_opa(&bar_indic, LV_STATE_NORMAL, LV_OPA_COVER);
    lv_style_set_radius(&bar_indic, LV_STATE_NORMAL, LV_RADIUS_CIRCLE);
    lv_style_set_bg_color(&bar_indic, LV_STATE_NORMAL, COLOR_ACCENT);
    lv_style_set_bg_color(&bar_indic, LV_STATE_DISABLED, COLOR_DISABLED);
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
    lv_style_set_bg_color(&led, LV_STATE_NORMAL, COLOR_ACCENT);
    lv_style_set_border_width(&led, LV_STATE_NORMAL, 2);
    lv_style_set_border_opa(&led, LV_STATE_NORMAL, LV_OPA_50);
    lv_style_set_border_color(&led, LV_STATE_NORMAL, lv_color_lighten(COLOR_ACCENT, LV_OPA_30));
    lv_style_set_radius(&led, LV_STATE_NORMAL, LV_RADIUS_CIRCLE);
    lv_style_set_shadow_width(&led, LV_STATE_NORMAL, LV_DPI / 4);
    lv_style_set_shadow_color(&led, LV_STATE_NORMAL, COLOR_ACCENT);
    lv_style_set_shadow_spread(&led, LV_STATE_NORMAL, LV_DPI / 16);
#endif
}

static void slider_init(void)
{
#if LV_USE_SLIDER != 0
    lv_style_init(&slider_knob);
    lv_style_set_bg_opa(&slider_knob, LV_STATE_NORMAL, LV_OPA_COVER);
    lv_style_set_bg_color(&slider_knob, LV_STATE_NORMAL, LV_COLOR_WHITE);
    lv_style_set_radius(&slider_knob, LV_STATE_NORMAL, LV_RADIUS_CIRCLE);
    lv_style_set_pad_left(&slider_knob, LV_STATE_NORMAL, LV_DPI/20);
    lv_style_set_pad_right(&slider_knob, LV_STATE_NORMAL, LV_DPI/20);
    lv_style_set_pad_top(&slider_knob, LV_STATE_NORMAL, LV_DPI/20);
    lv_style_set_pad_bottom(&slider_knob, LV_STATE_NORMAL, LV_DPI/20);
#endif
}

static void sw_init(void)
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

static void lmeter_init(void)
{
#if LV_USE_LMETER != 0
    lv_style_init(&lmeter);
    lv_style_set_scale_color(&lmeter, LV_STATE_NORMAL, LV_COLOR_AQUA);
    lv_style_set_scale_grad_color(&lmeter, LV_STATE_NORMAL, LV_COLOR_NAVY);
    lv_style_set_scale_end_color(&lmeter, LV_STATE_NORMAL, LV_COLOR_GRAY);
    lv_style_set_line_rounded(&lmeter, LV_STATE_NORMAL, true);
    lv_style_set_line_width(&lmeter, LV_STATE_NORMAL, 4);
    lv_style_set_scale_end_line_width(&lmeter, LV_STATE_NORMAL, 2);

    /*Padding to not clip rounded line endings*/
    lv_style_set_pad_left(&lmeter, LV_STATE_NORMAL, 3);
    lv_style_set_pad_right(&lmeter, LV_STATE_NORMAL, 3);
    lv_style_set_pad_top(&lmeter, LV_STATE_NORMAL, 3);
    lv_style_set_pad_bottom(&lmeter, LV_STATE_NORMAL, 3);

#endif
}

static void gauge_init(void)
{
#if LV_USE_GAUGE != 0
    lv_style_init(&gauge_main);
    lv_style_copy(&gauge_main, &panel);
    lv_style_set_radius(&gauge_main, LV_STATE_NORMAL, LV_RADIUS_CIRCLE);
    lv_style_set_border_side(&gauge_main, LV_STATE_NORMAL, LV_BORDER_SIDE_FULL);
    lv_style_set_scale_color(&gauge_main, LV_STATE_NORMAL, COLOR_DISABLED);
    lv_style_set_scale_grad_color(&gauge_main, LV_STATE_NORMAL, COLOR_DISABLED);
    lv_style_set_scale_end_color(&gauge_main, LV_STATE_NORMAL, COLOR_ACCENT);
    lv_style_set_line_width(&gauge_main, LV_STATE_NORMAL, 2);
    lv_style_set_scale_end_line_width(&gauge_main, LV_STATE_NORMAL, 4);
    lv_style_set_scale_end_border_width(&gauge_main, LV_STATE_NORMAL, 4);
    lv_style_set_bg_opa(&gauge_main, LV_STATE_NORMAL, LV_OPA_COVER);
    lv_style_set_pad_left(&gauge_main, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_pad_right(&gauge_main, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_pad_top(&gauge_main, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_pad_inner(&gauge_main, LV_STATE_NORMAL, LV_DPI / 6);
    lv_style_set_scale_width(&gauge_main, LV_STATE_NORMAL, LV_DPI/12);

    lv_style_init(&gauge_strong);
    lv_style_set_scale_color(&gauge_strong, LV_STATE_NORMAL, COLOR_DISABLED);
    lv_style_set_scale_grad_color(&gauge_strong, LV_STATE_NORMAL, COLOR_DISABLED);
    lv_style_set_scale_end_color(&gauge_strong, LV_STATE_NORMAL, COLOR_ACCENT);
    lv_style_set_line_width(&gauge_strong, LV_STATE_NORMAL, 4);
    lv_style_set_scale_end_line_width(&gauge_strong, LV_STATE_NORMAL, 4);
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
    lv_style_set_line_color(&arc, LV_STATE_NORMAL, COLOR_ACCENT);
    lv_style_set_line_width(&arc, LV_STATE_NORMAL, LV_DPI / 8);
    lv_style_set_line_rounded(&arc, LV_STATE_NORMAL, true);

    lv_style_init(&arc_bg);
    lv_style_set_line_color(&arc_bg, LV_STATE_NORMAL, LV_COLOR_GRAY);
    lv_style_set_line_width(&arc_bg, LV_STATE_NORMAL, LV_DPI / 8);
    lv_style_set_line_rounded(&arc_bg, LV_STATE_NORMAL, true);

#endif
}

static void preload_init(void)
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
    lv_style_set_line_color(&chart_series_bg, LV_STATE_NORMAL, COLOR_DISABLED);


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
    lv_style_set_pad_left(&calendar_daynames, LV_STATE_NORMAL , LV_DPI / 7);
    lv_style_set_pad_right(&calendar_daynames, LV_STATE_NORMAL , LV_DPI / 7);
    lv_style_set_pad_bottom(&calendar_daynames, LV_STATE_NORMAL , LV_DPI / 7);

    lv_style_init(&calendar_date_nums);
    lv_style_set_radius(&calendar_date_nums, LV_STATE_NORMAL, LV_DPI / 50);
    lv_style_set_text_color(&calendar_date_nums, LV_STATE_CHECKED, LV_COLOR_WHITE);
    lv_style_set_text_color(&calendar_date_nums, LV_STATE_FOCUS, COLOR_ACCENT);

    lv_style_set_bg_opa(&calendar_date_nums, LV_STATE_CHECKED , LV_OPA_20);
    lv_style_set_bg_opa(&calendar_date_nums, LV_STATE_PRESSED , LV_OPA_20);
    lv_style_set_bg_opa(&calendar_date_nums, LV_STATE_PRESSED | LV_STATE_CHECKED , LV_OPA_40);
    lv_style_set_bg_color(&calendar_date_nums, LV_STATE_CHECKED, LV_COLOR_WHITE);
    lv_style_set_border_width(&calendar_date_nums, LV_STATE_CHECKED , 2);
    lv_style_set_border_side(&calendar_date_nums, LV_STATE_CHECKED , LV_BORDER_SIDE_LEFT);
    lv_style_set_border_color(&calendar_date_nums,LV_STATE_CHECKED, COLOR_ACCENT);
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
    lv_style_set_bg_color(&cpicker_bg, LV_STATE_NORMAL, COLOR_SCREEN);
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

static void cb_init(void)
{
#if LV_USE_CHECKBOX != 0
    lv_style_init(&cb_bg);
    lv_style_set_radius(&cb_bg, LV_STATE_NORMAL, LV_DPI / 50);
//    lv_style_set_pad_left(&cb_bg, LV_STATE_NORMAL, LV_DPI / 20);
//    lv_style_set_pad_right(&cb_bg, LV_STATE_NORMAL, LV_DPI / 20);
//    lv_style_set_pad_bottom(&cb_bg, LV_STATE_NORMAL, LV_DPI / 20);
//    lv_style_set_pad_top(&cb_bg, LV_STATE_NORMAL, LV_DPI / 20);
    lv_style_set_pad_inner(&cb_bg, LV_STATE_NORMAL , LV_DPI / 20);
    lv_style_set_outline_color(&cb_bg, LV_STATE_FOCUS, COLOR_ACCENT);
    lv_style_set_outline_opa(&cb_bg, LV_STATE_FOCUS, LV_OPA_50);
    lv_style_set_outline_width(&cb_bg, LV_STATE_FOCUS, 3);
    lv_style_set_outline_pad(&cb_bg, LV_STATE_FOCUS, LV_DPI/20);

    lv_style_init(&cb_bullet);
    lv_style_set_radius(&cb_bullet, LV_STATE_NORMAL, LV_DPI / 50);
    lv_style_set_border_width(&cb_bullet, LV_STATE_NORMAL , 2);
    lv_style_set_border_color(&cb_bullet, LV_STATE_NORMAL , lv_color_hex(0x52555a));
    lv_style_set_border_color(&cb_bullet, LV_STATE_PRESSED , lv_color_darken(lv_color_hex(0x52555a), LV_OPA_30));
    lv_style_set_bg_color(&cb_bullet, LV_STATE_NORMAL, COLOR_ACCENT);
    lv_style_set_bg_opa(&cb_bullet, LV_STATE_NORMAL  , LV_OPA_TRANSP);
    lv_style_set_bg_opa(&cb_bullet, LV_STATE_CHECKED  , LV_OPA_COVER);
    lv_style_set_border_opa(&cb_bullet, LV_STATE_CHECKED  , LV_OPA_80);
    lv_style_set_transition_time(&cb_bullet, LV_STATE_NORMAL , 300);
#endif
}

static void btnm_init(void)
{
#if LV_USE_BTNMATRIX
    lv_style_init(&btnm_bg);
    lv_style_set_bg_opa(&btnm_bg, LV_STATE_NORMAL, LV_OPA_COVER);
    lv_style_set_border_width(&btnm_bg, LV_STATE_NORMAL, LV_DPI / 50);
    lv_style_set_border_color(&btnm_bg, LV_STATE_NORMAL, lv_color_hex(0x373a3f));
    lv_style_set_radius(&btnm_bg, LV_STATE_NORMAL, LV_DPI / 20);
    lv_style_set_clip_corner(&btnm_bg, LV_STATE_NORMAL, true);
    lv_style_set_border_post(&btnm_bg, LV_STATE_NORMAL, true);
    lv_style_set_bg_color(&btnm_bg, LV_STATE_NORMAL, COLOR_CONTAINER);
    lv_style_set_border_color(&btnm_bg, LV_STATE_FOCUS, lv_color_hex3(0xf66));

    lv_style_init(&btnm_btn);
    lv_style_set_border_width(&btnm_btn, LV_STATE_NORMAL, LV_DPI / 70 > 0? LV_DPI / 70 : 1);
    lv_style_set_border_side(&btnm_btn, LV_STATE_NORMAL, LV_BORDER_SIDE_FULL);
    lv_style_set_border_color(&btnm_btn, LV_STATE_NORMAL, lv_color_hex(0x373a3f));
    lv_style_set_bg_color(&btnm_btn, LV_STATE_PRESSED, lv_color_hex3(0x666));
    lv_style_set_bg_color(&btnm_btn, LV_STATE_CHECKED, COLOR_ACCENT);
    lv_style_set_bg_color(&btnm_btn, LV_STATE_DISABLED, COLOR_DISABLED);
    lv_style_set_bg_color(&btnm_btn, LV_STATE_CHECKED | LV_STATE_PRESSED, lv_color_darken(COLOR_ACCENT, LV_OPA_40));
    lv_style_set_text_color(&btnm_btn, LV_STATE_NORMAL, LV_COLOR_WHITE);
    lv_style_set_text_color(&btnm_btn, LV_STATE_DISABLED , LV_COLOR_GRAY);
    lv_style_set_text_color(&btnm_btn, LV_STATE_FOCUS, LV_COLOR_RED);
    lv_style_set_bg_opa(&btnm_btn, LV_STATE_CHECKED, LV_OPA_COVER);
    lv_style_set_bg_opa(&btnm_btn, LV_STATE_PRESSED, LV_OPA_COVER);
    lv_style_set_bg_opa(&btnm_btn, LV_STATE_DISABLED, LV_OPA_COVER);

#endif
}

static void kb_init(void)
{
#if LV_USE_KEYBOARD
    lv_style_init(&kb_bg);
    lv_style_copy(&kb_bg, &panel);
    lv_style_set_pad_inner(&kb_bg, LV_STATE_NORMAL, LV_DPI/20);
    lv_style_set_pad_left(&kb_bg, LV_STATE_NORMAL, LV_DPI/20);
    lv_style_set_pad_right(&kb_bg, LV_STATE_NORMAL, LV_DPI/20);
    lv_style_set_pad_top(&kb_bg, LV_STATE_NORMAL, LV_DPI/20);
    lv_style_set_pad_bottom(&kb_bg, LV_STATE_NORMAL, LV_DPI/20);

#endif
}

static void mbox_init(void)
{
#if LV_USE_MSGBOX
    lv_style_init(&mbox_btn_bg);
    lv_style_set_pad_right(&mbox_btn_bg, LV_STATE_NORMAL,  LV_DPI / 10);
    lv_style_set_pad_bottom(&mbox_btn_bg, LV_STATE_NORMAL,  LV_DPI / 10);
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
    lv_style_set_pad_right(&sb,LV_STATE_NORMAL,  LV_DPI / 10);
    lv_style_set_pad_bottom(&sb,LV_STATE_NORMAL,  LV_DPI / 10);

#endif
}

static void ta_init(void)
{
#if LV_USE_TEXTAREA
    lv_style_init(&ta_cursor);
    lv_style_set_border_color(&ta_cursor, LV_STATE_NORMAL, lv_color_hex(0x6c737b));
    lv_style_set_border_width(&ta_cursor, LV_STATE_NORMAL, 2);
    lv_style_set_pad_left(&ta_cursor, LV_STATE_NORMAL, 1);
    lv_style_set_border_side(&ta_cursor, LV_STATE_NORMAL, LV_BORDER_SIDE_LEFT);

    lv_style_init(&ta_oneline);
    lv_style_set_border_width(&ta_oneline, LV_STATE_NORMAL, 1);
    lv_style_set_radius(&ta_oneline, LV_STATE_NORMAL, LV_RADIUS_CIRCLE);
    lv_style_set_border_color(&ta_oneline, LV_STATE_NORMAL, lv_color_hex(0x3b3e43));
    lv_style_set_text_color(&ta_oneline, LV_STATE_NORMAL, lv_color_hex(0x6c737b));
    lv_style_set_pad_left(&ta_oneline, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_pad_right(&ta_oneline, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_pad_top(&ta_oneline, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_pad_bottom(&ta_oneline, LV_STATE_NORMAL, LV_DPI / 10);

    lv_style_init(&ta_placeholder);
    lv_style_set_text_color(&ta_placeholder, LV_STATE_NORMAL, lv_color_hex(0x3b3e43));
#endif
}

static void spinbox_init(void)
{
#if LV_USE_SPINBOX
#endif
}

static void list_init(void)
{
#if LV_USE_LIST != 0
    lv_style_init(&list_bg);
    lv_style_copy(&list_bg, &panel);
    lv_style_set_pad_left(&list_bg, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_pad_right(&list_bg, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_pad_top(&list_bg, LV_STATE_NORMAL, 0);
    lv_style_set_pad_bottom(&list_bg, LV_STATE_NORMAL, 0);
    lv_style_set_pad_inner(&list_bg, LV_STATE_NORMAL, 0);

    lv_style_init(&list_btn);
    lv_style_set_bg_opa(&list_btn, LV_STATE_PRESSED, LV_OPA_20);
    lv_style_set_bg_opa(&list_btn, LV_STATE_NORMAL, LV_OPA_TRANSP);
    lv_style_set_bg_color(&list_btn, LV_STATE_PRESSED, LV_COLOR_WHITE);
    lv_style_set_bg_color(&list_btn, LV_STATE_DISABLED, COLOR_DISABLED);
    lv_style_set_text_color(&list_btn, LV_STATE_NORMAL, lv_color_hex(0xffffff));
    lv_style_set_text_color(&list_btn, LV_STATE_PRESSED, lv_color_darken(lv_color_hex(0xffffff), LV_OPA_20));
    lv_style_set_text_color(&list_btn, LV_STATE_DISABLED, lv_color_hex(0x686b70));
    lv_style_set_text_color(&list_btn, LV_STATE_FOCUS, lv_color_hex(0xff0000));
    lv_style_set_text_color(&list_btn, LV_STATE_FOCUS| LV_STATE_PRESSED, lv_color_hex(0xffff00));
    lv_style_set_image_recolor(&list_btn, LV_STATE_NORMAL, LV_COLOR_WHITE);
    lv_style_set_image_recolor(&list_btn, LV_STATE_PRESSED, lv_color_darken(lv_color_hex(0xffffff), LV_OPA_20));
    lv_style_set_border_opa(&list_btn, LV_STATE_NORMAL, LV_OPA_COVER);
    lv_style_set_border_width(&list_btn, LV_STATE_NORMAL, 1);
    lv_style_set_border_side(&list_btn, LV_STATE_NORMAL, LV_BORDER_SIDE_BOTTOM);
    lv_style_set_border_color(&list_btn, LV_STATE_NORMAL, lv_color_hex(0x979a9f));

    lv_style_set_pad_left(&list_btn, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_pad_right(&list_btn, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_pad_top(&list_btn, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_pad_bottom(&list_btn, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_pad_inner(&list_btn, LV_STATE_NORMAL, LV_DPI / 10);
    lv_style_set_transition_time(&list_btn, LV_STATE_NORMAL, 500);

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
    lv_style_set_bg_color(&ddlist_page, LV_STATE_NORMAL, lv_color_hex3(0xeee));
    lv_style_set_text_color(&ddlist_page, LV_STATE_NORMAL, lv_color_hex3(0x333));
    lv_style_set_text_line_space(&ddlist_page, LV_STATE_NORMAL, LV_DPI / 5);
    lv_style_set_border_post(&ddlist_page, LV_STATE_NORMAL, true);

    lv_style_init(&ddlist_sel);
    lv_style_set_bg_opa(&ddlist_sel, LV_STATE_NORMAL, LV_OPA_COVER);
    lv_style_set_bg_color(&ddlist_sel, LV_STATE_NORMAL, COLOR_ACCENT);

    lv_style_set_bg_color(&ddlist_sel, LV_STATE_PRESSED, LV_COLOR_SILVER);

    lv_style_set_text_color(&ddlist_sel, LV_STATE_NORMAL, lv_color_hex3(0x333));



#endif
}

static void roller_init(void)
{
#if LV_USE_ROLLER != 0

#endif
}

static void tabview_init(void)
{
#if LV_USE_TEXTAREABVIEW != 0
    lv_style_init(&tabview_btns_bg);
    lv_style_set_bg_opa(&tabview_btns_bg, LV_STATE_NORMAL, LV_OPA_COVER);
    lv_style_set_bg_color(&tabview_btns_bg, LV_STATE_NORMAL, COLOR_CONTAINER);
    lv_style_set_border_color(&tabview_btns_bg, LV_STATE_NORMAL, lv_color_lighten(COLOR_CONTAINER, LV_OPA_10));
    lv_style_set_border_width(&tabview_btns_bg, LV_STATE_NORMAL, LV_DPI / 30 > 0 ? LV_DPI / 30 : 1);
    lv_style_set_border_side(&tabview_btns_bg, LV_STATE_NORMAL , LV_BORDER_SIDE_BOTTOM);
    lv_style_set_text_color(&tabview_btns_bg, LV_STATE_NORMAL, lv_color_hex(0x979a9f));
    lv_style_set_font(&tabview_btns_bg, LV_STATE_NORMAL, &lv_font_roboto_16);
    lv_style_set_image_recolor(&tabview_btns_bg, LV_STATE_NORMAL, lv_color_hex(0x979a9f));


    lv_style_init(&tabview_btns);
    lv_style_set_bg_opa(&tabview_btns, LV_STATE_PRESSED, LV_OPA_COVER);
    lv_style_set_bg_color(&tabview_btns, LV_STATE_PRESSED, lv_color_hex(0x444444));
    lv_style_set_text_color(&tabview_btns, LV_STATE_CHECKED, COLOR_ACCENT);
    lv_style_set_text_color(&tabview_btns, LV_STATE_FOCUS, LV_COLOR_RED);
    lv_style_set_pad_top(&tabview_btns, LV_STATE_NORMAL, LV_DPI / 5);
    lv_style_set_pad_bottom(&tabview_btns, LV_STATE_NORMAL, LV_DPI / 5);

    lv_style_init(&tabview_indic);
    lv_style_set_bg_opa(&tabview_indic, LV_STATE_NORMAL, LV_OPA_COVER);
    lv_style_set_bg_color(&tabview_indic, LV_STATE_NORMAL, COLOR_ACCENT);
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
#if LV_USE_TEXTAREABLE != 0
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
 * Initialize the alien theme
 * @param hue [0..360] hue value from HSV color space to define the theme's base color
 * @param font pointer to a font (NULL to use the default)
 */
lv_theme_t * lv_theme_alien_init(uint16_t hue, lv_font_t * font)
{

    lv_mem_monitor_t mon1;
    lv_mem_monitor(&mon1);

    basic_init();
    cont_init();
    btn_init();
    label_init();
    bar_init();
    img_init();
    line_init();
    led_init();
    slider_init();
    sw_init();
    lmeter_init();
    gauge_init();
    arc_init();
    preload_init();
    chart_init();
    calendar_init();
    cpicker_init();
    cb_init();
    btnm_init();
    kb_init();
    mbox_init();
    page_init();
    ta_init();
    spinbox_init();
    list_init();
    ddlist_init();
    roller_init();
    tabview_init();
    tileview_init();
    table_init();
    win_init();

    theme.apply_cb = lv_theme_alien_apply;

    lv_mem_monitor_t mon2;
    lv_mem_monitor(&mon2);
    printf("theme size: %d\n", mon1.free_size - mon2.free_size);
    return &theme;
}

/**
 * Get a pointer to the theme
 * @return pointer to the theme
 */
lv_theme_t * lv_theme_alien_get(void)
{
    return &theme;
}


void lv_theme_alien_apply(lv_obj_t * obj, lv_theme_style_t name)
{
    lv_style_list_t * list;

    switch(name) {
    case LV_THEME_SCR:
        list = lv_obj_get_style_list(obj, LV_OBJ_PART_MAIN);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &scr);
        break;
    case LV_THEME_OBJ:
        list = lv_obj_get_style_list(obj, LV_OBJ_PART_MAIN);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &panel);
        break;
#if LV_USE_CONT
    case LV_THEME_CONT:
        list = lv_obj_get_style_list(obj, LV_CONT_PART_MAIN);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &panel);
        break;
#endif

#if LV_USE_BTN
    case LV_THEME_BTN:
        list = lv_obj_get_style_list(obj, LV_BTN_PART_MAIN);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &btn);
        break;
#endif

#if LV_USE_BTNMATRIX
    case LV_THEME_BTNM:
        list = lv_obj_get_style_list(obj, LV_BTNMATRIX_PART_BG);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &btnm_bg);

        list = lv_obj_get_style_list(obj, LV_BTNMATRIX_PART_BTN);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &btnm_btn);
        break;
#endif

#if LV_USE_KEYBOARD
    case LV_THEME_KB:
        list = lv_obj_get_style_list(obj, LV_KEYBOARD_PART_BG);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &kb_bg);

        list = lv_obj_get_style_list(obj, LV_KEYBOARD_PART_BTN);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &btnm_btn);
        break;
#endif

#if LV_USE_BAR
    case LV_THEME_BAR:
        list = lv_obj_get_style_list(obj, LV_BAR_PART_BG);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &bar_bg);

        list = lv_obj_get_style_list(obj, LV_BAR_PART_INDIC);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &bar_indic);
        break;
#endif

#if LV_USE_SWITCH
    case LV_THEME_SW:
        list = lv_obj_get_style_list(obj, LV_SWITCH_PART_BG);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &bar_bg);

        list = lv_obj_get_style_list(obj, LV_SWITCH_PART_INDIC);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &bar_indic);

        list = lv_obj_get_style_list(obj, LV_SWITCH_PART_KNOB);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &sw_knob);
        break;
#endif

#if LV_USE_CANVAS
    case LV_THEME_CANVAS:
        list = lv_obj_get_style_list(obj, LV_CANVAS_PART_MAIN);
        lv_style_list_reset(list);
        break;
#endif

#if LV_USE_IMG
    case LV_THEME_IMAGE:
        list = lv_obj_get_style_list(obj, LV_IMG_PART_MAIN);
        lv_style_list_reset(list);
        break;
#endif

#if LV_USE_IMGBTN
    case LV_THEME_IMGBTN:
        list = lv_obj_get_style_list(obj, LV_IMG_PART_MAIN);
        lv_style_list_reset(list);
        break;
#endif

#if LV_USE_LABEL
    case LV_THEME_LABEL:
        list = lv_obj_get_style_list(obj, LV_LABEL_PART_MAIN);
        lv_style_list_reset(list);
        break;
#endif

#if LV_USE_LINE
    case LV_THEME_LINE:
        list = lv_obj_get_style_list(obj, LV_LABEL_PART_MAIN);
        lv_style_list_reset(list);
        break;
#endif

#if LV_USE_ARC
    case LV_THEME_ARC:
        list = lv_obj_get_style_list(obj, LV_ARC_PART_BG);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &arc_bg);

        list = lv_obj_get_style_list(obj, LV_ARC_PART_ARC);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &arc);
        break;
#endif

#if LV_USE_SPINNER
    case LV_THEME_PRELOAD:
        list = lv_obj_get_style_list(obj, LV_SPINNER_PART_BG);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &arc_bg);

        list = lv_obj_get_style_list(obj, LV_SPINNER_PART_ARC);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &arc);
        break;
#endif

#if LV_USE_SLIDER
    case LV_THEME_SLIDER:
        list = lv_obj_get_style_list(obj, LV_SLIDER_PART_BG);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &bar_bg);

        list = lv_obj_get_style_list(obj, LV_SLIDER_PART_INDIC);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &bar_indic);

        list = lv_obj_get_style_list(obj, LV_SLIDER_PART_KNOB);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &slider_knob);
        break;
#endif

#if LV_USE_CHECKBOX
    case LV_THEME_CB:
        list = lv_obj_get_style_list(obj, LV_CHECKBOX_PART_BG);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &cb_bg);

        list = lv_obj_get_style_list(obj, LV_CHECKBOX_PART_BULLET);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &cb_bullet);
        break;
#endif

#if LV_USE_MSGBOX
    case LV_THEME_MBOX:
        list = lv_obj_get_style_list(obj, LV_MSGBOX_PART_BG);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &panel);
        break;

    case LV_THEME_MBOX_BTNS:
        list = lv_obj_get_style_list(obj, LV_MSGBOX_PART_BTN_BG);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &mbox_btn_bg);

        list = lv_obj_get_style_list(obj, LV_MSGBOX_PART_BTN);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &btnm_btn);
        break;

#endif
#if LV_USE_LED
    case LV_THEME_LED:
        list = lv_obj_get_style_list(obj, LV_LED_PART_MAIN);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &led);
        break;
#endif
#if LV_USE_PAGE
    case LV_THEME_PAGE:
        list = lv_obj_get_style_list(obj, LV_PAGE_PART_BG);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &panel);

        list = lv_obj_get_style_list(obj, LV_PAGE_PART_SCRL);
        lv_style_list_reset(list);

        list = lv_obj_get_style_list(obj, LV_PAGE_PART_SCRLBAR);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &sb);
        break;
#endif
#if LV_USE_TEXTAREABVIEW
    case LV_THEME_TABVIEW:
        list = lv_obj_get_style_list(obj, LV_TABVIEW_PART_BG);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &scr);

        list = lv_obj_get_style_list(obj, LV_TABVIEW_PART_BG_SCRL);
        lv_style_list_reset(list);

        list = lv_obj_get_style_list(obj, LV_TABVIEW_PART_TAB_BG);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &tabview_btns_bg);


        list = lv_obj_get_style_list(obj, LV_TABVIEW_PART_INDIC);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &tabview_indic);

        list = lv_obj_get_style_list(obj, LV_TABVIEW_PART_TAB);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &tabview_btns);
        break;

    case LV_THEME_TABVIEW_PAGE:
        list = lv_obj_get_style_list(obj, LV_PAGE_PART_BG);
        lv_style_list_reset(list);

        list = lv_obj_get_style_list(obj, LV_PAGE_PART_SCRL);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &tabview_page_scrl);

        break;
#endif

#if LV_USE_TILEVIEW
    case LV_THEME_TILEVIEW:
        list = lv_obj_get_style_list(obj, LV_TILEVIEW_PART_BG);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &scr);

        list = lv_obj_get_style_list(obj, LV_TILEVIEW_PART_SCRLBAR);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &sb);

        list = lv_obj_get_style_list(obj, LV_TILEVIEW_PART_EDGE_FLASH);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &btn);
        break;
#endif


#if LV_USE_ROLLER
    case LV_THEME_ROLLER:
        list = lv_obj_get_style_list(obj, LV_ROLLER_PART_BG);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &panel);

        list = lv_obj_get_style_list(obj, LV_ROLLER_PART_SEL);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &btn);
        break;
#endif


#if LV_USE_OBJMASK
    case LV_THEME_OBJMASK:
        list = lv_obj_get_style_list(obj, LV_OBJMASK_PART_MAIN);
        lv_style_list_reset(list);
        break;
#endif

#if LV_USE_LIST
    case LV_THEME_LIST:
        list = lv_obj_get_style_list(obj, LV_LIST_PART_BG);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &list_bg);

        list = lv_obj_get_style_list(obj, LV_LIST_PART_SCRL);
        lv_style_list_reset(list);

        list = lv_obj_get_style_list(obj, LV_LIST_PART_SCRLBAR);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &sb);
        break;

    case LV_THEME_LIST_BTN:
        list = lv_obj_get_style_list(obj, LV_BTN_PART_MAIN);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &list_btn);
        break;
#endif

#if LV_USE_DROPDOWN
    case LV_THEME_DDLIST:
        list = lv_obj_get_style_list(obj, LV_DROPDOWN_PART_BTN);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &ddlist_btn);

        list = lv_obj_get_style_list(obj, LV_DROPDOWN_PART_LIST);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &ddlist_page);

        list = lv_obj_get_style_list(obj, LV_DROPDOWN_PART_SCRLBAR);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &sb);

        list = lv_obj_get_style_list(obj, LV_DROPDOWN_PART_SELECTED);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &ddlist_sel);
        break;
#endif

#if LV_USE_CHART
    case LV_THEME_CHART:
        list = lv_obj_get_style_list(obj, LV_CHART_PART_BG);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &panel);

        list = lv_obj_get_style_list(obj, LV_CHART_PART_SERIES_BG);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &chart_series_bg);

        list = lv_obj_get_style_list(obj, LV_CHART_PART_SERIES);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &chart_series);
        break;
#endif
#if LV_USE_TEXTAREABLE
    case LV_THEME_TABLE:
        list = lv_obj_get_style_list(obj, LV_TABLE_PART_BG);
        lv_style_list_reset(list);

        list = lv_obj_get_style_list(obj, LV_TABLE_PART_CELL1);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &table_cell);

        list = lv_obj_get_style_list(obj, LV_TABLE_PART_CELL2);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &table_cell);

        list = lv_obj_get_style_list(obj, LV_TABLE_PART_CELL3);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &table_cell);

        list = lv_obj_get_style_list(obj, LV_TABLE_PART_CELL4);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &table_cell);
        break;
#endif

#if LV_USE_WIN
    case LV_THEME_WIN:
        list = lv_obj_get_style_list(obj, LV_WIN_PART_BG);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &scr);

        list = lv_obj_get_style_list(obj, LV_WIN_PART_SCRLBAR);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &sb);

        list = lv_obj_get_style_list(obj, LV_WIN_PART_CONTENT_SCRL);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &tabview_page_scrl);

        list = lv_obj_get_style_list(obj, LV_WIN_PART_HEADER);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &tabview_btns_bg);
        break;
    case LV_THEME_WIN_BTN:
        list = lv_obj_get_style_list(obj, LV_BTN_PART_MAIN);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &tabview_btns);
        break;
#endif

#if LV_USE_TEXTAREA
    case LV_THEME_TA:
        list = lv_obj_get_style_list(obj, LV_TEXTAREA_PART_BG);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &panel);

        list = lv_obj_get_style_list(obj, LV_TEXTAREA_PART_PLACEHOLDER);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &ta_placeholder);

        list = lv_obj_get_style_list(obj, LV_TEXTAREA_PART_CURSOR);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &ta_cursor);

        list = lv_obj_get_style_list(obj, LV_TEXTAREA_PART_SCRLBAR);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &sb);
        break;

    case LV_THEME_TA_ONELINE:
        list = lv_obj_get_style_list(obj, LV_TEXTAREA_PART_BG);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &ta_oneline);

        list = lv_obj_get_style_list(obj, LV_TEXTAREA_PART_PLACEHOLDER);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &ta_placeholder);

        list = lv_obj_get_style_list(obj, LV_TEXTAREA_PART_CURSOR);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &ta_cursor);

        list = lv_obj_get_style_list(obj, LV_TEXTAREA_PART_SCRLBAR);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &sb);
        break;
#endif

#if LV_USE_CALENDAR
    case LV_THEME_CALENDAR:
        list = lv_obj_get_style_list(obj, LV_CALENDAR_PART_BG);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &panel);

        list = lv_obj_get_style_list(obj, LV_CALENDAR_PART_DATE);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &calendar_date_nums);

        list = lv_obj_get_style_list(obj, LV_CALENDAR_PART_HEADER);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &calendar_header);

        list = lv_obj_get_style_list(obj, LV_CALENDAR_PART_DAY_NAMES);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &calendar_daynames);
        break;
#endif
#if LV_USE_CPICKER
    case LV_THEME_CPICKER:
        list = lv_obj_get_style_list(obj, LV_CPICKER_PART_MAIN);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &cpicker_bg);

        list = lv_obj_get_style_list(obj, LV_CPICKER_PART_INDIC);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &cpicker_indic);
        break;
#endif

#if LV_USE_LMETER
    case LV_THEME_LMETER:
        list = lv_obj_get_style_list(obj, LV_LINEMETER_PART_MAIN);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &lmeter);
        break;
#endif
#if LV_USE_GAUGE
    case LV_THEME_GAUGE:
        list = lv_obj_get_style_list(obj, LV_GAUGE_PART_MAIN);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &gauge_main);

        list = lv_obj_get_style_list(obj, LV_GAUGE_PART_STRONG);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &gauge_strong);

        list = lv_obj_get_style_list(obj, LV_GAUGE_PART_NEEDLE);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &gauge_needle);
        break;
#endif
    }


    lv_obj_refresh_style(obj);


}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif
