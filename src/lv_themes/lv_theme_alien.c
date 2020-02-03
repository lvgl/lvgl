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


#if LV_USE_BTNM
static lv_style_t btnm_bg, btnm_btn;
#endif

#if LV_USE_BTNM
static lv_style_t kb_bg, kb_btn;
#endif

#if LV_USE_BAR
static lv_style_t bar_bg;
static lv_style_t bar_indic;
#endif

#if LV_USE_SLIDER
static lv_style_t slider_knob;
#endif

#if LV_USE_SW
static lv_style_t sw_knob;
#endif

#if LV_USE_CB
static lv_style_t cb_bg, cb_bullet;
#endif

#if LV_USE_LMETER
static lv_style_t lmeter;
#endif

#if LV_USE_GAUGE
static lv_style_t gauge;
static lv_style_t gauge_strong;
#endif


#if LV_USE_LIST
static lv_style_t list_bg, list_btn;
#endif

#if LV_USE_DDLIST
static lv_style_t ddlist_bg, ddlist_sel;
#endif

#if LV_USE_TA
static lv_style_t ta_cursor, ta_oneline, ta_placeholder;
#endif

#if LV_USE_ARC
static lv_style_t arc;
static lv_style_t arc_bg;
#endif

#if LV_USE_CALENDAR
static lv_style_t calendar_date_nums, calendar_header, calendar_daynames;
#endif


#if LV_USE_TABVIEW
static lv_style_t tabview_btns, tabview_btns_bg, tabview_indic, tabaview_page_scrl;
#endif

#if LV_USE_TABLE
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
    lv_style_set_opa(&scr, LV_STYLE_BG_OPA, LV_OPA_COVER);
    lv_style_set_color(&scr, LV_STYLE_BG_COLOR, COLOR_SCREEN);
    lv_style_set_color(&scr, LV_STYLE_TEXT_COLOR , lv_color_hex(0xb8b8b9));

    lv_style_init(&panel);
    lv_style_set_int(&panel, LV_STYLE_RADIUS, LV_DPI / 25);
    lv_style_set_opa(&panel, LV_STYLE_BG_OPA, LV_OPA_COVER);
    lv_style_set_color(&panel, LV_STYLE_BG_COLOR, COLOR_CONTAINER);
//    lv_style_set_color(&panel, LV_STYLE_BG_COLOR | LV_STYLE_STATE_FOCUS, LV_COLOR_RED);
    lv_style_set_color(&panel, LV_STYLE_BORDER_COLOR, lv_color_lighten(COLOR_CONTAINER, LV_OPA_10));
    lv_style_set_int(&panel, LV_STYLE_BORDER_WIDTH, LV_DPI / 50 > 0 ? LV_DPI / 50 : 1);
    lv_style_set_int(&panel, LV_STYLE_BORDER_SIDE , LV_BORDER_SIDE_TOP);
    lv_style_set_int(&panel, LV_STYLE_PAD_LEFT, LV_DPI / 5);
    lv_style_set_int(&panel, LV_STYLE_PAD_RIGHT, LV_DPI / 5);
    lv_style_set_int(&panel, LV_STYLE_PAD_TOP, LV_DPI / 5);
    lv_style_set_int(&panel, LV_STYLE_PAD_BOTTOM, LV_DPI / 5);
    lv_style_set_int(&panel, LV_STYLE_PAD_INNER, LV_DPI / 5);
    lv_style_set_color(&panel, LV_STYLE_TEXT_COLOR, lv_color_hex(0x979a9f));
    lv_style_set_ptr(&panel, LV_STYLE_FONT, &lv_font_roboto_16);
    lv_style_set_color(&panel, LV_STYLE_IMAGE_RECOLOR, lv_color_hex(0x979a9f));
    lv_style_set_color(&panel, LV_STYLE_LINE_COLOR, lv_color_hex(0x979a9f));
    lv_style_set_int(&panel, LV_STYLE_LINE_WIDTH, 1);
    lv_style_set_color(&panel, LV_STYLE_BORDER_COLOR | LV_STYLE_STATE_FOCUS, LV_COLOR_RED);

    lv_style_init(&btn);
    lv_style_set_int(&btn, LV_STYLE_RADIUS, LV_RADIUS_CIRCLE);
    lv_style_set_opa(&btn, LV_STYLE_BG_OPA, LV_OPA_COVER);
    lv_style_set_color(&btn, LV_STYLE_BG_COLOR, COLOR_ACCENT);
    lv_style_set_color(&btn, LV_STYLE_BG_COLOR | LV_STYLE_STATE_PRESSED, lv_color_darken(COLOR_ACCENT, LV_OPA_20));
    lv_style_set_color(&btn, LV_STYLE_BG_COLOR | LV_STYLE_STATE_DISABLED, COLOR_DISABLED);
    lv_style_set_color(&btn, LV_STYLE_TEXT_COLOR, lv_color_hex(0xffffff));
    lv_style_set_color(&btn, LV_STYLE_TEXT_COLOR| LV_STYLE_STATE_PRESSED, lv_color_darken(lv_color_hex(0xffffff), LV_OPA_20));
    lv_style_set_color(&btn, LV_STYLE_TEXT_COLOR| LV_STYLE_STATE_DISABLED, lv_color_hex(0x686b70));
    lv_style_set_color(&btn, LV_STYLE_IMAGE_RECOLOR, LV_COLOR_WHITE);
    lv_style_set_color(&btn, LV_STYLE_IMAGE_RECOLOR| LV_STYLE_STATE_PRESSED, lv_color_darken(lv_color_hex(0xffffff), LV_OPA_20));
    lv_style_set_int(&btn, LV_STYLE_PAD_LEFT, LV_DPI / 5);
    lv_style_set_int(&btn, LV_STYLE_PAD_RIGHT, LV_DPI / 5);
    lv_style_set_int(&btn, LV_STYLE_PAD_TOP, LV_DPI / 10);
    lv_style_set_int(&btn, LV_STYLE_PAD_BOTTOM, LV_DPI / 10);
    lv_style_set_int(&btn, LV_STYLE_PAD_INNER, LV_DPI / 10);
    lv_style_set_int(&btn, LV_STYLE_TRANSITION_TIME, 500);
    lv_style_set_color(&btn, LV_STYLE_BG_COLOR | LV_STYLE_STATE_FOCUS, lv_color_mix(LV_COLOR_RED, COLOR_ACCENT, LV_OPA_50));
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
    lv_style_set_int(&bar_bg, LV_STYLE_RADIUS, LV_RADIUS_CIRCLE);
    lv_style_set_opa(&bar_bg, LV_STYLE_BG_OPA, LV_OPA_COVER);
    lv_style_set_color(&bar_bg, LV_STYLE_BG_COLOR, COLOR_BACKGROUND);

    lv_style_init(&bar_indic);
    lv_style_set_opa(&bar_indic, LV_STYLE_BG_OPA, LV_OPA_COVER);
    lv_style_set_int(&bar_indic, LV_STYLE_RADIUS, LV_RADIUS_CIRCLE);
    lv_style_set_color(&bar_indic, LV_STYLE_BG_COLOR, COLOR_ACCENT);
    lv_style_set_color(&bar_indic, LV_STYLE_BG_COLOR | LV_STYLE_STATE_DISABLED, COLOR_DISABLED);
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

#endif
}

static void slider_init(void)
{
#if LV_USE_SLIDER != 0
    lv_style_init(&slider_knob);
    lv_style_set_opa(&slider_knob, LV_STYLE_BG_OPA, LV_OPA_COVER);
    lv_style_set_color(&slider_knob, LV_STYLE_BG_COLOR, LV_COLOR_WHITE);
    lv_style_set_int(&slider_knob, LV_STYLE_RADIUS, LV_RADIUS_CIRCLE);
    lv_style_set_int(&slider_knob, LV_STYLE_PAD_TOP, LV_DPI/20);
    lv_style_set_int(&slider_knob, LV_STYLE_PAD_BOTTOM, LV_DPI/20);
    lv_style_set_int(&slider_knob, LV_STYLE_PAD_LEFT, LV_DPI/20);
    lv_style_set_int(&slider_knob, LV_STYLE_PAD_RIGHT, LV_DPI/20);
    lv_style_set_color(&slider_knob, LV_STYLE_BG_COLOR | LV_STYLE_STATE_FOCUS, LV_COLOR_RED);
#endif
}

static void sw_init(void)
{
#if LV_USE_SW != 0
    lv_style_init(&sw_knob);
    lv_style_set_opa(&sw_knob, LV_STYLE_BG_OPA, LV_OPA_COVER);
    lv_style_set_color(&sw_knob, LV_STYLE_BG_COLOR, LV_COLOR_WHITE);
    lv_style_set_int(&sw_knob, LV_STYLE_RADIUS, LV_RADIUS_CIRCLE);
    lv_style_set_int(&sw_knob, LV_STYLE_PAD_TOP,    - LV_DPI/20);
    lv_style_set_int(&sw_knob, LV_STYLE_PAD_BOTTOM, - LV_DPI/20);
    lv_style_set_int(&sw_knob, LV_STYLE_PAD_LEFT,   - LV_DPI/20);
    lv_style_set_int(&sw_knob, LV_STYLE_PAD_RIGHT,  - LV_DPI/20);
    lv_style_set_color(&sw_knob, LV_STYLE_BG_COLOR | LV_STYLE_STATE_FOCUS, LV_COLOR_RED);
#endif
}

static void lmeter_init(void)
{
#if LV_USE_LMETER != 0
    lv_style_init(&lmeter);
    lv_style_set_color(&lmeter, LV_STYLE_SCALE_COLOR, LV_COLOR_AQUA);
    lv_style_set_color(&lmeter, LV_STYLE_SCALE_GRAD_COLOR, LV_COLOR_NAVY);
    lv_style_set_color(&lmeter, LV_STYLE_SCALE_END_COLOR, LV_COLOR_GRAY);
    lv_style_set_int(&lmeter, LV_STYLE_LINE_WIDTH, 2);
#endif
}

static void gauge_init(void)
{
#if LV_USE_GAUGE != 0
    lv_style_init(&gauge);
    lv_style_set_color(&gauge, LV_STYLE_SCALE_COLOR, COLOR_DISABLED);
    lv_style_set_color(&gauge, LV_STYLE_SCALE_GRAD_COLOR, COLOR_DISABLED);
    lv_style_set_color(&gauge, LV_STYLE_SCALE_END_COLOR, COLOR_ACCENT);
    lv_style_set_int(&gauge, LV_STYLE_LINE_WIDTH, 2);
    lv_style_set_int(&gauge, LV_STYLE_SCALE_END_BORDER_WIDTH, 4);
    lv_style_set_opa(&gauge, LV_STYLE_BG_OPA, LV_OPA_COVER);
    lv_style_set_color(&gauge, LV_STYLE_BG_COLOR, LV_COLOR_LIME);
    lv_style_set_int(&gauge, LV_STYLE_SIZE, 4);


    lv_style_init(&gauge_strong);
    lv_style_set_color(&gauge_strong, LV_STYLE_SCALE_COLOR, COLOR_DISABLED);
    lv_style_set_color(&gauge_strong, LV_STYLE_SCALE_GRAD_COLOR, COLOR_DISABLED);
    lv_style_set_color(&gauge_strong, LV_STYLE_SCALE_END_COLOR, COLOR_ACCENT);
    lv_style_set_int(&gauge_strong, LV_STYLE_LINE_WIDTH, 4);
    lv_style_set_int(&gauge_strong, LV_STYLE_SCALE_WIDTH, LV_DPI/5);
    lv_style_set_int(&gauge_strong, LV_STYLE_PAD_INNER, LV_DPI/10);
#endif
}

static void arc_init(void)
{
#if LV_USE_ARC != 0
    lv_style_init(&arc);
    lv_style_set_color(&arc, LV_STYLE_LINE_COLOR, COLOR_ACCENT);
    lv_style_set_int(&arc, LV_STYLE_LINE_WIDTH, LV_DPI / 8);
    lv_style_set_int(&arc, LV_STYLE_LINE_ROUNDED, 1);

    lv_style_init(&arc_bg);
    lv_style_set_color(&arc_bg, LV_STYLE_LINE_COLOR, LV_COLOR_GRAY);
    lv_style_set_int(&arc_bg, LV_STYLE_LINE_WIDTH, LV_DPI / 8);
    lv_style_set_int(&arc_bg, LV_STYLE_LINE_ROUNDED, 1);

#endif
}

static void preload_init(void)
{
#if LV_USE_PRELOAD != 0

//    theme.style.preload = theme.style.arc;
#endif
}

static void chart_init(void)
{
#if LV_USE_CHART

    lv_style_init(&chart_series_bg);
    lv_style_set_int(&chart_series_bg, LV_STYLE_LINE_WIDTH , 1);
    lv_style_set_int(&chart_series_bg, LV_STYLE_LINE_DASH_WIDTH, 4);
    lv_style_set_int(&chart_series_bg, LV_STYLE_LINE_DASH_GAP, 4);
    lv_style_set_int(&chart_series_bg, LV_STYLE_PAD_BOTTOM , LV_DPI / 10);
    lv_style_set_int(&chart_series_bg, LV_STYLE_PAD_LEFT , LV_DPI / 10);
    lv_style_set_int(&chart_series_bg, LV_STYLE_PAD_RIGHT , LV_DPI / 10);
    lv_style_set_color(&chart_series_bg, LV_STYLE_LINE_COLOR, COLOR_DISABLED);


    lv_style_init(&chart_series);
    lv_style_set_int(&chart_series, LV_STYLE_LINE_WIDTH , 1);
    lv_style_set_int(&chart_series, LV_STYLE_SIZE , 2);
    lv_style_set_int(&chart_series, LV_STYLE_PAD_INNER , 2);
    lv_style_set_int(&chart_series, LV_STYLE_RADIUS , 1);
    lv_style_set_opa(&chart_series, LV_STYLE_BG_OPA , LV_OPA_COVER);
    lv_style_set_opa(&chart_series, LV_STYLE_BG_OPA, LV_OPA_100);
    lv_style_set_int(&chart_series, LV_STYLE_BG_GRAD_DIR, LV_GRAD_DIR_VER);
    lv_style_set_int(&chart_series, LV_STYLE_BG_MAIN_STOP, 96);
    lv_style_set_int(&chart_series, LV_STYLE_BG_GRAD_STOP, 32);

#endif
}

static void calendar_init(void)
{
#if LV_USE_CALENDAR

    lv_style_init(&calendar_header);
    lv_style_set_int(&calendar_header, LV_STYLE_PAD_TOP, LV_DPI / 7);
    lv_style_set_int(&calendar_header, LV_STYLE_PAD_LEFT , LV_DPI / 7);
    lv_style_set_int(&calendar_header, LV_STYLE_PAD_RIGHT , LV_DPI / 7);
    lv_style_set_int(&calendar_header, LV_STYLE_PAD_BOTTOM , LV_DPI / 7);
    lv_style_set_color(&calendar_header, LV_STYLE_TEXT_COLOR | LV_STYLE_STATE_PRESSED, LV_COLOR_WHITE);

    lv_style_init(&calendar_daynames);
    lv_style_set_int(&calendar_daynames, LV_STYLE_PAD_LEFT , LV_DPI / 7);
    lv_style_set_int(&calendar_daynames, LV_STYLE_PAD_RIGHT , LV_DPI / 7);
    lv_style_set_int(&calendar_daynames, LV_STYLE_PAD_BOTTOM , LV_DPI / 7);

    lv_style_init(&calendar_date_nums);
    lv_style_set_int(&calendar_date_nums, LV_STYLE_RADIUS, LV_DPI / 50);
    lv_style_set_color(&calendar_date_nums, LV_STYLE_TEXT_COLOR | LV_STYLE_STATE_CHECKED, LV_COLOR_WHITE);
    lv_style_set_color(&calendar_date_nums, LV_STYLE_TEXT_COLOR | LV_STYLE_STATE_FOCUS, COLOR_ACCENT);

    lv_style_set_opa(&calendar_date_nums, LV_STYLE_BG_OPA | LV_STYLE_STATE_CHECKED , LV_OPA_20);
    lv_style_set_opa(&calendar_date_nums, LV_STYLE_BG_OPA | LV_STYLE_STATE_PRESSED , LV_OPA_20);
    lv_style_set_opa(&calendar_date_nums, LV_STYLE_BG_OPA | LV_STYLE_STATE_PRESSED | LV_STYLE_STATE_CHECKED , LV_OPA_40);
    lv_style_set_color(&calendar_date_nums, LV_STYLE_BG_COLOR | LV_STYLE_STATE_CHECKED, LV_COLOR_WHITE);
    lv_style_set_int(&calendar_date_nums, LV_STYLE_BORDER_WIDTH | LV_STYLE_STATE_CHECKED , 2);
    lv_style_set_int(&calendar_date_nums, LV_STYLE_BORDER_SIDE| LV_STYLE_STATE_CHECKED , LV_BORDER_SIDE_LEFT);
    lv_style_set_color(&calendar_date_nums, LV_STYLE_BORDER_COLOR | LV_STYLE_STATE_CHECKED, COLOR_ACCENT);
    lv_style_set_int(&calendar_date_nums, LV_STYLE_PAD_INNER, LV_DPI / 30);
    lv_style_set_int(&calendar_date_nums, LV_STYLE_PAD_LEFT , LV_DPI / 7);
    lv_style_set_int(&calendar_date_nums, LV_STYLE_PAD_RIGHT , LV_DPI / 7);
    lv_style_set_int(&calendar_date_nums, LV_STYLE_PAD_BOTTOM , LV_DPI / 7);
#endif
}

static void cb_init(void)
{
#if LV_USE_CB != 0
    lv_style_init(&cb_bg);
    lv_style_set_int(&cb_bg, LV_STYLE_PAD_INNER , LV_DPI / 10);

    lv_style_init(&cb_bullet);
    lv_style_set_int(&cb_bullet, LV_STYLE_RADIUS, LV_DPI / 50);
    lv_style_set_int(&cb_bullet, LV_STYLE_BORDER_WIDTH , 2);
    lv_style_set_color(&cb_bullet, LV_STYLE_BORDER_COLOR , lv_color_hex(0x52555a));
    lv_style_set_color(&cb_bullet, LV_STYLE_BORDER_COLOR | LV_STYLE_STATE_PRESSED , lv_color_darken(lv_color_hex(0x52555a), LV_OPA_30));
    lv_style_set_color(&cb_bullet, LV_STYLE_BORDER_COLOR | LV_STYLE_STATE_FOCUS, LV_COLOR_RED);
    lv_style_set_color(&cb_bullet, LV_STYLE_BORDER_COLOR | LV_STYLE_STATE_FOCUS | LV_STYLE_STATE_PRESSED, LV_COLOR_LIME);
    lv_style_set_color(&cb_bullet, LV_STYLE_BG_COLOR, COLOR_ACCENT);
    lv_style_set_opa(&cb_bullet, LV_STYLE_BG_OPA | LV_STYLE_STATE_CHECKED  , LV_OPA_COVER);
    lv_style_set_opa(&cb_bullet, LV_STYLE_BORDER_OPA | LV_STYLE_STATE_CHECKED  , LV_OPA_80);
    lv_style_set_int(&cb_bullet, LV_STYLE_TRANSITION_TIME , 1000);



#endif
}

static void btnm_init(void)
{
#if LV_USE_BTNM
    lv_style_init(&btnm_bg);
    lv_style_set_int(&btnm_bg, LV_STYLE_BORDER_WIDTH, LV_DPI / 50);
    lv_style_set_color(&btnm_bg, LV_STYLE_BORDER_COLOR, lv_color_hex(0x373a3f));
    lv_style_set_int(&btnm_bg, LV_STYLE_RADIUS, LV_DPI / 20);
    lv_style_set_int(&btnm_bg, LV_STYLE_CLIP_CORNER, 1);
    lv_style_set_int(&btnm_bg, LV_STYLE_BORDER_POST, 1);

    lv_style_init(&btnm_btn);
    lv_style_set_int(&btnm_btn, LV_STYLE_BORDER_WIDTH, LV_DPI / 50);
    lv_style_set_int(&btnm_btn, LV_STYLE_BORDER_SIDE, LV_BORDER_SIDE_FULL);
    lv_style_set_color(&btnm_btn, LV_STYLE_BORDER_COLOR, lv_color_hex(0x373a3f));
    lv_style_set_color(&btnm_btn, LV_STYLE_BG_COLOR | LV_STYLE_STATE_PRESSED, lv_color_hex3(0x666));
    lv_style_set_color(&btnm_btn, LV_STYLE_BG_COLOR | LV_STYLE_STATE_CHECKED, COLOR_ACCENT);
    lv_style_set_color(&btnm_btn, LV_STYLE_BG_COLOR | LV_STYLE_STATE_DISABLED, COLOR_DISABLED);
    lv_style_set_color(&btnm_btn, LV_STYLE_BG_COLOR | LV_STYLE_STATE_CHECKED | LV_STYLE_STATE_PRESSED, lv_color_darken(COLOR_ACCENT, LV_OPA_40));
    lv_style_set_color(&btnm_btn, LV_STYLE_TEXT_COLOR , LV_COLOR_WHITE);
    lv_style_set_color(&btnm_btn, LV_STYLE_TEXT_COLOR | LV_STYLE_STATE_DISABLED , LV_COLOR_GRAY);
    lv_style_set_opa(&btnm_btn, LV_STYLE_BG_OPA | LV_STYLE_STATE_CHECKED, LV_OPA_COVER);
    lv_style_set_opa(&btnm_btn, LV_STYLE_BG_OPA | LV_STYLE_STATE_PRESSED, LV_OPA_COVER);
    lv_style_set_opa(&btnm_btn, LV_STYLE_BG_OPA | LV_STYLE_STATE_DISABLED, LV_OPA_COVER);

#endif
}

static void kb_init(void)
{
#if LV_USE_KB
    lv_style_init(&kb_bg);
    lv_style_copy(&kb_bg, &panel);
    lv_style_set_int(&kb_bg, LV_STYLE_PAD_INNER, LV_DPI/10);

#endif
}

static void mbox_init(void)
{
#if LV_USE_MBOX

#endif
}

static void page_init(void)
{
#if LV_USE_PAGE
    lv_style_init(&sb);
    lv_style_set_opa(&sb, LV_STYLE_BG_OPA, LV_OPA_COVER);
    lv_style_set_color(&sb, LV_STYLE_BG_COLOR, lv_color_hex(0x3a3d42));
    lv_style_set_int(&sb, LV_STYLE_RADIUS, LV_RADIUS_CIRCLE);
    lv_style_set_int(&sb,LV_STYLE_SIZE, LV_DPI / 30);
    lv_style_set_int(&sb,LV_STYLE_PAD_RIGHT,  LV_DPI / 10);
    lv_style_set_int(&sb,LV_STYLE_PAD_BOTTOM,  LV_DPI / 10);

#endif
}

static void ta_init(void)
{
#if LV_USE_TA
    lv_style_init(&ta_cursor);
    lv_style_set_color(&ta_cursor, LV_STYLE_BORDER_COLOR, lv_color_hex(0x6c737b));
    lv_style_set_int(&ta_cursor, LV_STYLE_BORDER_WIDTH, 2);
    lv_style_set_int(&ta_cursor, LV_STYLE_PAD_LEFT, 1);
    lv_style_set_int(&ta_cursor, LV_STYLE_BORDER_SIDE, LV_BORDER_SIDE_LEFT);

    lv_style_init(&ta_oneline);
    lv_style_set_int(&ta_oneline, LV_STYLE_BORDER_WIDTH, 1);
    lv_style_set_int(&ta_oneline, LV_STYLE_RADIUS, LV_RADIUS_CIRCLE);
    lv_style_set_color(&ta_oneline, LV_STYLE_BORDER_COLOR, lv_color_hex(0x3b3e43));
    lv_style_set_color(&ta_oneline, LV_STYLE_TEXT_COLOR, lv_color_hex(0x6c737b));
    lv_style_set_int(&ta_oneline, LV_STYLE_PAD_LEFT, LV_DPI / 10);
    lv_style_set_int(&ta_oneline, LV_STYLE_PAD_RIGHT, LV_DPI / 10);
    lv_style_set_int(&ta_oneline, LV_STYLE_PAD_TOP, LV_DPI / 10);
    lv_style_set_int(&ta_oneline, LV_STYLE_PAD_BOTTOM, LV_DPI / 10);

    lv_style_init(&ta_placeholder);
    lv_style_set_color(&ta_placeholder, LV_STYLE_TEXT_COLOR, lv_color_hex(0x3b3e43));
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
    lv_style_set_int(&list_bg, LV_STYLE_PAD_LEFT, LV_DPI / 10);
    lv_style_set_int(&list_bg, LV_STYLE_PAD_RIGHT, LV_DPI / 10);
    lv_style_set_int(&list_bg, LV_STYLE_PAD_TOP, 0);
    lv_style_set_int(&list_bg, LV_STYLE_PAD_BOTTOM, 0);
    lv_style_set_int(&list_bg, LV_STYLE_PAD_INNER, 0);

    lv_style_init(&list_btn);
    lv_style_set_opa(&list_btn, LV_STYLE_BG_OPA| LV_STYLE_STATE_PRESSED, LV_OPA_20);
    lv_style_set_opa(&list_btn, LV_STYLE_BG_OPA, LV_OPA_TRANSP);
    lv_style_set_color(&list_btn, LV_STYLE_BG_COLOR | LV_STYLE_STATE_PRESSED, LV_COLOR_WHITE);
    lv_style_set_color(&list_btn, LV_STYLE_BG_COLOR | LV_STYLE_STATE_DISABLED, COLOR_DISABLED);
    lv_style_set_color(&list_btn, LV_STYLE_TEXT_COLOR, lv_color_hex(0xffffff));
    lv_style_set_color(&list_btn, LV_STYLE_TEXT_COLOR| LV_STYLE_STATE_PRESSED, lv_color_darken(lv_color_hex(0xffffff), LV_OPA_20));
    lv_style_set_color(&list_btn, LV_STYLE_TEXT_COLOR| LV_STYLE_STATE_DISABLED, lv_color_hex(0x686b70));
    lv_style_set_color(&list_btn, LV_STYLE_TEXT_COLOR| LV_STYLE_STATE_FOCUS, lv_color_hex(0xff0000));
    lv_style_set_color(&list_btn, LV_STYLE_TEXT_COLOR| LV_STYLE_STATE_FOCUS| LV_STYLE_STATE_PRESSED, lv_color_hex(0xffff00));
    lv_style_set_color(&list_btn, LV_STYLE_IMAGE_RECOLOR, LV_COLOR_WHITE);
    lv_style_set_color(&list_btn, LV_STYLE_IMAGE_RECOLOR| LV_STYLE_STATE_PRESSED, lv_color_darken(lv_color_hex(0xffffff), LV_OPA_20));
    lv_style_set_opa(&list_btn, LV_STYLE_BORDER_OPA, LV_OPA_COVER);
    lv_style_set_int(&list_btn, LV_STYLE_BORDER_WIDTH, 1);
    lv_style_set_int(&list_btn, LV_STYLE_BORDER_SIDE, LV_BORDER_SIDE_BOTTOM);
    lv_style_set_color(&list_btn, LV_STYLE_BORDER_COLOR, lv_color_hex(0x979a9f));

    lv_style_set_int(&list_btn, LV_STYLE_PAD_LEFT, LV_DPI / 10);
    lv_style_set_int(&list_btn, LV_STYLE_PAD_RIGHT, LV_DPI / 10);
    lv_style_set_int(&list_btn, LV_STYLE_PAD_TOP, LV_DPI / 10);
    lv_style_set_int(&list_btn, LV_STYLE_PAD_BOTTOM, LV_DPI / 10);
    lv_style_set_int(&list_btn, LV_STYLE_PAD_INNER, LV_DPI / 10);
    lv_style_set_int(&list_btn, LV_STYLE_TRANSITION_TIME, 500);

#endif
}

static void ddlist_init(void)
{
#if LV_USE_DDLIST != 0

#endif
}

static void roller_init(void)
{
#if LV_USE_ROLLER != 0

#endif
}

static void tabview_init(void)
{
#if LV_USE_TABVIEW != 0
    lv_style_init(&tabview_btns_bg);
    lv_style_set_opa(&tabview_btns_bg, LV_STYLE_BG_OPA, LV_OPA_COVER);
    lv_style_set_color(&tabview_btns_bg, LV_STYLE_BG_COLOR, COLOR_CONTAINER);
    lv_style_set_color(&tabview_btns_bg, LV_STYLE_BORDER_COLOR, lv_color_lighten(COLOR_CONTAINER, LV_OPA_10));
    lv_style_set_int(&tabview_btns_bg, LV_STYLE_BORDER_WIDTH, LV_DPI / 30 > 0 ? LV_DPI / 30 : 1);
    lv_style_set_int(&tabview_btns_bg, LV_STYLE_BORDER_SIDE , LV_BORDER_SIDE_BOTTOM);
    lv_style_set_color(&tabview_btns_bg, LV_STYLE_TEXT_COLOR, lv_color_hex(0x979a9f));
    lv_style_set_ptr(&tabview_btns_bg, LV_STYLE_FONT, &lv_font_roboto_16);
    lv_style_set_color(&tabview_btns_bg, LV_STYLE_IMAGE_RECOLOR, lv_color_hex(0x979a9f));


    lv_style_init(&tabview_btns);
    lv_style_set_opa(&tabview_btns, LV_STYLE_BG_OPA | LV_STYLE_STATE_PRESSED, LV_OPA_COVER);
    lv_style_set_color(&tabview_btns, LV_STYLE_BG_COLOR | LV_STYLE_STATE_PRESSED, lv_color_hex(0x444444));
    lv_style_set_color(&tabview_btns, LV_STYLE_TEXT_COLOR | LV_STYLE_STATE_CHECKED, COLOR_ACCENT);
    lv_style_set_color(&tabview_btns, LV_STYLE_TEXT_COLOR | LV_STYLE_STATE_FOCUS, LV_COLOR_GREEN);
    lv_style_set_int(&tabview_btns, LV_STYLE_PAD_TOP, LV_DPI / 5);
    lv_style_set_int(&tabview_btns, LV_STYLE_PAD_BOTTOM, LV_DPI / 5);

    lv_style_init(&tabview_indic);
    lv_style_set_opa(&tabview_indic, LV_STYLE_BG_OPA, LV_OPA_COVER);
    lv_style_set_color(&tabview_indic, LV_STYLE_BG_COLOR, COLOR_ACCENT);
    lv_style_set_int(&tabview_indic, LV_STYLE_SIZE, LV_DPI / 20 > 0 ? LV_DPI / 20 : 1);
    lv_style_set_int(&tabview_indic, LV_STYLE_RADIUS, LV_RADIUS_CIRCLE);


    lv_style_init(&tabaview_page_scrl);
    lv_style_set_int(&tabaview_page_scrl, LV_STYLE_PAD_TOP, LV_DPI / 5);
    lv_style_set_int(&tabaview_page_scrl, LV_STYLE_PAD_BOTTOM, LV_DPI / 5);
    lv_style_set_int(&tabaview_page_scrl, LV_STYLE_PAD_LEFT, LV_DPI / 3);
    lv_style_set_int(&tabaview_page_scrl, LV_STYLE_PAD_RIGHT, LV_DPI / 3);
    lv_style_set_int(&tabaview_page_scrl, LV_STYLE_PAD_INNER, LV_DPI / 5);
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
    lv_style_set_color(&table_cell, LV_STYLE_BORDER_COLOR, lv_color_hex(0x303338));
    lv_style_set_int(&table_cell, LV_STYLE_BORDER_WIDTH, 1);
    lv_style_set_int(&table_cell, LV_STYLE_BORDER_SIDE, LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_BOTTOM);
    lv_style_set_int(&table_cell, LV_STYLE_PAD_LEFT, LV_DPI/7);
    lv_style_set_int(&table_cell, LV_STYLE_PAD_RIGHT, LV_DPI/7);
    lv_style_set_int(&table_cell, LV_STYLE_PAD_TOP, LV_DPI/7);
    lv_style_set_int(&table_cell, LV_STYLE_PAD_BOTTOM, LV_DPI/7);

#endif
}

static void win_init(void)
{
#if LV_USE_WIN != 0
    static lv_style_t header;

    lv_style_copy(&header, &def);
    header.body.radius         = 0;
    header.body.padding.left   = LV_DPI / 12;
    header.body.padding.right  = LV_DPI / 12;
    header.body.padding.top    = LV_DPI / 20;
    header.body.padding.bottom = LV_DPI / 20;
    header.body.main_color     = lv_color_hsv_to_rgb(_hue, 30, 60);
    header.body.grad_color     = header.body.main_color;
    header.body.border.opa     = panel.body.border.opa;
    header.body.border.width   = panel.body.border.width;
    header.body.border.color   = lv_color_hsv_to_rgb(_hue, 20, 80);
    header.body.border.part    = LV_BORDER_PART_BOTTOM;
    header.text.color          = lv_color_hsv_to_rgb(_hue, 5, 100);
    header.image.color         = lv_color_hsv_to_rgb(_hue, 5, 100);

    theme.style.win.bg      = &bg;
    theme.style.win.sb      = &sb;
    theme.style.win.header  = &header;
    theme.style.win.content = &lv_style_transp;
    theme.style.win.btn.rel = &btn_rel;
    theme.style.win.btn.pr  = &btn_pr;
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


    theme.get_style_cb = lv_theme_alien_get_style;
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
    case LV_THEME_CONT:
        list = lv_obj_get_style_list(obj, LV_CONT_PART_MAIN);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &panel);
        break;
    case LV_THEME_BTN:
        list = lv_obj_get_style_list(obj, LV_BTN_PART_MAIN);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &btn);
        break;

    case LV_THEME_BTNM:
        list = lv_obj_get_style_list(obj, LV_BTNM_PART_BG);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &btnm_bg);

        list = lv_obj_get_style_list(obj, LV_BTNM_PART_BTN);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &btnm_btn);
        break;
    case LV_THEME_KB:
        list = lv_obj_get_style_list(obj, LV_KB_PART_BG);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &kb_bg);

        list = lv_obj_get_style_list(obj, LV_KB_PART_BTN);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &btnm_btn);
        break;
    case LV_THEME_BAR:
        list = lv_obj_get_style_list(obj, LV_BAR_PART_BG);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &bar_bg);

        list = lv_obj_get_style_list(obj, LV_BAR_PART_INDIC);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &bar_indic);
        break;

    case LV_THEME_SW:
        list = lv_obj_get_style_list(obj, LV_SW_PART_BG);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &bar_bg);

        list = lv_obj_get_style_list(obj, LV_SW_PART_INDIC);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &bar_indic);

        list = lv_obj_get_style_list(obj, LV_SW_PART_KNOB);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &sw_knob);
        break;

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

#if LV_USE_PRELOAD
    case LV_THEME_PRELOAD:
        list = lv_obj_get_style_list(obj, LV_PRELOAD_PART_BG);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &arc_bg);

        list = lv_obj_get_style_list(obj, LV_PRELOAD_PART_ARC);
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

#if LV_USE_CB
    case LV_THEME_CB:
        list = lv_obj_get_style_list(obj, LV_CB_PART_BG);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &cb_bg);

        list = lv_obj_get_style_list(obj, LV_CB_PART_BULLET);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &cb_bullet);
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
#if LV_USE_TABVIEW
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
        lv_style_list_add_style(list, &tabaview_page_scrl);

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

#if LV_USE_DDLIST
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

#if LV_USE_DDLIST
    case LV_THEME_DDLIST:
        list = lv_obj_get_style_list(obj, LV_DDLIST_PART_BTN);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &btn);

        list = lv_obj_get_style_list(obj, LV_DDLIST_PART_LIST);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &panel);

        list = lv_obj_get_style_list(obj, LV_DDLIST_PART_SCRLBAR);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &sb);

        list = lv_obj_get_style_list(obj, LV_DDLIST_PART_SELECTED);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &btn);
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
#if LV_USE_TABLE
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

#if LV_USE_TA
    case LV_THEME_TA_ONELINE:
        list = lv_obj_get_style_list(obj, LV_TA_PART_BG);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &ta_oneline);

        list = lv_obj_get_style_list(obj, LV_TA_PART_PLACEHOLDER);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &ta_placeholder);

        list = lv_obj_get_style_list(obj, LV_TA_PART_CURSOR);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &ta_cursor);

        list = lv_obj_get_style_list(obj, LV_TA_PART_SCRLBAR);
        lv_style_list_reset(list);
        lv_style_list_add_style(list, &sb);
        break;
#endif

#if LV_USE_TA
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
    }


    lv_obj_refresh_style(obj);


}

lv_style_t * lv_theme_alien_get_style(lv_theme_style_t name)
{
    switch(name) {
#if LV_USE_LMETER
    case LV_THEME_LMETER:
        return &lmeter;
#endif
#if LV_USE_GAUGE
    case LV_THEME_GAUGE:
        return &gauge;
    case LV_THEME_GAUGE_STRONG:
        return &gauge_strong;
#endif
#if LV_USE_LIST
    case LV_THEME_LIST_BTN:
        return &btn;
#endif

#if LV_USE_LED
    case LV_THEME_LED:
        return &btn;
#endif
#if LV_USE_MBOX
    case LV_THEME_MBOX_BG:
        return &panel;
    case LV_THEME_MBOX_BTN_BG:
        return NULL;
    case LV_THEME_MBOX_BTN:
        return &btn;
#endif
    }

    return NULL;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif
