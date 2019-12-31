/**
 * @file lv_theme_alien.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_theme.h"

#if LV_USE_THEME_ALIEN

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

static lv_theme_t theme;

static lv_style_t scr;
static lv_style_t transp;
static lv_style_t panel; /*General fancy background (e.g. to chart or ta)*/
static lv_style_t btn;
static lv_style_t sb;

#if LV_USE_BAR
static lv_style_t bar_indic;
#endif

#if LV_USE_SLIDER
static lv_style_t knob;
#endif

#if LV_USE_LMETER
static lv_style_t lmeter_bg;
#endif

#if LV_USE_DDLIST
static lv_style_t ddlist_bg, ddlist_sel;
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
    lv_style_set_color(&scr, LV_STYLE_BG_COLOR, LV_COLOR_MAKE(0x20, 0x20, 0x20));
    lv_style_set_color(&scr, LV_STYLE_TEXT_COLOR , LV_COLOR_WHITE);

    lv_style_init(&transp);
    lv_style_set_value(&transp, LV_STYLE_BG_OPA, LV_OPA_TRANSP);
    lv_style_set_value(&transp, LV_STYLE_BORDER_WIDTH, 0);

    lv_style_init(&panel);
    lv_style_set_value(&panel, LV_STYLE_PAD_LEFT, LV_DPI / 20);
    lv_style_set_value(&panel, LV_STYLE_PAD_RIGHT, LV_DPI / 20);
    lv_style_set_value(&panel, LV_STYLE_PAD_TOP, LV_DPI / 20);
    lv_style_set_value(&panel, LV_STYLE_PAD_BOTTOM, LV_DPI / 20);
    lv_style_set_value(&panel, LV_STYLE_PAD_INNER, LV_DPI / 20);
    lv_style_set_value(&panel, LV_STYLE_RADIUS, LV_DPI / 16);
    lv_style_set_value(&panel, LV_STYLE_BORDER_WIDTH, LV_DPI / 50 > 0 ? LV_DPI / 50 : 1);
    lv_style_set_color(&panel, LV_STYLE_BG_COLOR, LV_COLOR_SILVER);
    lv_style_set_color(&panel, LV_STYLE_BG_GRAD_COLOR, LV_COLOR_GRAY);
    lv_style_set_color(&panel, LV_STYLE_BORDER_COLOR, LV_COLOR_GRAY);
    lv_style_set_color(&panel, LV_STYLE_TEXT_COLOR, LV_COLOR_BLACK);


    lv_style_init(&btn);
    lv_style_set_value(&btn, LV_STYLE_PAD_LEFT, LV_DPI / 20);
    lv_style_set_value(&btn, LV_STYLE_PAD_RIGHT, LV_DPI / 20);
    lv_style_set_value(&btn, LV_STYLE_PAD_TOP, LV_DPI / 20);
    lv_style_set_value(&btn, LV_STYLE_PAD_BOTTOM, LV_DPI / 20);
    lv_style_set_value(&btn, LV_STYLE_PAD_INNER, LV_DPI / 16);
    lv_style_set_value(&btn, LV_STYLE_RADIUS, 5);
    lv_style_set_value(&btn, LV_STYLE_BORDER_WIDTH, 2);//LV_DPI / 50 > 0 ? LV_DPI / 50 : 1);
    lv_style_set_value(&btn, LV_STYLE_BG_GRAD_DIR, LV_GRAD_DIR_VER);
    lv_style_set_color(&btn, LV_STYLE_BG_COLOR, LV_COLOR_RED);
    lv_style_set_color(&btn, LV_STYLE_BG_GRAD_COLOR, LV_COLOR_MAROON);
    lv_style_set_color(&btn, LV_STYLE_BORDER_COLOR, LV_COLOR_NAVY);
    lv_style_set_color(&btn, LV_STYLE_BG_GRAD_COLOR | LV_STYLE_STATE_PRESSED, LV_COLOR_LIME);
    lv_style_set_color(&btn, LV_STYLE_BG_COLOR | LV_STYLE_STATE_CHECKED, LV_COLOR_BLUE);
    lv_style_set_color(&btn, LV_STYLE_TEXT_COLOR, LV_COLOR_WHITE);
    lv_style_set_opa(&btn, LV_STYLE_BG_OPA, LV_OPA_50);
    lv_style_set_opa(&btn, LV_STYLE_BORDER_OPA, LV_OPA_70);
    lv_style_set_opa(&btn, LV_STYLE_TEXT_OPA, LV_OPA_50);
    lv_style_set_value(&btn, LV_STYLE_SHADOW_WIDTH, 5);
    lv_style_set_value(&btn, LV_STYLE_SHADOW_OFFSET_Y, 10);
    lv_style_set_color(&btn, LV_STYLE_BORDER_COLOR | LV_STYLE_STATE_FOCUS, LV_COLOR_AQUA);
    lv_style_set_value(&btn, LV_STYLE_BORDER_WIDTH | LV_STYLE_STATE_FOCUS, 6);




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
//    lv_style_copy(&btn_rel, &def);
//    btn_rel.glass               = 0;
//    btn_rel.body.opa            = LV_OPA_TRANSP;
//    btn_rel.body.radius         = LV_RADIUS_CIRCLE;
//    btn_rel.body.border.width   = 2;
//    btn_rel.body.border.color   = lv_color_hsv_to_rgb(_hue, 70, 90);
//    btn_rel.body.border.opa     = LV_OPA_80;
//    btn_rel.body.padding.left   = LV_DPI / 4;
//    btn_rel.body.padding.right  = LV_DPI / 4;
//    btn_rel.body.padding.top    = LV_DPI / 6;
//    btn_rel.body.padding.bottom = LV_DPI / 6;
//    btn_rel.body.padding.inner  = LV_DPI / 10;
//    btn_rel.text.color          = lv_color_hsv_to_rgb(_hue, 8, 96);
//    btn_rel.text.font           = _font;
//    btn_rel.image.color         = lv_color_hsv_to_rgb(_hue, 8, 96);
//
//    lv_style_copy(&btn_pr, &btn_rel);
//    btn_pr.body.opa        = LV_OPA_COVER;
//    btn_pr.body.main_color = lv_color_hsv_to_rgb(_hue, 50, 50);
//    btn_pr.body.grad_color = lv_color_hsv_to_rgb(_hue, 50, 50);
//    btn_pr.body.border.opa = LV_OPA_60;
//    btn_pr.text.font       = _font;
//    btn_pr.text.color      = lv_color_hsv_to_rgb(_hue, 10, 100);
//    btn_pr.image.color     = lv_color_hsv_to_rgb(_hue, 10, 100);
//
//    lv_style_copy(&btn_trel, &btn_pr);
//    btn_trel.body.opa          = LV_OPA_COVER;
//    btn_trel.body.main_color   = lv_color_hsv_to_rgb(_hue, 50, 60);
//    btn_trel.body.grad_color   = lv_color_hsv_to_rgb(_hue, 50, 60);
//    btn_trel.body.border.opa   = LV_OPA_60;
//    btn_trel.body.border.color = lv_color_hsv_to_rgb(_hue, 80, 90);
//    btn_trel.text.font         = _font;
//    btn_trel.text.color        = lv_color_hsv_to_rgb(_hue, 0, 100);
//    btn_trel.image.color       = lv_color_hsv_to_rgb(_hue, 0, 100);
//
//    lv_style_copy(&btn_tpr, &btn_trel);
//    btn_tpr.body.opa          = LV_OPA_COVER;
//    btn_tpr.body.main_color   = lv_color_hsv_to_rgb(_hue, 50, 50);
//    btn_tpr.body.grad_color   = lv_color_hsv_to_rgb(_hue, 50, 50);
//    btn_tpr.body.border.opa   = LV_OPA_60;
//    btn_tpr.body.border.color = lv_color_hsv_to_rgb(_hue, 80, 70);
//    btn_tpr.text.font         = _font;
//    btn_tpr.text.color        = lv_color_hsv_to_rgb(_hue, 10, 90);
//    btn_tpr.image.color       = lv_color_hsv_to_rgb(_hue, 10, 90);
//
//    lv_style_copy(&btn_ina, &btn_rel);
//    btn_ina.body.border.opa   = LV_OPA_60;
//    btn_ina.body.border.color = lv_color_hsv_to_rgb(_hue, 10, 50);
//    btn_ina.text.font         = _font;
//    btn_ina.text.color        = lv_color_hsv_to_rgb(_hue, 10, 90);
//
//    theme.style.btn.rel     = &btn_rel;
//    theme.style.btn.pr      = &btn_pr;
//    theme.style.btn.tgl_rel = &btn_trel;
//    theme.style.btn.tgl_pr  = &btn_tpr;
//    theme.style.btn.ina     = &btn_ina;
#endif
}

static void label_init(void)
{
#if LV_USE_LABEL != 0
//    static lv_style_t label_prim, label_sec, label_hint;
//
//    lv_style_copy(&label_prim, &def);
//    label_prim.text.font  = _font;
//    label_prim.text.color = lv_color_hsv_to_rgb(_hue, 80, 96);
//
//    lv_style_copy(&label_sec, &label_prim);
//    label_sec.text.color = lv_color_hsv_to_rgb(_hue, 40, 85);
//
//    lv_style_copy(&label_hint, &label_prim);
//    label_hint.text.color = lv_color_hsv_to_rgb(_hue, 20, 70);
//
//    theme.style.label.prim = &label_prim;
//    theme.style.label.sec  = &label_sec;
//    theme.style.label.hint = &label_hint;
#endif
}

static void bar_init(void)
{
#if LV_USE_BAR
    lv_style_init(&bar_indic);
    lv_style_set_color(&bar_indic, LV_STYLE_BG_COLOR, LV_COLOR_BLUE);
    lv_style_set_value(&bar_indic, LV_STYLE_PAD_TOP, LV_DPI/20);
    lv_style_set_value(&bar_indic, LV_STYLE_PAD_BOTTOM, LV_DPI/20);
    lv_style_set_value(&bar_indic, LV_STYLE_PAD_LEFT, LV_DPI/20);
    lv_style_set_value(&bar_indic, LV_STYLE_PAD_RIGHT, LV_DPI/20);
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
    static lv_style_t line_decor;
    lv_style_copy(&line_decor, &def);
    line_decor.line.color = lv_color_hsv_to_rgb(_hue, 50, 50);
    line_decor.line.width = 1;

    theme.style.line.decor = &line_decor;
#endif
}

static void led_init(void)
{
#if LV_USE_LED != 0
    static lv_style_t led;
    lv_style_copy(&led, &lv_style_pretty_color);
    led.body.shadow.width = LV_DPI / 10;
    led.body.radius       = LV_RADIUS_CIRCLE;
    led.body.border.width = LV_DPI / 30;
    led.body.border.opa   = LV_OPA_30;
    led.body.main_color   = lv_color_hsv_to_rgb(_hue, 100, 100);
    led.body.grad_color   = lv_color_hsv_to_rgb(_hue, 100, 40);
    led.body.border.color = lv_color_hsv_to_rgb(_hue, 60, 60);
    led.body.shadow.color = lv_color_hsv_to_rgb(_hue, 100, 100);

    theme.style.led = &led;
#endif
}

static void slider_init(void)
{
#if LV_USE_SLIDER != 0
    lv_style_init(&knob);
    lv_style_set_color(&knob, LV_STYLE_BG_COLOR, LV_COLOR_RED);
    lv_style_set_color(&knob, LV_STYLE_BG_COLOR | LV_STYLE_STATE_CHECKED, LV_COLOR_LIME);
    lv_style_set_value(&knob, LV_STYLE_PAD_TOP, LV_DPI/20);
    lv_style_set_value(&knob, LV_STYLE_PAD_BOTTOM, LV_DPI/20);
    lv_style_set_value(&knob, LV_STYLE_PAD_LEFT, LV_DPI/20);
    lv_style_set_value(&knob, LV_STYLE_PAD_RIGHT, LV_DPI/20);

#endif
}

static void sw_init(void)
{
#if LV_USE_SW != 0

#endif
}

static void lmeter_init(void)
{
#if LV_USE_LMETER != 0
    lv_style_copy(&lmeter_bg, &def);
    lmeter_bg.body.main_color    = lv_color_hsv_to_rgb(_hue, 10, 70);
    lmeter_bg.body.grad_color    = lv_color_hsv_to_rgb(_hue, 80, 80);
    lmeter_bg.body.padding.left  = LV_DPI / 8; /*Scale line length*/
    lmeter_bg.body.padding.right = LV_DPI / 8; /*Scale line length*/
    lmeter_bg.line.color         = lv_color_hex3(0x222);
    lmeter_bg.line.width         = 2;

    theme.style.lmeter = &lmeter_bg;

#endif
}

static void gauge_init(void)
{
#if LV_USE_GAUGE != 0
    static lv_style_t gauge_bg;
    lv_style_copy(&gauge_bg, &def);
    gauge_bg.body.main_color     = lv_color_hsv_to_rgb(_hue, 10, 70);
    gauge_bg.body.grad_color     = gauge_bg.body.main_color;
    gauge_bg.body.padding.left   = LV_DPI / 16; /*Scale line length*/
    gauge_bg.body.padding.right  = LV_DPI / 16; /*Scale line length*/
    gauge_bg.body.padding.top    = LV_DPI / 10; /*Needle center size*/
    gauge_bg.body.padding.bottom = LV_DPI / 10; /*Needle center size*/
    gauge_bg.body.padding.inner  = LV_DPI / 12; /*Label - scale distance*/
    gauge_bg.body.border.color   = lv_color_hex3(0x777);
    gauge_bg.line.color          = lv_color_hsv_to_rgb(_hue, 80, 75);
    gauge_bg.line.width          = 2;
    gauge_bg.text.color          = lv_color_hsv_to_rgb(_hue, 10, 90);
    gauge_bg.text.font           = _font;

    theme.style.gauge = &gauge_bg;
#endif
}

static void arc_init(void)
{
#if LV_USE_ARC != 0

    static lv_style_t arc;
    lv_style_copy(&arc, &def);
    arc.line.width   = 8;
    arc.line.color   = lv_color_hsv_to_rgb(_hue, 70, 90);
    arc.line.rounded = 1;

    /*For preloader*/
    arc.body.border.width   = 2;
    arc.body.border.color   = lv_color_hex3(0x555);
    arc.body.padding.left   = 3;
    arc.body.padding.right  = 3;
    arc.body.padding.top    = 3;
    arc.body.padding.bottom = 3;

    theme.style.arc = &arc;
#endif
}

static void preload_init(void)
{
#if LV_USE_PRELOAD != 0

    theme.style.preload = theme.style.arc;
#endif
}

static void chart_init(void)
{
#if LV_USE_CHART
    theme.style.chart = &panel;
#endif
}

static void calendar_init(void)
{
#if LV_USE_CALENDAR
    static lv_style_t header;
    static lv_style_t color_text;
    static lv_style_t gray_text;
    static lv_style_t today_box;

    lv_style_copy(&header, &def);
    header.body.radius         = 0;
    header.body.padding.left   = LV_DPI / 12;
    header.body.padding.right  = LV_DPI / 12;
    header.body.padding.top    = LV_DPI / 14;
    header.body.padding.bottom = LV_DPI / 14;
    header.body.main_color     = lv_color_hsv_to_rgb(_hue, 30, 60);
    header.body.grad_color     = header.body.main_color;
    header.body.border.opa     = panel.body.border.opa;
    header.body.border.width   = panel.body.border.width;
    header.body.border.color   = lv_color_hsv_to_rgb(_hue, 20, 80);
    header.text.color          = lv_color_hsv_to_rgb(_hue, 5, 100);

    lv_style_copy(&today_box, &header);
    today_box.body.main_color = lv_color_hsv_to_rgb(_hue, 40, 70);
    today_box.body.grad_color = today_box.body.main_color;
    today_box.body.opa        = LV_OPA_TRANSP;

    lv_style_copy(&color_text, &def);
    color_text.text.color = lv_color_hsv_to_rgb(_hue, 30, 80);

    lv_style_copy(&gray_text, &def);
    gray_text.text.color = lv_color_hsv_to_rgb(_hue, 10, 65);

    theme.style.calendar.bg               = &panel;
    theme.style.calendar.header           = &header;
    theme.style.calendar.week_box         = &header;
    theme.style.calendar.today_box        = &today_box;
    theme.style.calendar.day_names        = &color_text;
    theme.style.calendar.highlighted_days = &color_text;
    theme.style.calendar.inactive_days    = &gray_text;
#endif
}

static void cb_init(void)
{
#if LV_USE_CB != 0


#endif
}

static void btnm_init(void)
{
#if LV_USE_BTNM
//    lv_style_copy(&btnm_bg, &lv_style_transp_tight);
//    btnm_bg.body.border.width = 1;
//    btnm_bg.body.border.color = lv_color_hsv_to_rgb(_hue, 60, 80);
//    btnm_bg.body.border.opa   = LV_OPA_COVER;
//    btnm_bg.body.radius       = LV_DPI / 8;
//
//    lv_style_copy(&btnm_rel, &lv_style_plain);
//    btnm_rel.body.opa    = LV_OPA_TRANSP;
//    btnm_rel.body.radius = LV_DPI / 8;
//    btnm_rel.text.color  = lv_color_hsv_to_rgb(_hue, 60, 80);
//    btnm_rel.text.font   = _font;
//
//    lv_style_copy(&btnm_pr, &lv_style_plain);
//    btnm_pr.body.main_color = lv_color_hsv_to_rgb(_hue, 40, 70);
//    btnm_pr.body.grad_color = lv_color_hsv_to_rgb(_hue, 40, 70);
//    btnm_pr.body.radius     = LV_DPI / 8;
//    btnm_pr.text.color      = lv_color_hsv_to_rgb(_hue, 40, 40);
//    btnm_pr.text.font       = _font;
//
//    lv_style_copy(&btnm_trel, &btnm_rel);
//    btnm_trel.body.border.color = lv_color_hsv_to_rgb(_hue, 80, 80);
//    btnm_trel.body.border.width = 3;
//
//    lv_style_copy(&btnm_ina, &btnm_rel);
//    btnm_ina.text.color = lv_color_hsv_to_rgb(_hue, 10, 60);
//
//    theme.style.btnm.bg          = &btnm_bg;
//    theme.style.btnm.btn.rel     = &btnm_rel;
//    theme.style.btnm.btn.pr      = &btnm_pr;
//    theme.style.btnm.btn.tgl_rel = &btnm_trel;
//    theme.style.btnm.btn.tgl_pr  = &btnm_pr;
//    theme.style.btnm.btn.ina     = &btnm_ina;
#endif
}

static void kb_init(void)
{
#if LV_USE_KB
    theme.style.kb.bg          = &btnm_bg;
    theme.style.kb.btn.rel     = &btnm_rel;
    theme.style.kb.btn.pr      = &btnm_pr;
    theme.style.kb.btn.tgl_rel = &btnm_trel;
    theme.style.kb.btn.tgl_pr  = &btnm_pr;
    theme.style.kb.btn.ina     = &btnm_ina;
#endif
}

static void mbox_init(void)
{
#if LV_USE_MBOX
    static lv_style_t mbox_bg;
    lv_style_copy(&mbox_bg, &panel);
    mbox_bg.body.shadow.width = LV_DPI / 12;

    theme.style.mbox.bg      = &mbox_bg;
    theme.style.mbox.btn.bg  = &lv_style_transp;
    theme.style.mbox.btn.rel = &btn_trel;
    theme.style.mbox.btn.pr  = &btn_tpr;
#endif
}

static void page_init(void)
{
#if LV_USE_PAGE
//    theme.style.page.bg   = &panel;
//    theme.style.page.scrl = &lv_style_transp_fit;
//    theme.style.page.sb   = &sb;
#endif
}

static void ta_init(void)
{
#if LV_USE_TA
    theme.style.ta.area    = &panel;
    theme.style.ta.oneline = &panel;
    theme.style.ta.cursor  = NULL;
    theme.style.ta.sb      = &sb;
#endif
}

static void spinbox_init(void)
{
#if LV_USE_SPINBOX
    theme.style.spinbox.bg     = &panel;
    theme.style.spinbox.cursor = theme.style.ta.cursor;
    theme.style.spinbox.sb     = theme.style.ta.sb;
#endif
}

static void list_init(void)
{
#if LV_USE_LIST != 0
    static lv_style_t list_bg, list_rel, list_pr, list_trel, list_tpr, list_ina;
    lv_style_copy(&list_rel, &def);
    list_rel.body.opa          = LV_OPA_TRANSP;
    list_rel.body.border.width = 1;
    list_rel.body.border.color = lv_color_hsv_to_rgb(_hue, 50, 85);
    list_rel.body.border.opa   = LV_OPA_COVER;
    list_rel.text.color        = lv_color_hsv_to_rgb(_hue, 10, 94);
    list_rel.text.font         = _font;
    list_rel.image.color       = lv_color_hsv_to_rgb(_hue, 10, 94);

    lv_style_copy(&list_pr, &list_rel);
    list_pr.body.opa        = LV_OPA_COVER;
    list_pr.body.main_color = lv_color_hsv_to_rgb(_hue, 34, 41);
    list_pr.body.grad_color = lv_color_hsv_to_rgb(_hue, 34, 41);
    list_pr.text.color      = lv_color_hsv_to_rgb(_hue, 7, 96);
    list_pr.image.color     = lv_color_hsv_to_rgb(_hue, 7, 96);

    lv_style_copy(&list_trel, &list_rel);
    lv_style_copy(&list_tpr, &list_pr);
    lv_style_copy(&list_ina, &def);

    lv_style_copy(&list_bg, &list_rel);
    list_bg.body.padding.left   = 0;
    list_bg.body.padding.right  = 0;
    list_bg.body.padding.top    = 0;
    list_bg.body.padding.bottom = 0;

    theme.style.list.sb          = &sb;
    theme.style.list.bg          = &list_bg;
    theme.style.list.scrl        = &lv_style_transp_tight;
    theme.style.list.btn.rel     = &list_rel;
    theme.style.list.btn.pr      = &list_pr;
    theme.style.list.btn.tgl_rel = &list_trel;
    theme.style.list.btn.tgl_pr  = &list_tpr;
    theme.style.list.btn.ina     = &list_ina;
#endif
}

static void ddlist_init(void)
{
#if LV_USE_DDLIST != 0
    lv_style_copy(&ddlist_bg, &panel);
    ddlist_bg.text.line_space     = LV_DPI / 8;
    ddlist_bg.body.padding.left   = LV_DPI / 6;
    ddlist_bg.body.padding.right  = LV_DPI / 6;
    ddlist_bg.body.padding.top    = LV_DPI / 6;
    ddlist_bg.body.padding.bottom = LV_DPI / 6;

    lv_style_copy(&ddlist_sel, &panel);
    ddlist_sel.body.main_color = lv_color_hsv_to_rgb(_hue, 45, 70);
    ddlist_sel.body.grad_color = lv_color_hsv_to_rgb(_hue, 45, 70);
    ddlist_sel.body.opa        = LV_OPA_COVER;
    ddlist_sel.body.radius     = 0;

    theme.style.ddlist.bg  = &ddlist_bg;
    theme.style.ddlist.sel = &ddlist_sel;
    theme.style.ddlist.sb  = &sb;
#endif
}

static void roller_init(void)
{
#if LV_USE_ROLLER != 0
    static lv_style_t roller_bg, roller_sel;
    lv_style_copy(&roller_bg, &ddlist_bg);
    roller_bg.text.line_space   = LV_DPI / 6;
    roller_bg.body.radius       = LV_DPI / 20;
    roller_bg.body.main_color   = lv_color_hex3(0x222);
    roller_bg.body.grad_color   = lv_color_hex3(0x666);
    roller_bg.body.border.opa   = LV_OPA_30;
    roller_bg.text.opa          = LV_OPA_70;
    roller_bg.text.color        = lv_color_hsv_to_rgb(_hue, 20, 70);
    roller_bg.body.shadow.width = 0;

    lv_style_copy(&roller_sel, &panel);
    roller_sel.body.opa    = LV_OPA_TRANSP;
    roller_sel.body.radius = 0;
    roller_sel.text.opa    = LV_OPA_COVER;
    roller_sel.text.color  = lv_color_hsv_to_rgb(_hue, 70, 95);

    theme.style.roller.bg  = &roller_bg;
    theme.style.roller.sel = &roller_sel;
#endif
}

static void tabview_init(void)
{
#if LV_USE_TABVIEW != 0
    static lv_style_t tab_rel, tab_pr, tab_trel, tab_tpr, tab_indic;
    lv_style_copy(&tab_rel, &def);
    tab_rel.body.main_color     = lv_color_hex3(0x666);
    tab_rel.body.grad_color     = lv_color_hex3(0x666);
    tab_rel.body.padding.left   = 0;
    tab_rel.body.padding.right  = 0;
    tab_rel.body.padding.top    = LV_DPI / 6;
    tab_rel.body.padding.bottom = LV_DPI / 6;
    tab_rel.body.padding.inner  = 0;
    tab_rel.body.border.width   = 1;
    tab_rel.body.border.color   = LV_COLOR_SILVER;
    tab_rel.body.border.opa     = LV_OPA_40;
    tab_rel.text.color          = lv_color_hex3(0xDDD);
    tab_rel.text.font           = _font;

    lv_style_copy(&tab_pr, &tab_rel);
    tab_pr.body.main_color = lv_color_hex3(0x444);
    tab_pr.body.grad_color = lv_color_hex3(0x444);

    lv_style_copy(&tab_trel, &def);
    tab_trel.body.opa            = LV_OPA_TRANSP;
    tab_trel.body.padding.left   = 0;
    tab_trel.body.padding.right  = 0;
    tab_trel.body.padding.top    = LV_DPI / 6;
    tab_trel.body.padding.bottom = LV_DPI / 6;
    tab_trel.body.padding.inner  = 0;
    tab_trel.body.border.width   = 1;
    tab_trel.body.border.color   = LV_COLOR_SILVER;
    tab_trel.body.border.opa     = LV_OPA_40;
    tab_trel.text.color          = lv_color_hsv_to_rgb(_hue, 10, 94);
    tab_trel.text.font           = _font;

    lv_style_copy(&tab_tpr, &def);
    tab_tpr.body.main_color     = LV_COLOR_GRAY;
    tab_tpr.body.grad_color     = LV_COLOR_GRAY;
    tab_tpr.body.padding.left   = 0;
    tab_tpr.body.padding.right  = 0;
    tab_tpr.body.padding.top    = LV_DPI / 6;
    tab_tpr.body.padding.bottom = LV_DPI / 6;
    tab_tpr.body.padding.inner  = 0;
    tab_tpr.body.border.width   = 1;
    tab_tpr.body.border.color   = LV_COLOR_SILVER;
    tab_tpr.body.border.opa     = LV_OPA_40;
    tab_tpr.text.color          = lv_color_hsv_to_rgb(_hue, 10, 94);
    tab_tpr.text.font           = _font;

    lv_style_copy(&tab_indic, &def);
    tab_indic.body.border.width  = 0;
    tab_indic.body.main_color    = lv_color_hsv_to_rgb(_hue, 80, 87);
    tab_indic.body.grad_color    = lv_color_hsv_to_rgb(_hue, 80, 87);
    tab_indic.body.padding.inner = LV_DPI / 10; /*Indicator height*/

    theme.style.tabview.bg          = &bg;
    theme.style.tabview.indic       = &tab_indic;
    theme.style.tabview.btn.bg      = &lv_style_transp_tight;
    theme.style.tabview.btn.rel     = &tab_rel;
    theme.style.tabview.btn.pr      = &tab_pr;
    theme.style.tabview.btn.tgl_rel = &tab_trel;
    theme.style.tabview.btn.tgl_pr  = &tab_tpr;
#endif
}

static void tileview_init(void)
{
#if LV_USE_TILEVIEW != 0
    theme.style.tileview.bg   = &lv_style_transp_tight;
    theme.style.tileview.scrl = &lv_style_transp_tight;
    theme.style.tileview.sb   = theme.style.page.sb;
#endif
}

static void table_init(void)
{
#if LV_USE_TABLE != 0
    static lv_style_t cell;
    lv_style_copy(&cell, &panel);
    cell.body.radius         = 0;
    cell.body.border.width   = 1;
    cell.body.padding.left   = LV_DPI / 12;
    cell.body.padding.right  = LV_DPI / 12;
    cell.body.padding.top    = LV_DPI / 12;
    cell.body.padding.bottom = LV_DPI / 12;

    theme.style.table.bg   = &lv_style_transp_tight;
    theme.style.table.cell = &cell;
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

lv_style_t * lv_theme_alien_get_style(lv_theme_style_t name)
{
    switch(name) {
    case LV_THEME_SCR:
        return &scr;
    case LV_THEME_SCR_TRANSP:
        return &transp;
    case LV_THEME_PANEL:
        return &panel;
    case LV_THEME_BTN:
        return &btn;
    case LV_THEME_BTNM:
        return &panel;
    case LV_THEME_BTNM_BTN:
        return &btn;
    case LV_THEME_BAR:
        return &panel;
    case LV_THEME_BAR_INDIC:
        return &bar_indic;
    case LV_THEME_SLIDER:
        return &panel;
    case LV_THEME_SLIDER_INDIC:
        return &bar_indic;
    case LV_THEME_SLIDER_KNOB:
        return &knob;
    case LV_THEME_CB:
           return &panel;
    case LV_THEME_CB_BULLET:
        return &btn;
    }

    return NULL;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif
