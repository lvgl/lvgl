/**
 * @file lv_theme_material.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_theme.h"

#if LV_USE_THEME_MATERIAL

/*********************
 *      DEFINES
 *********************/
#define DEF_RADIUS 4
#define DEF_SHADOW_COLOR lv_color_hex3(0xaaa)

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
static lv_style_t def;

/*Static style definitions*/
static lv_style_t sb;

/*Saved input parameters*/
static uint16_t _hue;
static lv_font_t * _font;

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void basic_init(void)
{
    static lv_style_t bg, panel, scr;

    lv_style_copy(&def, &lv_style_plain); /*Initialize the default style*/
    def.text.font   = _font;
    def.body.radius = DEF_RADIUS;

    lv_style_copy(&bg, &def);
    bg.body.main_color = lv_color_hex(0xf0f0f0);
    bg.body.grad_color = bg.body.main_color;
    bg.body.radius     = 0;

    lv_style_copy(&scr, &bg);
    scr.body.padding.bottom = 0;
    scr.body.padding.top    = 0;
    scr.body.padding.left   = 0;
    scr.body.padding.right  = 0;

    lv_style_copy(&panel, &def);
    panel.body.radius         = DEF_RADIUS;
    panel.body.main_color     = LV_COLOR_WHITE;
    panel.body.grad_color     = LV_COLOR_WHITE;
    panel.body.border.width   = 1;
    panel.body.border.color   = lv_color_hex3(0xbbb);
    panel.body.border.opa     = LV_OPA_COVER;
    panel.body.shadow.color   = DEF_SHADOW_COLOR;
    panel.body.shadow.type    = LV_SHADOW_BOTTOM;
    panel.body.shadow.width   = 4;
    panel.body.padding.left   = LV_DPI / 8;
    panel.body.padding.right  = LV_DPI / 8;
    panel.body.padding.top    = LV_DPI / 8;
    panel.body.padding.bottom = LV_DPI / 8;
    panel.body.padding.inner  = LV_DPI / 12;
    panel.text.color          = lv_color_hex3(0x333);
    panel.image.color         = lv_color_hex3(0x333);

    lv_style_copy(&sb, &def);
    sb.body.main_color     = LV_COLOR_BLACK;
    sb.body.grad_color     = LV_COLOR_BLACK;
    sb.body.opa            = LV_OPA_40;
    sb.body.padding.right  = LV_DPI / 25;
    sb.body.padding.bottom = LV_DPI / 25;

    theme.style.bg    = &bg;
    theme.style.scr   = &scr;
    theme.style.panel = &panel;
}

static void cont_init(void)
{
#if LV_USE_CONT != 0

    theme.style.cont = theme.style.panel;
#endif
}

static void btn_init(void)
{
#if LV_USE_BTN != 0
    static lv_style_t rel, pr, tgl_rel, tgl_pr, ina;

    lv_style_copy(&rel, &def);
    rel.body.main_color     = lv_color_hsv_to_rgb(_hue, 90, 70);
    rel.body.grad_color     = rel.body.main_color;
    rel.body.radius         = DEF_RADIUS;
    rel.body.padding.left   = LV_DPI / 6;
    rel.body.padding.right  = LV_DPI / 6;
    rel.body.padding.top    = LV_DPI / 8;
    rel.body.padding.bottom = LV_DPI / 8;
    rel.body.padding.inner  = LV_DPI / 10;
    rel.body.shadow.color   = DEF_SHADOW_COLOR;
    rel.body.shadow.type    = LV_SHADOW_BOTTOM;
    rel.body.shadow.width   = 6;
    rel.text.color          = lv_color_hsv_to_rgb(_hue, 5, 95);
    rel.image.color         = lv_color_hsv_to_rgb(_hue, 5, 95);

    lv_style_copy(&pr, &rel);
    pr.body.main_color   = lv_color_hsv_to_rgb(_hue, 90, 60);
    pr.body.grad_color   = pr.body.main_color;
    pr.body.shadow.width = 4;

    lv_style_copy(&tgl_rel, &rel);
    tgl_rel.body.main_color   = lv_color_hsv_to_rgb(_hue, 95, 50);
    tgl_rel.body.grad_color   = tgl_rel.body.main_color;
    tgl_rel.body.shadow.width = 4;

    lv_style_copy(&tgl_pr, &tgl_rel);
    tgl_pr.body.main_color   = lv_color_hsv_to_rgb(_hue, 95, 40);
    tgl_pr.body.grad_color   = tgl_pr.body.main_color;
    tgl_pr.body.shadow.width = 2;

    lv_style_copy(&ina, &rel);
    ina.body.main_color   = lv_color_hex3(0xccc);
    ina.body.grad_color   = ina.body.main_color;
    ina.body.shadow.width = 0;
    ina.text.color        = lv_color_hsv_to_rgb(_hue, 95, 5);
    ina.image.color       = lv_color_hsv_to_rgb(_hue, 95, 5);

    theme.style.btn.rel     = &rel;
    theme.style.btn.pr      = &pr;
    theme.style.btn.tgl_rel = &tgl_rel;
    theme.style.btn.tgl_pr  = &tgl_pr;
    theme.style.btn.ina     = &ina;
#endif
}

static void label_init(void)
{
#if LV_USE_LABEL != 0
    static lv_style_t prim, sec, hint;

    lv_style_copy(&prim, &def);
    prim.text.font  = _font;
    prim.text.color = lv_color_hsv_to_rgb(_hue, 80, 10);

    lv_style_copy(&sec, &prim);
    sec.text.color = lv_color_hsv_to_rgb(_hue, 80, 75);

    lv_style_copy(&hint, &prim);
    hint.text.color = lv_color_hsv_to_rgb(_hue, 40, 90);

    theme.style.label.prim = &prim;
    theme.style.label.sec  = &sec;
    theme.style.label.hint = &hint;
#endif
}

static void img_init(void)
{
#if LV_USE_IMG != 0
    static lv_style_t img_light, img_dark;
    lv_style_copy(&img_light, &def);
    img_light.image.color   = lv_color_hsv_to_rgb(_hue, 15, 85);
    img_light.image.intense = LV_OPA_80;

    lv_style_copy(&img_dark, &def);
    img_light.image.color   = lv_color_hsv_to_rgb(_hue, 85, 65);
    img_light.image.intense = LV_OPA_80;

    theme.style.img.light = &def;
    theme.style.img.dark  = &def;
#endif
}

static void line_init(void)
{
#if LV_USE_LINE != 0

    theme.style.line.decor = &def;
#endif
}

static void led_init(void)
{
#if LV_USE_LED != 0
    static lv_style_t led;
    lv_style_copy(&led, &def);
    led.body.shadow.width = LV_DPI / 10;
    led.body.radius       = LV_RADIUS_CIRCLE;
    led.body.border.width = LV_DPI / 30;
    led.body.border.opa   = LV_OPA_30;
    led.body.main_color   = lv_color_hsv_to_rgb(_hue, 100, 100);
    led.body.grad_color   = lv_color_hsv_to_rgb(_hue, 100, 100);
    led.body.border.color = lv_color_hsv_to_rgb(_hue, 60, 60);
    led.body.shadow.color = lv_color_hsv_to_rgb(_hue, 100, 100);

    theme.style.led = &led;
#endif
}

static void bar_init(void)
{
#if LV_USE_BAR
    static lv_style_t bar_bg, bar_indic;

    lv_style_copy(&bar_bg, &def);
    bar_bg.body.main_color     = lv_color_hsv_to_rgb(_hue, 15, 95);
    bar_bg.body.grad_color     = bar_bg.body.main_color;
    bar_bg.body.radius         = 3;
    bar_bg.body.border.width   = 0;
    bar_bg.body.padding.left   = LV_DPI / 16;
    bar_bg.body.padding.right  = LV_DPI / 16;
    bar_bg.body.padding.top    = LV_DPI / 16;
    bar_bg.body.padding.bottom = LV_DPI / 16;

    lv_style_copy(&bar_indic, &bar_bg);
    bar_indic.body.main_color     = lv_color_hsv_to_rgb(_hue, 85, 70);
    bar_indic.body.grad_color     = bar_indic.body.main_color;
    bar_indic.body.padding.left   = 0;
    bar_indic.body.padding.right  = 0;
    bar_indic.body.padding.top    = 0;
    bar_indic.body.padding.bottom = 0;

    theme.style.bar.bg    = &bar_bg;
    theme.style.bar.indic = &bar_indic;
#endif
}

static void slider_init(void)
{
#if LV_USE_SLIDER != 0
    static lv_style_t knob;

    lv_style_copy(&knob, &def);
    knob.body.radius       = LV_RADIUS_CIRCLE;
    knob.body.border.width = 0;
    knob.body.main_color   = theme.style.bar.indic->body.main_color;
    knob.body.grad_color   = knob.body.main_color;

    theme.style.slider.bg    = theme.style.bar.bg;
    theme.style.slider.indic = theme.style.bar.indic;
    theme.style.slider.knob  = &knob;
#endif
}

static void sw_init(void)
{
#if LV_USE_SW != 0
    static lv_style_t sw_bg, sw_indic, sw_knob_off, sw_knob_on;
    lv_style_copy(&sw_bg, theme.style.slider.bg);
    sw_bg.body.radius = LV_RADIUS_CIRCLE;

    lv_style_copy(&sw_indic, theme.style.slider.bg);
    sw_indic.body.radius = LV_RADIUS_CIRCLE;

    lv_style_copy(&sw_knob_on, theme.style.slider.knob);
    sw_knob_on.body.shadow.width = 3;
    sw_knob_on.body.shadow.type  = LV_SHADOW_BOTTOM;
    sw_knob_on.body.shadow.color = DEF_SHADOW_COLOR;

    lv_style_copy(&sw_knob_off, &sw_knob_on);
    sw_knob_off.body.main_color   = lv_color_hex(0xfafafa);
    sw_knob_off.body.grad_color   = sw_knob_off.body.main_color;
    sw_knob_off.body.border.width = 1;
    sw_knob_off.body.border.color = lv_color_hex3(0x999);
    sw_knob_off.body.border.opa   = LV_OPA_COVER;

    theme.style.sw.bg       = &sw_bg;
    theme.style.sw.indic    = &sw_indic;
    theme.style.sw.knob_off = &sw_knob_off;
    theme.style.sw.knob_on  = &sw_knob_on;
#endif
}

static void lmeter_init(void)
{
#if LV_USE_LMETER != 0
    static lv_style_t lmeter;
    lv_style_copy(&lmeter, &def);
    lmeter.body.main_color   = lv_color_hsv_to_rgb(_hue, 75, 90);
    lmeter.body.grad_color   = lmeter.body.main_color;
    lmeter.body.padding.left = LV_DPI / 10; /*Scale line length*/
    lmeter.line.color        = lv_color_hex3(0x999);
    lmeter.line.width        = 2;

    theme.style.lmeter = &lmeter;
#endif
}

static void gauge_init(void)
{
#if LV_USE_GAUGE != 0

    static lv_style_t gauge;
    lv_style_copy(&gauge, &def);
    gauge.body.main_color    = lv_color_hsv_to_rgb(_hue, 10, 60);
    gauge.body.grad_color    = gauge.body.main_color;
    gauge.body.padding.left  = LV_DPI / 16; /*Scale line length*/
    gauge.body.padding.inner = LV_DPI / 8;
    gauge.body.border.color  = lv_color_hex3(0x999);
    gauge.text.color         = lv_color_hex3(0x333);
    gauge.line.width         = 3;
    gauge.line.color         = lv_color_hsv_to_rgb(_hue, 95, 70);

    theme.style.gauge = &gauge;
#endif
}

static void arc_init(void)
{
#if LV_USE_ARC != 0

    static lv_style_t arc;
    lv_style_copy(&arc, &def);
    arc.line.width = 10;
    arc.line.color = lv_color_hsv_to_rgb(_hue, 90, 90);

    /*For prelaoder*/
    arc.body.border.width   = 10;
    arc.body.border.color   = lv_color_hsv_to_rgb(_hue, 30, 90);
    arc.body.padding.left   = 0;
    arc.body.padding.right  = 0;
    arc.body.padding.top    = 0;
    arc.body.padding.bottom = 0;

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
    theme.style.chart = theme.style.panel;
#endif
}

static void calendar_init(void)
{
#if LV_USE_CALENDAR
    static lv_style_t ina_days;
    lv_style_copy(&ina_days, &def);
    ina_days.text.color = lv_color_hsv_to_rgb(_hue, 0, 70);

    static lv_style_t high_days;
    lv_style_copy(&high_days, &def);
    high_days.text.color = lv_color_hsv_to_rgb(_hue, 80, 90);

    static lv_style_t week_box;
    lv_style_copy(&week_box, &def);
    week_box.body.main_color     = lv_color_hsv_to_rgb(_hue, 40, 100);
    week_box.body.grad_color     = lv_color_hsv_to_rgb(_hue, 40, 100);
    week_box.body.padding.top    = LV_DPI / 20;
    week_box.body.padding.bottom = LV_DPI / 20;
    week_box.body.padding.left   = theme.style.panel->body.padding.left;
    week_box.body.padding.right  = theme.style.panel->body.padding.right;
    week_box.body.border.color   = theme.style.panel->body.border.color;
    week_box.body.border.width   = theme.style.panel->body.border.width;
    week_box.body.border.part    = LV_BORDER_LEFT | LV_BORDER_RIGHT;
    week_box.body.radius         = 0;

    static lv_style_t today_box;
    lv_style_copy(&today_box, &def);
    today_box.body.main_color     = LV_COLOR_WHITE;
    today_box.body.grad_color     = LV_COLOR_WHITE;
    today_box.body.padding.top    = LV_DPI / 20;
    today_box.body.padding.bottom = LV_DPI / 20;
    today_box.body.radius         = 0;

    theme.style.calendar.bg               = theme.style.panel;
    theme.style.calendar.header           = &lv_style_transp;
    theme.style.calendar.inactive_days    = &ina_days;
    theme.style.calendar.highlighted_days = &high_days;
    theme.style.calendar.week_box         = &week_box;
    theme.style.calendar.today_box        = &today_box;
#endif
}

static void cb_init(void)
{
#if LV_USE_CB != 0
    static lv_style_t rel, pr, tgl_rel, tgl_pr, ina;
    lv_style_copy(&rel, theme.style.panel);
    rel.body.shadow.type  = LV_SHADOW_BOTTOM;
    rel.body.shadow.width = 3;

    lv_style_copy(&pr, &rel);
    pr.body.main_color   = lv_color_hex3(0xccc);
    pr.body.grad_color   = pr.body.main_color;
    pr.body.shadow.width = 0;

    lv_style_copy(&tgl_rel, &rel);
    tgl_rel.body.main_color   = lv_color_hsv_to_rgb(_hue, 75, 85);
    tgl_rel.body.grad_color   = tgl_rel.body.main_color;
    tgl_rel.body.shadow.type  = LV_SHADOW_FULL;
    tgl_rel.body.shadow.width = 0;

    lv_style_copy(&tgl_pr, &tgl_rel);
    tgl_pr.body.main_color   = lv_color_hsv_to_rgb(_hue, 75, 65);
    tgl_pr.body.grad_color   = tgl_pr.body.main_color;
    tgl_pr.body.shadow.width = 0;

    lv_style_copy(&ina, theme.style.btn.ina);

    theme.style.cb.bg          = &lv_style_transp;
    theme.style.cb.box.rel     = &rel;
    theme.style.cb.box.pr      = &pr;
    theme.style.cb.box.tgl_rel = &tgl_rel;
    theme.style.cb.box.tgl_pr  = &tgl_pr;
    theme.style.cb.box.ina     = &ina;
#endif
}

static void btnm_init(void)
{
#if LV_USE_BTNM
    static lv_style_t bg, rel, pr, tgl_rel, tgl_pr, ina;

    lv_style_copy(&bg, theme.style.panel);
    bg.body.padding.left   = 0;
    bg.body.padding.right  = 0;
    bg.body.padding.top    = 0;
    bg.body.padding.bottom = 0;
    bg.body.padding.inner  = 0;
    bg.text.color          = lv_color_hex3(0x555);

    lv_style_copy(&rel, theme.style.panel);
    rel.body.border.part  = LV_BORDER_FULL | LV_BORDER_INTERNAL;
    rel.body.border.width = 1;
    rel.body.border.color = lv_color_hex3(0xbbb);
    rel.body.opa          = LV_OPA_TRANSP;
    rel.body.shadow.width = 0;

    lv_style_copy(&pr, &rel);
    pr.glass             = 0;
    pr.body.main_color   = lv_color_hex3(0xddd);
    pr.body.grad_color   = pr.body.main_color;
    pr.body.border.width = 0;
    pr.body.opa          = LV_OPA_COVER;

    lv_style_copy(&tgl_rel, &pr);
    tgl_rel.body.main_color = lv_color_hsv_to_rgb(_hue, 90, 70);
    tgl_rel.body.grad_color = tgl_rel.body.main_color;
    tgl_rel.text.color      = lv_color_hsv_to_rgb(_hue, 5, 95);

    lv_style_copy(&tgl_pr, &tgl_rel);
    tgl_pr.body.main_color   = lv_color_hsv_to_rgb(_hue, 95, 65);
    tgl_pr.body.grad_color   = tgl_pr.body.main_color;
    tgl_pr.body.border.width = 0;

    lv_style_copy(&ina, &pr);
    ina.body.main_color = lv_color_hex3(0xccc);
    ina.body.grad_color = ina.body.main_color;

    theme.style.btnm.bg          = &bg;
    theme.style.btnm.btn.rel     = &rel;
    theme.style.btnm.btn.pr      = &pr;
    theme.style.btnm.btn.tgl_rel = &tgl_rel;
    theme.style.btnm.btn.tgl_pr  = &tgl_pr;
    theme.style.btnm.btn.ina     = &def;
#endif
}

static void kb_init(void)
{
#if LV_USE_KB

    static lv_style_t rel;
    lv_style_copy(&rel, &lv_style_transp);
    rel.text.font = _font;

    theme.style.kb.bg          = theme.style.btnm.bg;
    theme.style.kb.btn.rel     = &rel;
    theme.style.kb.btn.pr      = theme.style.btnm.btn.pr;
    theme.style.kb.btn.tgl_rel = theme.style.btnm.btn.tgl_rel;
    theme.style.kb.btn.tgl_pr  = theme.style.btnm.btn.tgl_pr;
    theme.style.kb.btn.ina     = theme.style.btnm.btn.ina;
#endif
}

static void mbox_init(void)
{
#if LV_USE_MBOX
    static lv_style_t pr, rel;

    lv_style_copy(&rel, &lv_style_transp);
    rel.glass      = 0;
    rel.text.font  = _font;
    rel.text.color = lv_color_hsv_to_rgb(_hue, 85, 75);

    lv_style_copy(&pr, theme.style.btnm.btn.pr);
    pr.text.color = lv_color_hsv_to_rgb(_hue, 85, 60);

    theme.style.mbox.bg      = theme.style.panel;
    theme.style.mbox.btn.bg  = &lv_style_transp;
    theme.style.mbox.btn.rel = &rel;
    theme.style.mbox.btn.pr  = &pr;
#endif
}

static void page_init(void)
{
#if LV_USE_PAGE

    theme.style.page.bg   = theme.style.panel;
    theme.style.page.scrl = &lv_style_transp;
    theme.style.page.sb   = &sb;
#endif
}

static void ta_init(void)
{
#if LV_USE_TA
    static lv_style_t oneline;

    lv_style_copy(&oneline, &def);
    oneline.body.opa          = LV_OPA_TRANSP;
    oneline.body.radius       = 0;
    oneline.body.border.part  = LV_BORDER_BOTTOM;
    oneline.body.border.width = 3;
    oneline.body.border.color = lv_color_hex3(0x333);
    oneline.body.border.opa   = LV_OPA_COVER;
    oneline.text.color        = lv_color_hex3(0x333);

    theme.style.ta.area    = theme.style.panel;
    theme.style.ta.oneline = &oneline;
    theme.style.ta.cursor  = NULL; /*Let library to calculate the cursor's style*/
    theme.style.ta.sb      = &sb;
#endif
}

static void spinbox_init(void)
{
#if LV_USE_SPINBOX
    theme.style.spinbox.bg     = theme.style.panel;
    theme.style.spinbox.cursor = theme.style.ta.cursor;
    theme.style.spinbox.sb     = theme.style.ta.sb;
#endif
}

static void list_init(void)
{
#if LV_USE_LIST != 0

    static lv_style_t list_bg, rel, pr, tgl_rel, tgl_pr, ina;

    lv_style_copy(&list_bg, theme.style.panel);
    list_bg.body.padding.left   = 0;
    list_bg.body.padding.right  = 0;
    list_bg.body.padding.top    = 0;
    list_bg.body.padding.bottom = 0;
    list_bg.body.padding.inner  = 0;

    lv_style_copy(&rel, &lv_style_transp);
    rel.body.padding.left   = LV_DPI / 8;
    rel.body.padding.right  = LV_DPI / 8;
    rel.body.padding.top    = LV_DPI / 6;
    rel.body.padding.bottom = LV_DPI / 6;
    rel.body.radius         = 10;
    rel.body.border.color   = lv_color_hex3(0xbbb);
    rel.body.border.width   = 1;
    rel.body.border.part    = LV_BORDER_BOTTOM;

    lv_style_copy(&pr, &rel);
    pr.glass             = 0;
    pr.body.main_color   = lv_color_hex3(0xddd);
    pr.body.grad_color   = pr.body.main_color;
    pr.body.border.width = 0;
    pr.body.opa          = LV_OPA_COVER;
    pr.body.radius       = DEF_RADIUS;
    pr.text.font         = _font;

    lv_style_copy(&tgl_rel, &pr);
    tgl_rel.body.main_color = lv_color_hsv_to_rgb(_hue, 90, 70);
    tgl_rel.body.grad_color = tgl_rel.body.main_color;
    tgl_rel.text.color      = lv_color_hsv_to_rgb(_hue, 5, 95);

    lv_style_copy(&tgl_pr, &tgl_rel);
    tgl_pr.body.main_color   = lv_color_hsv_to_rgb(_hue, 90, 60);
    tgl_pr.body.grad_color   = tgl_pr.body.main_color;
    tgl_pr.body.border.width = 0;

    lv_style_copy(&ina, &pr);
    ina.body.main_color = lv_color_hex3(0xccc);
    ina.body.grad_color = ina.body.main_color;

    theme.style.list.sb          = &sb;
    theme.style.list.bg          = &list_bg;
    theme.style.list.scrl        = &lv_style_transp_tight;
    theme.style.list.btn.rel     = &rel;
    theme.style.list.btn.pr      = &pr;
    theme.style.list.btn.tgl_rel = &tgl_rel;
    theme.style.list.btn.tgl_pr  = &tgl_pr;
    theme.style.list.btn.ina     = &ina;
#endif
}

static void ddlist_init(void)
{
#if LV_USE_DDLIST != 0
    static lv_style_t bg, sel;
    lv_style_copy(&bg, theme.style.panel);
    bg.body.padding.left   = LV_DPI / 6;
    bg.body.padding.right  = LV_DPI / 6;
    bg.body.padding.top    = LV_DPI / 6;
    bg.body.padding.bottom = LV_DPI / 6;
    bg.text.line_space     = LV_DPI / 8;

    lv_style_copy(&sel, &bg);
    sel.body.main_color   = lv_color_hsv_to_rgb(_hue, 90, 70);
    sel.body.grad_color   = sel.body.main_color;
    sel.body.border.width = 0;
    sel.body.shadow.width = 0;
    sel.text.color        = lv_color_hsv_to_rgb(_hue, 5, 95);

    theme.style.ddlist.bg  = &bg;
    theme.style.ddlist.sel = &sel;
    theme.style.ddlist.sb  = &sb;
#endif
}

static void roller_init(void)
{
#if LV_USE_ROLLER != 0
    static lv_style_t roller_bg, roller_sel;

    lv_style_copy(&roller_bg, &lv_style_transp);
    roller_bg.body.padding.left   = LV_DPI / 6;
    roller_bg.body.padding.right  = LV_DPI / 6;
    roller_bg.body.padding.top    = LV_DPI / 6;
    roller_bg.body.padding.bottom = LV_DPI / 6;
    roller_bg.text.line_space     = LV_DPI / 8;
    roller_bg.text.font           = _font;
    roller_bg.glass               = 0;

    lv_style_copy(&roller_sel, &roller_bg);
    roller_sel.text.color = lv_color_hsv_to_rgb(_hue, 90, 70);

    theme.style.roller.bg  = &roller_bg;
    theme.style.roller.sel = &roller_sel;
#endif
}

static void tabview_init(void)
{
#if LV_USE_TABVIEW != 0
    static lv_style_t indic, btn_bg, rel, pr, tgl_rel, tgl_pr;

    lv_style_copy(&indic, &def);
    indic.body.main_color    = lv_color_hsv_to_rgb(_hue, 90, 70);
    indic.body.grad_color    = indic.body.main_color;
    indic.body.radius        = 0;
    indic.body.border.width  = 0;
    indic.body.padding.inner = LV_DPI / 20;

    lv_style_copy(&btn_bg, &def);
    btn_bg.body.main_color     = lv_color_hex3(0xccc);
    btn_bg.body.grad_color     = btn_bg.body.main_color;
    btn_bg.body.radius         = 0;
    btn_bg.body.border.width   = 1;
    btn_bg.body.border.color   = lv_color_hex3(0x888);
    btn_bg.body.border.part    = LV_BORDER_BOTTOM;
    btn_bg.body.border.opa     = LV_OPA_COVER;
    btn_bg.body.shadow.width   = 5;
    btn_bg.body.shadow.color   = DEF_SHADOW_COLOR;
    btn_bg.body.shadow.type    = LV_SHADOW_BOTTOM;
    btn_bg.body.padding.inner  = 0;
    btn_bg.body.padding.left   = 0;
    btn_bg.body.padding.right  = 0;
    btn_bg.body.padding.top    = 0;
    btn_bg.body.padding.bottom = 0;
    btn_bg.text.color          = lv_color_hex3(0x333);

    lv_style_copy(&rel, &lv_style_transp);
    rel.body.padding.top    = LV_DPI / 8;
    rel.body.padding.bottom = LV_DPI / 8;
    rel.text.font           = _font;

    lv_style_copy(&pr, &def);
    pr.body.main_color   = lv_color_hex3(0xbbb);
    pr.body.grad_color   = pr.body.main_color;
    pr.body.border.width = 0;
    pr.body.opa          = LV_OPA_COVER;
    pr.body.radius       = 0;
    pr.body.border.width = 1;
    pr.body.border.color = lv_color_hex3(0x888);
    pr.body.border.part  = LV_BORDER_BOTTOM;
    pr.body.border.opa   = LV_OPA_COVER;
    pr.text.color        = lv_color_hex3(0x111);

    lv_style_copy(&tgl_rel, &lv_style_transp);
    tgl_rel.glass      = 0;
    tgl_rel.text.font  = _font;
    tgl_rel.text.color = lv_color_hsv_to_rgb(_hue, 90, 70);

    lv_style_copy(&tgl_pr, &def);
    tgl_pr.body.main_color   = lv_color_hsv_to_rgb(_hue, 15, 85);
    tgl_pr.body.grad_color   = tgl_pr.body.main_color;
    tgl_pr.body.border.width = 0;
    tgl_pr.body.opa          = LV_OPA_COVER;
    tgl_pr.body.radius       = 0;
    tgl_pr.text.color        = lv_color_hsv_to_rgb(_hue, 90, 60);

    theme.style.tabview.bg          = theme.style.bg;
    theme.style.tabview.indic       = &indic;
    theme.style.tabview.btn.bg      = &btn_bg;
    theme.style.tabview.btn.rel     = &rel;
    theme.style.tabview.btn.pr      = &pr;
    theme.style.tabview.btn.tgl_rel = &tgl_rel;
    theme.style.tabview.btn.tgl_pr  = &tgl_pr;
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
    lv_style_copy(&cell, theme.style.panel);
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
    static lv_style_t header, pr;

    lv_style_copy(&header, &def);
    header.body.main_color     = lv_color_hex3(0xccc);
    header.body.grad_color     = header.body.main_color;
    header.body.radius         = 0;
    header.body.border.width   = 1;
    header.body.border.color   = lv_color_hex3(0xbbb);
    header.body.border.part    = LV_BORDER_BOTTOM;
    header.body.border.opa     = LV_OPA_COVER;
    header.body.padding.inner  = 0;
    header.body.padding.left   = 0;
    header.body.padding.right  = 0;
    header.body.padding.top    = 0;
    header.body.padding.bottom = 0;
    header.text.color          = lv_color_hex3(0x333);
    header.image.color         = lv_color_hex3(0x333);

    lv_style_copy(&pr, &def);
    pr.body.main_color   = lv_color_hex3(0xbbb);
    pr.body.grad_color   = pr.body.main_color;
    pr.body.border.width = 0;
    pr.body.opa          = LV_OPA_COVER;
    pr.body.radius       = 0;
    pr.text.color        = lv_color_hex3(0x111);
    pr.image.color       = lv_color_hex3(0x111);

    theme.style.win.bg      = theme.style.panel;
    theme.style.win.sb      = &sb;
    theme.style.win.header  = &header;
    theme.style.win.content = &lv_style_transp;
    theme.style.win.btn.rel = &lv_style_transp;
    theme.style.win.btn.pr  = &pr;
#endif
}

#if LV_USE_GROUP

static void style_mod(lv_group_t * group, lv_style_t * style)
{
    (void)group; /*Unused*/
#if LV_COLOR_DEPTH != 1
    uint16_t hue2 = (_hue + 60) % 360;

    /*Make the style to be a little bit orange*/
    style->body.border.opa   = LV_OPA_COVER;
    style->body.border.color = lv_color_hsv_to_rgb(hue2, 90, 70);

    /*If not empty or has border then emphasis the border*/
    if(style->body.opa != LV_OPA_TRANSP || style->body.border.width != 0) style->body.border.width = LV_DPI / 30;

    style->body.main_color   = lv_color_mix(style->body.main_color, lv_color_hsv_to_rgb(hue2, 90, 70), LV_OPA_70);
    style->body.grad_color   = lv_color_mix(style->body.grad_color, lv_color_hsv_to_rgb(hue2, 90, 70), LV_OPA_70);
    style->body.shadow.color = lv_color_mix(style->body.shadow.color, lv_color_hsv_to_rgb(hue2, 90, 70), LV_OPA_60);

    style->text.color = lv_color_mix(style->text.color, lv_color_hsv_to_rgb(hue2, 90, 70), LV_OPA_70);
#else
    style->body.border.opa   = LV_OPA_COVER;
    style->body.border.color = LV_COLOR_BLACK;
    style->body.border.width = 2;
#endif
}

static void style_mod_edit(lv_group_t * group, lv_style_t * style)
{

    uint16_t hue2 = (_hue + 300) % 360;

    (void)group; /*Unused*/
#if LV_COLOR_DEPTH != 1
    /*Make the style to be a little bit orange*/
    style->body.border.opa   = LV_OPA_COVER;
    style->body.border.color = LV_COLOR_GREEN;

    /*If not empty or has border then emphasis the border*/
    if(style->body.opa != LV_OPA_TRANSP || style->body.border.width != 0) style->body.border.width = LV_DPI / 30;


    style->body.main_color   = lv_color_mix(style->body.main_color, lv_color_hsv_to_rgb(hue2, 90, 70), LV_OPA_70);
    style->body.grad_color   = lv_color_mix(style->body.grad_color, lv_color_hsv_to_rgb(hue2, 90, 70), LV_OPA_70);
    style->body.shadow.color = lv_color_mix(style->body.shadow.color, lv_color_hsv_to_rgb(hue2, 90, 70), LV_OPA_60);

    style->text.color = lv_color_mix(style->text.color, lv_color_hsv_to_rgb(hue2, 90, 70), LV_OPA_70);
#else
    style->body.border.opa   = LV_OPA_COVER;
    style->body.border.color = LV_COLOR_BLACK;
    style->body.border.width = 3;
#endif
}

#endif /*LV_USE_GROUP*/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize the material theme
 * @param hue [0..360] hue value from HSV color space to define the theme's base color
 * @param font pointer to a font (NULL to use the default)
 * @return pointer to the initialized theme
 */
lv_theme_t * lv_theme_material_init(uint16_t hue, lv_font_t * font)
{
    if(font == NULL) font = LV_FONT_DEFAULT;

    _hue  = hue;
    _font = font;

    /*For backward compatibility initialize all theme elements with a default style */
    uint16_t i;
    lv_style_t ** style_p = (lv_style_t **)&theme.style;
    for(i = 0; i < LV_THEME_STYLE_COUNT; i++) {
        *style_p = &def;
        style_p++;
    }

    basic_init();
    cont_init();
    btn_init();
    label_init();
    img_init();
    line_init();
    led_init();
    bar_init();
    slider_init();
    sw_init();
    lmeter_init();
    gauge_init();
    chart_init();
    arc_init();
    preload_init();
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

#if LV_USE_GROUP
    theme.group.style_mod_xcb      = style_mod;
    theme.group.style_mod_edit_xcb = style_mod_edit;
#endif

    return &theme;
}

/**
 * Get a pointer to the theme
 * @return pointer to the theme
 */
lv_theme_t * lv_theme_get_material(void)
{
    return &theme;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif
