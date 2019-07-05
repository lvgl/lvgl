/**
 * @file lv_theme_zen.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_theme.h"

#if LV_USE_THEME_ZEN

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
/*Static style definitions*/
static lv_style_t def;
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
    static lv_style_t bg;
    static lv_style_t scr;
    static lv_style_t panel;

    lv_style_copy(&def, &lv_style_pretty); /*Initialize the default style*/
    def.body.border.opa = LV_OPA_COVER;
    def.text.font       = _font;
    def.text.color      = lv_color_hex3(0x444);
    def.image.color     = lv_color_hex3(0x444);

    lv_style_copy(&bg, &def);
    bg.body.main_color   = LV_COLOR_WHITE;
    bg.body.grad_color   = LV_COLOR_WHITE;
    bg.body.radius       = 0;
    bg.body.border.width = 0;
    bg.body.shadow.width = 0;

    lv_style_copy(&scr, &bg);
    scr.body.padding.bottom = 0;
    scr.body.padding.top    = 0;
    scr.body.padding.left   = 0;
    scr.body.padding.right  = 0;

    lv_style_copy(&panel, &bg);
    panel.body.radius         = LV_DPI / 10;
    panel.body.border.width   = 2;
    panel.body.border.color   = lv_color_hsv_to_rgb(_hue, 30, 90);
    panel.body.border.opa     = LV_OPA_COVER;
    panel.body.shadow.width   = 4;
    panel.body.shadow.color   = lv_color_hex3(0xddd);
    panel.body.padding.left   = LV_DPI / 6;
    panel.body.padding.right  = LV_DPI / 6;
    panel.body.padding.top    = LV_DPI / 8;
    panel.body.padding.bottom = LV_DPI / 8;
    panel.body.padding.inner  = LV_DPI / 10;

    lv_style_copy(&sb, &def);
    sb.body.main_color    = lv_color_hsv_to_rgb(_hue, 30, 90);
    sb.body.grad_color    = sb.body.main_color;
    sb.body.border.width  = 0;
    sb.body.radius        = LV_RADIUS_CIRCLE;
    sb.body.padding.inner = LV_DPI / 10;

    theme.style.scr   = &scr;
    theme.style.bg    = &bg;
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
    static lv_style_t rel, pr, tgl_pr, ina;
    lv_style_copy(&rel, &def);
    rel.body.opa            = LV_OPA_TRANSP;
    rel.body.radius         = LV_RADIUS_CIRCLE;
    rel.body.border.width   = 2;
    rel.body.border.color   = lv_color_hsv_to_rgb(_hue, 40, 90);
    rel.body.border.opa     = LV_OPA_COVER;
    rel.body.shadow.width   = 4;
    rel.body.shadow.color   = lv_color_hex3(0xddd);
    rel.body.padding.left   = LV_DPI / 4;
    rel.body.padding.right  = LV_DPI / 4;
    rel.body.padding.top    = LV_DPI / 8;
    rel.body.padding.bottom = LV_DPI / 8;
    rel.text.color          = lv_color_hsv_to_rgb(_hue, 40, 90);
    rel.image.color         = lv_color_hsv_to_rgb(_hue, 40, 90);

    lv_style_copy(&pr, &rel);
    pr.body.border.color = lv_color_hsv_to_rgb(_hue, 40, 60);
    pr.body.shadow.width = 0;
    pr.text.color        = lv_color_hsv_to_rgb(_hue, 40, 60);
    pr.image.color       = lv_color_hsv_to_rgb(_hue, 40, 60);

    lv_style_copy(&tgl_pr, &pr);
    tgl_pr.body.border.color = lv_color_hsv_to_rgb(_hue, 40, 50);
    tgl_pr.text.color        = lv_color_hsv_to_rgb(_hue, 40, 50);
    tgl_pr.image.color       = lv_color_hsv_to_rgb(_hue, 40, 50);

    lv_style_copy(&ina, &tgl_pr);
    ina.body.border.color = lv_color_hex3(0xbbb);
    ina.text.color        = lv_color_hex3(0xbbb);
    ina.image.color       = lv_color_hex3(0xbbb);

    theme.style.btn.rel     = &rel;
    theme.style.btn.pr      = &pr;
    theme.style.btn.tgl_rel = &pr;
    theme.style.btn.tgl_pr  = &tgl_pr;
    theme.style.btn.ina     = &ina;
#endif
}

static void label_init(void)
{
#if LV_USE_LABEL != 0
    static lv_style_t prim, sec, hint;
    lv_style_copy(&prim, &def);
    lv_style_copy(&sec, &def);
    lv_style_copy(&hint, &def);

    prim.text.color = lv_color_hex3(0x555);
    sec.text.color  = lv_color_hsv_to_rgb(_hue, 50, 80);
    hint.text.color = lv_color_hsv_to_rgb(_hue, 25, 85);

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
    img_light.image.color   = lv_color_hsv_to_rgb(_hue, 85, 55);
    img_light.image.intense = LV_OPA_80;

    theme.style.img.light = &img_light;
    theme.style.img.dark  = &img_dark;
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
    lv_style_copy(&led, &lv_style_pretty_color);
    led.body.shadow.width = LV_DPI / 10;
    led.body.radius       = LV_RADIUS_CIRCLE;
    led.body.border.width = LV_DPI / 30;
    led.body.border.opa   = LV_OPA_30;
    led.body.main_color   = lv_color_hsv_to_rgb(_hue, 60, 100);
    led.body.grad_color   = lv_color_hsv_to_rgb(_hue, 60, 40);
    led.body.border.color = lv_color_hsv_to_rgb(_hue, 60, 60);
    led.body.shadow.color = lv_color_hsv_to_rgb(_hue, 80, 100);

    theme.style.led = &led;
#endif
}

static void bar_init(void)
{
#if LV_USE_BAR
    static lv_style_t bg, indic;

    lv_style_copy(&bg, &def);
    bg.body.opa          = LV_OPA_TRANSP;
    bg.body.radius       = LV_RADIUS_CIRCLE;
    bg.body.border.width = 2;
    bg.body.border.opa   = LV_OPA_COVER;
    bg.body.border.color = lv_color_hsv_to_rgb(_hue, 40, 90);

    lv_style_copy(&indic, &def);
    indic.body.radius         = LV_RADIUS_CIRCLE;
    indic.body.main_color     = lv_color_hsv_to_rgb(_hue, 40, 90);
    indic.body.grad_color     = indic.body.main_color;
    indic.body.border.width   = 0;
    indic.body.padding.left   = LV_DPI / 20;
    indic.body.padding.right  = LV_DPI / 20;
    indic.body.padding.top    = LV_DPI / 20;
    indic.body.padding.bottom = LV_DPI / 20;

    theme.style.bar.bg    = &bg;
    theme.style.bar.indic = &indic;
#endif
}

static void slider_init(void)
{
#if LV_USE_SLIDER != 0
    static lv_style_t knob;

    lv_style_copy(&knob, &def);
    knob.body.main_color   = theme.style.bar.indic->body.main_color;
    knob.body.grad_color   = knob.body.main_color;
    knob.body.radius       = LV_RADIUS_CIRCLE;
    knob.body.border.width = 0;

    theme.style.slider.bg    = theme.style.bar.bg;
    theme.style.slider.indic = theme.style.bar.indic;
    theme.style.slider.knob  = &knob;
#endif
}

static void sw_init(void)
{
#if LV_USE_SW != 0
    static lv_style_t indic;

    lv_style_copy(&indic, theme.style.slider.indic);
    indic.body.radius         = LV_RADIUS_CIRCLE;
    indic.body.main_color     = lv_color_hsv_to_rgb(_hue, 15, 95);
    indic.body.grad_color     = indic.body.main_color;
    indic.body.border.width   = theme.style.slider.bg->body.border.width;
    indic.body.border.color   = theme.style.slider.bg->body.border.color;
    indic.body.border.opa     = theme.style.slider.bg->body.border.opa;
    indic.body.padding.left   = 0;
    indic.body.padding.right  = 0;
    indic.body.padding.top    = 0;
    indic.body.padding.bottom = 0;

    theme.style.sw.bg       = theme.style.slider.bg;
    theme.style.sw.indic    = &indic;
    theme.style.sw.knob_off = theme.style.slider.knob;
    theme.style.sw.knob_on  = theme.style.slider.knob;
#endif
}

static void lmeter_init(void)
{
#if LV_USE_LMETER != 0
    static lv_style_t lmeter;

    lv_style_copy(&lmeter, &def);
    lmeter.line.color         = lv_color_hex3(0xddd);
    lmeter.line.width         = 2;
    lmeter.body.main_color    = lv_color_hsv_to_rgb(_hue, 80, 70);
    lmeter.body.grad_color    = lmeter.body.main_color;
    lmeter.body.padding.left  = LV_DPI / 8;
    lmeter.body.padding.right = LV_DPI / 8;

    theme.style.lmeter = &lmeter;
#endif
}

static void gauge_init(void)
{
#if LV_USE_GAUGE != 0
    static lv_style_t gauge;

    lv_style_copy(&gauge, &def);
    gauge.line.color         = lv_color_hsv_to_rgb(_hue, 50, 70);
    gauge.line.width         = 1;
    gauge.body.main_color    = lv_color_hex3(0x999);
    gauge.body.grad_color    = gauge.body.main_color;
    gauge.body.padding.left  = LV_DPI / 16;
    gauge.body.padding.right = LV_DPI / 16;
    gauge.body.border.color  = lv_color_hex3(0x666); /*Needle middle color*/

    theme.style.gauge = &gauge;
#endif
}

static void arc_init(void)
{
#if LV_USE_ARC != 0

    static lv_style_t arc;
    lv_style_copy(&arc, &def);
    arc.line.width   = 10;
    arc.line.color   = lv_color_hsv_to_rgb(_hue, 40, 90);
    arc.line.rounded = 1;

    /*For preloader*/
    arc.body.border.width = 0;

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
#if LV_USE_CALENDAR != 0
    static lv_style_t ina_days;
    lv_style_copy(&ina_days, &def);
    ina_days.text.color = lv_color_hsv_to_rgb(_hue, 0, 70);

    static lv_style_t high_days;
    lv_style_copy(&high_days, &def);
    high_days.text.color = lv_color_hsv_to_rgb(_hue, 50, 90);

    static lv_style_t today_box;
    lv_style_copy(&today_box, &def);
    today_box.body.opa            = LV_OPA_TRANSP;
    today_box.body.border.color   = theme.style.panel->body.border.color;
    today_box.body.padding.top    = LV_DPI / 20;
    today_box.body.padding.bottom = LV_DPI / 20;
    today_box.body.radius         = LV_RADIUS_CIRCLE;

    theme.style.calendar.bg               = theme.style.panel;
    theme.style.calendar.header           = &lv_style_transp;
    theme.style.calendar.inactive_days    = &ina_days;
    theme.style.calendar.highlighted_days = &high_days;
    theme.style.calendar.week_box         = &lv_style_transp_fit;
    theme.style.calendar.today_box        = &today_box;
#endif
}

static void cb_init(void)
{
#if LV_USE_CB != 0
    static lv_style_t rel, pr, tgl_rel, tgl_pr, ina;
    lv_style_copy(&rel, &def);
    rel.body.radius       = LV_DPI / 20;
    rel.body.shadow.width = 0;
    rel.body.border.width = 3;
    rel.body.border.opa   = LV_OPA_COVER;
    rel.body.border.color = lv_color_hsv_to_rgb(_hue, 35, 80);
    rel.body.main_color   = lv_color_hex3(0xfff);
    rel.body.grad_color   = rel.body.main_color;

    lv_style_copy(&pr, &rel);
    pr.body.border.color = lv_color_hsv_to_rgb(_hue, 35, 70);

    lv_style_copy(&tgl_rel, &rel);
    tgl_rel.body.border.color = lv_color_hsv_to_rgb(_hue, 45, 80);
    tgl_rel.body.main_color   = lv_color_hsv_to_rgb(_hue, 40, 90);
    tgl_rel.body.grad_color   = tgl_rel.body.main_color;

    lv_style_copy(&tgl_pr, &rel);
    tgl_pr.body.border.color = lv_color_hsv_to_rgb(_hue, 45, 70);
    tgl_pr.body.main_color   = lv_color_hsv_to_rgb(_hue, 40, 80);
    tgl_pr.body.grad_color   = tgl_pr.body.main_color;

    lv_style_copy(&ina, &rel);
    ina.body.border.color = lv_color_hex3(0xaaa);

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

    lv_style_copy(&bg, &lv_style_transp);
    bg.glass               = 0;
    bg.body.padding.left   = 0;
    bg.body.padding.right  = 0;
    bg.body.padding.top    = 0;
    bg.body.padding.bottom = 0;
    bg.body.padding.inner  = LV_DPI / 15;
    bg.text.font           = _font;

    lv_style_copy(&rel, &def);
    rel.body.opa          = LV_OPA_TRANSP;
    rel.body.border.width = 0;

    lv_style_copy(&pr, &def);
    pr.body.opa          = LV_OPA_TRANSP;
    pr.body.radius       = LV_DPI / 1;
    pr.body.border.width = 2;
    pr.body.border.color = lv_color_hsv_to_rgb(_hue, 40, 60);
    pr.body.border.opa   = LV_OPA_COVER;
    pr.text.color        = lv_color_hsv_to_rgb(_hue, 40, 60);

    lv_style_copy(&tgl_rel, &pr);
    tgl_rel.body.opa          = LV_OPA_COVER;
    tgl_rel.body.main_color   = lv_color_hsv_to_rgb(_hue, 15, 95);
    tgl_rel.body.grad_color   = tgl_rel.body.main_color;
    tgl_rel.body.border.width = 0;
    tgl_rel.text.color        = lv_color_hsv_to_rgb(_hue, 60, 40);

    lv_style_copy(&tgl_pr, &tgl_rel);
    tgl_pr.body.main_color = lv_color_hsv_to_rgb(_hue, 30, 70);
    tgl_pr.body.grad_color = tgl_pr.body.main_color;

    lv_style_copy(&ina, &pr);
    ina.body.main_color = lv_color_hex3(0x888);
    ina.body.grad_color = tgl_pr.body.main_color;
    ina.text.color      = lv_color_hex3(0x888);
    ;

    theme.style.btnm.bg          = &bg;
    theme.style.btnm.btn.rel     = &rel;
    theme.style.btnm.btn.pr      = &pr;
    theme.style.btnm.btn.tgl_rel = &tgl_rel;
    theme.style.btnm.btn.tgl_pr  = &tgl_pr;
    theme.style.btnm.btn.ina     = &ina;
#endif
}

static void kb_init(void)
{
#if LV_USE_KB
    static lv_style_t bg, rel, pr, tgl_rel, tgl_pr, ina;
    lv_style_copy(&bg, &def);
    bg.body.main_color     = lv_color_hex3(0x666);
    bg.body.grad_color     = bg.body.main_color;
    bg.body.padding.left   = 0;
    bg.body.padding.right  = 0;
    bg.body.padding.top    = 0;
    bg.body.padding.bottom = 0;
    bg.body.padding.inner  = 0;
    bg.body.radius         = 0;
    bg.body.border.width   = 0;

    lv_style_copy(&rel, &def);
    rel.body.opa          = LV_OPA_COVER;
    rel.body.radius       = 0;
    rel.body.border.width = 1;
    rel.body.border.color = lv_color_hex3(0x888);
    rel.body.border.opa   = LV_OPA_COVER;
    rel.text.color        = LV_COLOR_WHITE;

    lv_style_copy(&pr, &def);
    pr.body.main_color   = lv_color_hex3(0xeee);
    pr.body.grad_color   = pr.body.main_color;
    pr.body.border.color = lv_color_hex3(0x888);
    pr.body.border.width = 1;
    pr.body.border.opa   = LV_OPA_COVER;
    pr.body.radius       = 0;
    pr.text.color        = lv_color_hex3(0x666);

    lv_style_copy(&tgl_rel, &pr);
    tgl_rel.body.main_color = lv_color_hex3(0x999);
    tgl_rel.body.grad_color = tgl_rel.body.main_color;
    tgl_rel.text.color      = LV_COLOR_WHITE;

    lv_style_copy(&tgl_pr, &pr);
    tgl_pr.body.main_color = lv_color_hex3(0xbbb);
    tgl_pr.body.grad_color = tgl_pr.body.main_color;
    tgl_pr.text.color      = lv_color_hex3(0xddd);

    lv_style_copy(&ina, &pr);
    ina.body.main_color = lv_color_hex3(0x777);
    ina.body.grad_color = ina.body.main_color;
    ina.text.color      = lv_color_hex3(0xbbb);

    theme.style.kb.bg          = &bg;
    theme.style.kb.btn.rel     = &rel;
    theme.style.kb.btn.pr      = &pr;
    theme.style.kb.btn.tgl_rel = &tgl_rel;
    theme.style.kb.btn.tgl_pr  = &tgl_pr;
    theme.style.kb.btn.ina     = &ina;
#endif
}

static void mbox_init(void)
{
#if LV_USE_MBOX
    static lv_style_t bg, rel, pr;
    lv_style_copy(&bg, theme.style.panel);
    bg.body.main_color = lv_color_hsv_to_rgb(_hue, 10, 95);
    bg.body.grad_color = bg.body.main_color;
    bg.text.color      = lv_color_hsv_to_rgb(_hue, 40, 25);

    lv_style_copy(&rel, &def);
    rel.body.main_color     = lv_color_hsv_to_rgb(_hue, 25, 85);
    rel.body.grad_color     = rel.body.main_color;
    rel.body.radius         = LV_RADIUS_CIRCLE;
    rel.body.border.width   = 2;
    rel.body.border.color   = lv_color_hsv_to_rgb(_hue, 30, 70);
    rel.body.padding.left   = LV_DPI / 4;
    rel.body.padding.right  = LV_DPI / 4;
    rel.body.padding.top    = LV_DPI / 8;
    rel.body.padding.bottom = LV_DPI / 8;
    rel.text.color          = bg.text.color;

    lv_style_copy(&pr, &rel);
    pr.body.border.color = lv_color_hsv_to_rgb(_hue, 30, 90);
    pr.text.color        = lv_color_hsv_to_rgb(_hue, 40, 40);
    pr.body.main_color   = lv_color_hsv_to_rgb(_hue, 20, 85);
    pr.body.grad_color   = pr.body.main_color;

    theme.style.mbox.bg      = &bg;
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
    lv_style_copy(&oneline, theme.style.panel);
    oneline.body.radius         = LV_RADIUS_CIRCLE;
    oneline.body.padding.top    = LV_DPI / 10;
    oneline.body.padding.bottom = LV_DPI / 10;
    oneline.body.shadow.width   = 0;

    theme.style.ta.area    = theme.style.panel;
    theme.style.ta.oneline = &oneline;
    theme.style.ta.cursor  = NULL; /*Let library to calculate the cursor's style*/
    theme.style.ta.sb      = &def;
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
    static lv_style_t bg, rel, pr, tgl_rel, tgl_pr, ina;

    lv_style_copy(&bg, theme.style.panel);
    bg.body.padding.left   = 0;
    bg.body.padding.right  = 0;
    bg.body.padding.top    = 0;
    bg.body.padding.bottom = 0;

    lv_style_copy(&rel, &def);
    rel.body.opa            = LV_OPA_TRANSP;
    rel.body.border.width   = 0;
    rel.body.padding.left   = LV_DPI / 8;
    rel.body.padding.right  = LV_DPI / 8;
    rel.body.padding.top    = LV_DPI / 8;
    rel.body.padding.bottom = LV_DPI / 8;
    rel.text.color          = lv_color_hex3(0x666);
    rel.image.color         = lv_color_hex3(0x666);

    lv_style_copy(&pr, &rel);
    pr.text.color  = theme.style.btn.pr->text.color;
    pr.image.color = theme.style.btn.pr->image.color;

    lv_style_copy(&tgl_rel, &rel);
    tgl_rel.text.color = lv_color_hsv_to_rgb(_hue, 50, 90);

    lv_style_copy(&tgl_pr, &rel);
    tgl_pr.text.color  = theme.style.btn.tgl_pr->text.color;
    tgl_pr.image.color = theme.style.btn.tgl_pr->image.color;

    lv_style_copy(&ina, &rel);
    ina.text.color  = theme.style.btn.ina->text.color;
    ina.image.color = theme.style.btn.ina->image.color;

    theme.style.list.sb          = &sb;
    theme.style.list.bg          = &bg;
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
    bg.text.line_space     = LV_DPI / 8;
    bg.body.padding.left   = LV_DPI / 6;
    bg.body.padding.right  = LV_DPI / 6;
    bg.body.padding.top    = LV_DPI / 8;
    bg.body.padding.bottom = LV_DPI / 8;
    bg.text.color          = lv_color_hex3(0x666);

    lv_style_copy(&sel, &def);
    sel.body.opa          = LV_OPA_TRANSP;
    sel.body.border.width = 0;
    sel.text.color        = lv_color_hsv_to_rgb(_hue, 50, 80);

    theme.style.ddlist.bg  = &bg;
    theme.style.ddlist.sel = &sel;
    theme.style.ddlist.sb  = &def;
#endif
}

static void roller_init(void)
{
#if LV_USE_ROLLER != 0
    static lv_style_t bg, sel;
    lv_style_copy(&bg, &def);
    bg.body.border.width = 0;
    bg.body.opa          = LV_OPA_TRANSP;
    bg.text.line_space   = LV_DPI / 6;
    bg.text.color        = lv_color_hex3(0x999);

    lv_style_copy(&sel, theme.style.panel);
    sel.body.radius = LV_RADIUS_CIRCLE;
    sel.body.opa    = LV_OPA_TRANSP;

    theme.style.roller.bg  = &bg;
    theme.style.roller.sel = &sel;
#endif
}

static void tabview_init(void)
{
#if LV_USE_TABVIEW != 0
    static lv_style_t btn_bg, indic, rel, pr, tgl_rel, tgl_pr;

    lv_style_copy(&btn_bg, &def);
    btn_bg.body.opa          = LV_OPA_TRANSP;
    btn_bg.body.border.width = 2;
    btn_bg.body.border.part  = LV_BORDER_BOTTOM;
    btn_bg.body.border.color = lv_color_hsv_to_rgb(_hue, 10, 90);

    lv_style_copy(&indic, &def);
    indic.body.padding.inner = LV_DPI / 16;
    indic.body.border.width  = 0;
    indic.body.radius        = LV_RADIUS_CIRCLE;
    indic.body.main_color    = lv_color_hsv_to_rgb(_hue, 50, 80);
    indic.body.grad_color    = indic.body.main_color;

    lv_style_copy(&rel, &def);
    rel.body.opa          = LV_OPA_TRANSP;
    rel.body.border.width = 0;
    rel.text.color        = lv_color_hex3(0x999);

    lv_style_copy(&pr, &rel);
    pr.text.color = lv_color_hex3(0x777);

    lv_style_copy(&tgl_rel, &rel);
    tgl_rel.text.color = lv_color_hsv_to_rgb(_hue, 50, 80);

    lv_style_copy(&tgl_pr, &rel);
    tgl_pr.text.color = lv_color_hsv_to_rgb(_hue, 50, 70);

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
    cell.body.shadow.width   = 0;
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
    static lv_style_t header, rel, pr;

    lv_style_copy(&header, &def);
    header.body.opa            = LV_OPA_TRANSP;
    header.body.border.width   = 2;
    header.body.border.part    = LV_BORDER_BOTTOM;
    header.body.border.color   = lv_color_hsv_to_rgb(_hue, 10, 90);
    header.text.color          = lv_color_hex3(0x666);
    header.image.color         = lv_color_hex3(0x666);
    header.body.padding.top    = 0;
    header.body.padding.bottom = 0;
    header.body.padding.inner  = 0;

    lv_style_copy(&rel, &def);
    rel.body.opa          = LV_OPA_TRANSP;
    rel.body.border.width = 0;
    rel.text.color        = lv_color_hex3(0x666);
    rel.image.color       = lv_color_hex3(0x666);

    lv_style_copy(&pr, &rel);
    pr.text.color  = lv_color_hex3(0x333);
    pr.image.color = lv_color_hex3(0x333);

    theme.style.win.bg      = theme.style.panel;
    theme.style.win.sb      = &sb;
    theme.style.win.header  = &header;
    theme.style.win.content = &lv_style_transp;
    theme.style.win.btn.rel = &rel;
    theme.style.win.btn.pr  = &pr;
#endif
}

#if LV_USE_GROUP

static void style_mod(lv_group_t * group, lv_style_t * style)
{
    (void)group; /*Unused*/
#if LV_COLOR_DEPTH != 1
    /*Make the style to be a little bit orange*/
    style->body.border.opa   = LV_OPA_COVER;
    style->body.border.color = lv_color_hsv_to_rgb(_hue, 40, 50);

    /*If not empty or has border then emphasis the border*/
    if(style->body.opa != LV_OPA_TRANSP || style->body.border.width != 0) style->body.border.width = LV_DPI / 20;
#else
    style->body.border.opa   = LV_OPA_COVER;
    style->body.border.color = LV_COLOR_BLACK;
    style->body.border.width = 2;
#endif
}

static void style_mod_edit(lv_group_t * group, lv_style_t * style)
{
    (void)group; /*Unused*/

#if LV_COLOR_DEPTH != 1
    /*Make the style to be a little bit orange*/
    style->body.border.opa   = LV_OPA_COVER;
    style->body.border.color = LV_COLOR_GREEN;

    /*If not empty or has border then emphasis the border*/
    if(style->body.opa != LV_OPA_TRANSP || style->body.border.width != 0) style->body.border.width = LV_DPI / 20;

    style->body.main_color   = lv_color_mix(style->body.main_color, LV_COLOR_GREEN, LV_OPA_70);
    style->body.grad_color   = lv_color_mix(style->body.grad_color, LV_COLOR_GREEN, LV_OPA_70);
    style->body.shadow.color = lv_color_mix(style->body.shadow.color, LV_COLOR_GREEN, LV_OPA_60);

    style->text.color = lv_color_mix(style->text.color, LV_COLOR_GREEN, LV_OPA_70);
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
 * Initialize the zen theme
 * @param hue [0..360] hue value from HSV color space to define the theme's base color
 * @param font pointer to a font (NULL to use the default)
 * @return pointer to the initialized theme
 */
lv_theme_t * lv_theme_zen_init(uint16_t hue, lv_font_t * font)
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
lv_theme_t * lv_theme_get_zen(void)
{
    return &theme;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif
