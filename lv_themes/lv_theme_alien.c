/**
 * @file lv_theme_alien.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_theme.h"

#if USE_LV_THEME_ALIEN

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

static uint16_t _hue;
static lv_font_t * _font;
static lv_font_t * _font;
static lv_font_t * _font;

static lv_theme_t theme;
static lv_style_t def;
static lv_style_t bg;
static lv_style_t panel;        /*General fancy background (e.g. to chart or ta)*/
static lv_style_t sb;
static lv_style_t btn_rel, btn_pr, btn_trel, btn_tpr, btn_ina;

#if USE_LV_BAR
static lv_style_t bar_bg, bar_indic;
#endif

#if USE_LV_SLIDER
static lv_style_t slider_knob;
#endif

#if USE_LV_LMETER
static lv_style_t lmeter_bg;
#endif

#if USE_LV_DDLIST
static lv_style_t ddlist_bg, ddlist_sel;
#endif

#if USE_LV_BTNM
static lv_style_t btnm_bg, btnm_rel, btnm_pr, btnm_trel, btnm_ina;
#endif


/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void basic_init(void)
{
    /*Default*/
    lv_style_copy(&def, &lv_style_plain);
    def.body.opa = LV_OPA_COVER;
    def.glass = 0;

    def.body.empty = 0;
    def.body.main_color = LV_COLOR_HEX3(0x222);
    def.body.grad_color = LV_COLOR_HEX3(0x222);
    def.body.radius = 0;
    def.body.padding.hor = LV_DPI / 8;
    def.body.padding.ver = LV_DPI / 8;
    def.body.padding.inner = LV_DPI / 8;
    def.body.border.color = LV_COLOR_SILVER;
    def.body.border.width = 1;
    def.body.border.opa = LV_OPA_COVER;
    def.body.shadow.color = LV_COLOR_SILVER;
    def.body.shadow.width = 0;
    def.body.shadow.type = LV_SHADOW_FULL;

    def.text.color = LV_COLOR_HEX3(0xDDD);
    def.text.font = _font;
    def.text.letter_space = 1;
    def.text.line_space = 2;

    def.image.color = LV_COLOR_HEX3(0xDDD);
    def.image.intense = LV_OPA_TRANSP;

    def.line.color = LV_COLOR_HEX3(0xDDD);
    def.line.width = 1;

    /*Background*/
    lv_style_copy(&bg, &def);
    bg.body.main_color = LV_COLOR_HEX3(0x333);
    bg.body.grad_color =  LV_COLOR_HEX3(0x333);
    bg.body.border.width = 2;
    bg.body.border.color =  LV_COLOR_HEX3(0x666);
    bg.body.shadow.color = LV_COLOR_SILVER;

    /*Panel*/
    lv_style_copy(&panel, &def);
    panel.body.radius = LV_DPI / 10;
    panel.body.main_color = LV_COLOR_HEX3(0x666);
    panel.body.grad_color = LV_COLOR_HEX3(0x666);
    panel.body.border.color = LV_COLOR_HEX3(0xccc);
    panel.body.border.width = 2;
    panel.body.border.opa = LV_OPA_60;
    panel.text.color = lv_color_hsv_to_rgb(_hue, 8, 96);
    panel.line.color = lv_color_hsv_to_rgb(_hue, 20, 70);

    /*Scrollbar*/
    lv_style_copy(&sb, &def);
    sb.body.opa = LV_OPA_50;
    sb.body.radius = LV_RADIUS_CIRCLE;
    sb.body.border.color = LV_COLOR_SILVER;
    sb.body.border.opa = LV_OPA_40;
    sb.body.border.width = 1;
    sb.body.main_color = lv_color_hsv_to_rgb(_hue, 33, 92);
    sb.body.grad_color = lv_color_hsv_to_rgb(_hue, 33, 92);
    sb.body.padding.hor = 1;
    sb.body.padding.ver = 1;
    sb.body.padding.inner = LV_DPI / 15;      /*Scrollbar width*/

    theme.bg = &bg;
    theme.panel = &panel;

}

static void btn_init(void)
{
#if USE_LV_BTN != 0
    lv_style_copy(&btn_rel, &def);
    btn_rel.glass = 0;
    btn_rel.body.empty = 1;
    btn_rel.body.radius = LV_RADIUS_CIRCLE;
    btn_rel.body.border.width = 2;
    btn_rel.body.border.color = lv_color_hsv_to_rgb(_hue, 70, 90);
    btn_rel.body.border.opa = LV_OPA_80;
    btn_rel.body.padding.hor = LV_DPI / 4;
    btn_rel.body.padding.ver = LV_DPI / 6;
    btn_rel.body.padding.inner = LV_DPI / 10;
    btn_rel.text.color = lv_color_hsv_to_rgb(_hue, 8, 96);
    btn_rel.text.font = _font;

    lv_style_copy(&btn_pr, &btn_rel);
    btn_pr.body.opa = LV_OPA_COVER;
    btn_pr.body.empty = 0;
    btn_pr.body.main_color = lv_color_hsv_to_rgb(_hue, 50, 50);
    btn_pr.body.grad_color = lv_color_hsv_to_rgb(_hue, 50, 50);
    btn_pr.body.border.opa = LV_OPA_60;
    btn_pr.text.font = _font;
    btn_pr.text.color = lv_color_hsv_to_rgb(_hue, 10, 100);

    lv_style_copy(&btn_trel, &btn_pr);
    btn_trel.body.opa = LV_OPA_COVER;
    btn_trel.body.empty = 0;
    btn_trel.body.main_color = lv_color_hsv_to_rgb(_hue, 50, 60);
    btn_trel.body.grad_color = lv_color_hsv_to_rgb(_hue, 50, 60);
    btn_trel.body.border.opa = LV_OPA_60;
    btn_trel.body.border.color = lv_color_hsv_to_rgb(_hue, 80, 90);
    btn_trel.text.font = _font;
    btn_trel.text.color = lv_color_hsv_to_rgb(_hue, 0, 100);

    lv_style_copy(&btn_tpr, &btn_trel);
    btn_tpr.body.opa = LV_OPA_COVER;
    btn_tpr.body.empty = 0;
    btn_tpr.body.main_color = lv_color_hsv_to_rgb(_hue, 50, 50);
    btn_tpr.body.grad_color = lv_color_hsv_to_rgb(_hue, 50, 50);
    btn_tpr.body.border.opa = LV_OPA_60;
    btn_tpr.body.border.color = lv_color_hsv_to_rgb(_hue, 80, 70);
    btn_tpr.text.font = _font;
    btn_tpr.text.color = lv_color_hsv_to_rgb(_hue, 10, 90);

    lv_style_copy(&btn_ina, &btn_rel);
    btn_ina.body.border.opa = LV_OPA_60;
    btn_ina.body.border.color = lv_color_hsv_to_rgb(_hue, 10, 50);
    btn_ina.text.font = _font;
    btn_ina.text.color = lv_color_hsv_to_rgb(_hue, 10, 90);

    theme.btn.rel = &btn_rel;
    theme.btn.pr = &btn_pr;
    theme.btn.tgl_rel = &btn_trel;
    theme.btn.tgl_pr =  &btn_tpr;
    theme.btn.ina =  &btn_ina;
#endif
}


static void label_init(void)
{
#if USE_LV_LABEL != 0
    static lv_style_t label_prim, label_sec, label_hint;

    lv_style_copy(&label_prim, &def);
    label_prim.text.font = _font;
    label_prim.text.color = lv_color_hsv_to_rgb(_hue, 80, 96);

    lv_style_copy(&label_sec, &label_prim);
    label_sec.text.color = lv_color_hsv_to_rgb(_hue, 40, 85);

    lv_style_copy(&label_hint, &label_prim);
    label_hint.text.color = lv_color_hsv_to_rgb(_hue, 20, 70);

    theme.label.prim = &label_prim;
    theme.label.sec = &label_sec;
    theme.label.hint = &label_hint;
#endif
}


static void bar_init(void)
{
#if USE_LV_BAR
    lv_style_copy(&bar_bg, &def);
    bar_bg.body.opa = LV_OPA_30;
    bar_bg.body.radius = LV_RADIUS_CIRCLE;
    bar_bg.body.main_color = LV_COLOR_WHITE;
    bar_bg.body.grad_color = LV_COLOR_SILVER;
    bar_bg.body.border.width = 2;
    bar_bg.body.border.color = LV_COLOR_SILVER;
    bar_bg.body.border.opa = LV_OPA_20;
    bar_bg.body.padding.hor = 0;
    bar_bg.body.padding.ver = LV_DPI / 10;
    bar_bg.body.padding.inner = 0;

    lv_style_copy(&bar_indic, &def);
    bar_indic.body.radius = LV_RADIUS_CIRCLE;
    bar_indic.body.border.width = 2;
    bar_indic.body.border.color = LV_COLOR_SILVER;
    bar_indic.body.border.opa = LV_OPA_70;
    bar_indic.body.padding.hor = 0;
    bar_indic.body.padding.ver = 0;
    bar_indic.body.shadow.width = LV_DPI / 20;
    bar_indic.body.shadow.color = lv_color_hsv_to_rgb(_hue, 20, 90);
    bar_indic.body.main_color = lv_color_hsv_to_rgb(_hue, 40, 80);
    bar_indic.body.grad_color = lv_color_hsv_to_rgb(_hue, 40, 80);

    theme.bar.bg = &bar_bg;
    theme.bar.indic = &bar_indic;
#endif
}

static void img_init(void)
{
#if USE_LV_IMG != 0
    static lv_style_t img_light, img_dark;
    lv_style_copy(&img_light, &def);
    img_light.image.color = lv_color_hsv_to_rgb(_hue, 15, 85);
    img_light.image.intense = LV_OPA_80;

    lv_style_copy(&img_dark, &def);
    img_light.image.color = lv_color_hsv_to_rgb(_hue, 85, 65);
    img_light.image.intense = LV_OPA_80;

    theme.img.light = &img_light;
    theme.img.dark = &img_dark;
#endif
}

static void line_init(void)
{
#if USE_LV_LINE != 0
    static lv_style_t line_decor;
    lv_style_copy(&line_decor, &def);
    line_decor.line.color = lv_color_hsv_to_rgb(_hue, 50, 50);
    line_decor.line.width = 1;

    theme.line.decor = &line_decor;
#endif
}

static void led_init(void)
{
#if USE_LV_LED != 0
    static lv_style_t led;
    lv_style_copy(&led, &lv_style_pretty_color);
    led.body.shadow.width = LV_DPI / 10;
    led.body.radius = LV_RADIUS_CIRCLE;
    led.body.border.width= LV_DPI / 30;
    led.body.border.opa = LV_OPA_30;
    led.body.main_color = lv_color_hsv_to_rgb(_hue, 100, 100);
    led.body.grad_color = lv_color_hsv_to_rgb(_hue, 100, 40);
    led.body.border.color = lv_color_hsv_to_rgb(_hue, 60, 60);
    led.body.shadow.color = lv_color_hsv_to_rgb(_hue, 100, 100);

    theme.led = &led;
#endif
}

static void slider_init(void)
{
#if USE_LV_SLIDER != 0
    lv_style_copy(&slider_knob, &def);
    slider_knob.body.opa = LV_OPA_60;
    slider_knob.body.radius = LV_RADIUS_CIRCLE;
    slider_knob.body.main_color = LV_COLOR_WHITE;
    slider_knob.body.grad_color = LV_COLOR_SILVER;
    slider_knob.body.border.width = 1;
    slider_knob.body.border.color = LV_COLOR_GRAY;
    slider_knob.body.border.opa = LV_OPA_50;

    theme.slider.bg = &bar_bg;
    theme.slider.indic = &bar_indic;
    theme.slider.knob = &slider_knob;
#endif
}

static void sw_init(void)
{
#if USE_LV_SW != 0
    static lv_style_t sw_bg, sw_indic, sw_knob;
    lv_style_copy(&sw_bg, &bar_bg);
    sw_bg.body.opa = LV_OPA_COVER;
    sw_bg.body.padding.ver = -2 ;
    sw_bg.body.padding.hor = -2 ;
    sw_bg.body.main_color = LV_COLOR_HEX3(0x666);
    sw_bg.body.grad_color = LV_COLOR_HEX3(0x999);
    sw_bg.body.border.width = 2;
    sw_bg.body.border.opa = LV_OPA_50;

    lv_style_copy(&sw_indic, &bar_indic);
    sw_indic.body.shadow .width = LV_DPI / 20;
    sw_indic.body.padding.ver = 0;
    sw_indic.body.padding.hor = 0;

    lv_style_copy(&sw_knob, &slider_knob);
    sw_knob.body.opa = LV_OPA_80;

    theme.sw.bg = &sw_bg;
    theme.sw.indic = &sw_indic;
    theme.sw.knob_off = &sw_knob;
    theme.sw.knob_on = &sw_knob;
#endif
}


static void lmeter_init(void)
{
#if USE_LV_LMETER != 0
    lv_style_copy(&lmeter_bg, &def);
    lmeter_bg.body.main_color = lv_color_hsv_to_rgb(_hue, 10, 70);
    lmeter_bg.body.grad_color = lv_color_hsv_to_rgb(_hue, 80, 80);
    lmeter_bg.body.padding.hor = LV_DPI / 8;         /*Scale line length*/
    lmeter_bg.line.color = LV_COLOR_HEX3(0x222);
    lmeter_bg.line.width = 2;

    theme.lmeter = &lmeter_bg;

#endif
}

static void gauge_init(void)
{
#if USE_LV_GAUGE != 0
    static lv_style_t gauge_bg;
    lv_style_copy(&gauge_bg, &def);
    gauge_bg.body.main_color = lv_color_hsv_to_rgb(_hue, 10, 70);
    gauge_bg.body.grad_color = gauge_bg.body.main_color;
    gauge_bg.body.padding.hor = LV_DPI / 16;         /*Scale line length*/
    gauge_bg.body.padding.ver = LV_DPI / 10;        /*Needle center size*/
    gauge_bg.body.padding.inner = LV_DPI / 12;      /*Label - scale distance*/
    gauge_bg.body.border.color = LV_COLOR_HEX3(0x777);
    gauge_bg.line.color = lv_color_hsv_to_rgb(_hue, 80, 75);
    gauge_bg.line.width = 2;
    gauge_bg.text.color = lv_color_hsv_to_rgb(_hue, 10, 90);
    gauge_bg.text.font = _font;

    theme.gauge = &gauge_bg;
#endif
}

static void chart_init(void)
{
#if USE_LV_CHART
    theme.chart = &panel;
#endif
}

static void cb_init(void)
{
#if USE_LV_CB != 0
    static lv_style_t cb_bg, cb_rel, cb_pr, cb_trel, cb_tpr,  cb_ina;
    lv_style_copy(&cb_rel, &bg);
    cb_rel.body.radius = LV_DPI / 20;
    cb_rel.body.border.width = 1;
    cb_rel.body.border.color = LV_COLOR_GRAY;
    cb_rel.body.main_color = LV_COLOR_WHITE;
    cb_rel.body.grad_color = LV_COLOR_SILVER;

    lv_style_copy(&cb_bg, &bg);
    cb_bg.body.empty = 1;
    cb_bg.body.border.width = 0;
    cb_bg.body.padding.inner =  LV_DPI / 8;
    cb_bg.body.padding.hor =  0;
    cb_bg.body.padding.ver =  0;
    cb_bg.text.font = _font;

    lv_style_copy(&cb_pr, &cb_rel);
    cb_pr.body.main_color = lv_color_hsv_to_rgb(_hue, 10, 90);
    cb_pr.body.main_color = lv_color_hsv_to_rgb(_hue, 10, 82);

    lv_style_copy(&cb_trel, &cb_rel);
    cb_trel.body.border.width = 4;
    cb_trel.body.border.color = LV_COLOR_WHITE;
    cb_trel.body.border.opa = LV_OPA_60;
    cb_trel.body.main_color = lv_color_hsv_to_rgb(_hue, 50, 82);
    cb_trel.body.grad_color = lv_color_hsv_to_rgb(_hue, 50, 62);

    lv_style_copy(&cb_tpr, &cb_trel);
    cb_tpr.body.border.color = LV_COLOR_SILVER;
    cb_tpr.body.border.opa = LV_OPA_70;
    cb_tpr.body.main_color = lv_color_hsv_to_rgb(_hue, 50, 72);
    cb_tpr.body.grad_color = lv_color_hsv_to_rgb(_hue, 50, 52);

    lv_style_copy(&cb_ina, &cb_trel);
    cb_ina.body.border.width = 1;
    cb_ina.body.border.color = LV_COLOR_GRAY;
    cb_ina.body.main_color = LV_COLOR_SILVER;
    cb_ina.body.grad_color = LV_COLOR_SILVER;

    theme.cb.bg = &cb_bg;
    theme.cb.box.rel = &cb_rel;
    theme.cb.box.pr = &cb_pr;
    theme.cb.box.tgl_rel = &cb_trel;
    theme.cb.box.tgl_pr = &cb_tpr;
    theme.cb.box.ina = &cb_ina;
#endif
}


static void btnm_init(void)
{
#if USE_LV_BTNM
    lv_style_copy(&btnm_bg, &lv_style_transp_tight);
    btnm_bg.body.border.width = 1;
    btnm_bg.body.border.color = lv_color_hsv_to_rgb(_hue, 60, 80);
    btnm_bg.body.border.opa = LV_OPA_COVER;
    btnm_bg.body.radius = LV_DPI / 8;

    lv_style_copy(&btnm_rel, &lv_style_plain);
    btnm_rel.body.empty = 1;
    btnm_rel.body.radius = LV_DPI / 8;
    btnm_rel.text.color = lv_color_hsv_to_rgb(_hue, 60, 80);
    btnm_rel.text.font = _font;

    lv_style_copy(&btnm_pr, &lv_style_plain);
    btnm_pr.body.main_color = lv_color_hsv_to_rgb(_hue, 40, 70);
    btnm_pr.body.grad_color = lv_color_hsv_to_rgb(_hue, 40, 70);
    btnm_pr.body.radius = LV_DPI / 8;
    btnm_pr.text.color = lv_color_hsv_to_rgb(_hue, 40, 40);
    btnm_pr.text.font = _font;

    lv_style_copy(&btnm_trel, &btnm_rel);
    btnm_trel.body.border.color = lv_color_hsv_to_rgb(_hue, 80, 80);
    btnm_trel.body.border.width = 3;

    lv_style_copy(&btnm_ina, &btnm_rel);
    btnm_ina.text.color = lv_color_hsv_to_rgb(_hue, 10, 60);

    theme.btnm.bg = &btnm_bg;
    theme.btnm.btn.rel = &btnm_rel;
    theme.btnm.btn.pr = &btnm_pr;
    theme.btnm.btn.tgl_rel = &btnm_trel;
    theme.btnm.btn.tgl_pr = &btnm_pr;
    theme.btnm.btn.ina = &btnm_ina;
#endif
}

static void kb_init(void)
{
#if USE_LV_KB
    theme.kb.bg = &btnm_bg;
    theme.kb.btn.rel = &btnm_rel;
    theme.kb.btn.pr = &btnm_pr;
    theme.kb.btn.tgl_rel = &btnm_trel;
    theme.kb.btn.tgl_pr = &btnm_pr;
    theme.kb.btn.ina = &btnm_ina;
#endif

}

static void mbox_init(void)
{
#if USE_LV_MBOX
    static lv_style_t mbox_bg;
    lv_style_copy(&mbox_bg, &panel);
    mbox_bg.body.shadow.width = LV_DPI / 12;

    theme.mbox.bg = &mbox_bg;
    theme.mbox.btn.bg = &lv_style_transp;
    theme.mbox.btn.rel = &btn_trel;
    theme.mbox.btn.pr = &btn_tpr;
#endif
}

static void page_init(void)
{
#if USE_LV_PAGE
    theme.page.bg = &panel;
    theme.page.scrl = &lv_style_transp_fit;
    theme.page.sb = &sb;
#endif
}

static void ta_init(void)
{
#if USE_LV_TA
    theme.ta.area = &panel;
    theme.ta.oneline = &panel;
    theme.ta.cursor = NULL;
    theme.ta.sb = &sb;
#endif
}

static void list_init(void)
{
#if USE_LV_LIST != 0
    static lv_style_t list_bg, list_rel, list_pr, list_trel, list_tpr, list_ina;
    lv_style_copy(&list_rel, &def);
    list_rel.body.empty = 1;
    list_rel.body.border.width = 1;
    list_rel.body.border.color = lv_color_hsv_to_rgb(_hue, 50, 85);
    list_rel.body.border.opa = LV_OPA_COVER;
    list_rel.text.color = lv_color_hsv_to_rgb(_hue, 10, 94);
    list_rel.text.font = _font;

    lv_style_copy(&list_pr, &list_rel);
    list_pr.body.empty = 0;
    list_pr.body.opa = LV_OPA_COVER;
    list_pr.body.main_color = lv_color_hsv_to_rgb(_hue, 34, 41);
    list_pr.body.grad_color = lv_color_hsv_to_rgb(_hue, 34, 41);
    list_pr.text.color = lv_color_hsv_to_rgb(_hue, 7, 96);

    lv_style_copy(&list_trel, &list_rel);
    lv_style_copy(&list_tpr, &list_pr);
    lv_style_copy(&list_ina, &def);

    lv_style_copy(&list_bg, &list_rel);
    list_bg.body.padding.hor = 0;
    list_bg.body.padding.ver = 0;

    theme.list.sb = &sb;
    theme.list.bg = &list_bg;
    theme.list.scrl = &lv_style_transp_tight;
    theme.list.btn.rel = &list_rel;
    theme.list.btn.pr = &list_pr;
    theme.list.btn.tgl_rel = &list_trel;
    theme.list.btn.tgl_pr = &list_tpr;
    theme.list.btn.ina = &list_ina;
#endif
}

static void ddlist_init(void)
{
#if USE_LV_DDLIST != 0
    lv_style_copy(&ddlist_bg, &panel);
    ddlist_bg.text.line_space = LV_DPI / 8;
    ddlist_bg.body.padding.hor = LV_DPI / 6;
    ddlist_bg.body.padding.ver = LV_DPI / 6;

    lv_style_copy(&ddlist_sel, &panel);
    ddlist_sel.body.main_color = lv_color_hsv_to_rgb(_hue, 45, 70);
    ddlist_sel.body.grad_color = lv_color_hsv_to_rgb(_hue, 45, 70);
    ddlist_sel.body.opa = LV_OPA_COVER;
    ddlist_sel.body.radius = 0;

    theme.ddlist.bg = &ddlist_bg;
    theme.ddlist.sel = &ddlist_sel;
    theme.ddlist.sb = &sb;
#endif
}

static void roller_init(void)
{
#if USE_LV_ROLLER != 0
    static lv_style_t roller_bg, roller_sel;
    lv_style_copy(&roller_bg, &ddlist_bg);
    roller_bg.text.line_space = LV_DPI / 6;
    roller_bg.body.radius = LV_DPI / 20;
    roller_bg.body.main_color = LV_COLOR_HEX3(0x222);
    roller_bg.body.grad_color = LV_COLOR_HEX3(0x666);
    roller_bg.body.border.opa = LV_OPA_30;
    roller_bg.text.opa = LV_OPA_70;
    roller_bg.text.color = lv_color_hsv_to_rgb(_hue, 20, 70);
    roller_bg.body.shadow.width = 0;

    lv_style_copy(&roller_sel, &panel);
    roller_sel.body.empty = 1;
    roller_sel.body.radius = 0;
    roller_sel.text.opa = LV_OPA_COVER;
    roller_sel.text.color = lv_color_hsv_to_rgb(_hue, 70, 95);

    theme.roller.bg = &roller_bg;
    theme.roller.sel = &roller_sel;
#endif
}

static void tabview_init(void)
{
#if USE_LV_TABVIEW != 0
    static lv_style_t tab_rel, tab_pr, tab_trel, tab_tpr, tab_indic;
    lv_style_copy(&tab_rel, &def);
    tab_rel.body.main_color = LV_COLOR_HEX3(0x666);
    tab_rel.body.grad_color = LV_COLOR_HEX3(0x666);
    tab_rel.body.padding.hor = 0;
    tab_rel.body.padding.ver = LV_DPI / 6;
    tab_rel.body.padding.inner = 0;
    tab_rel.body.border.width = 1;
    tab_rel.body.border.color = LV_COLOR_SILVER;
    tab_rel.body.border.opa = LV_OPA_40;
    tab_rel.text.color = LV_COLOR_HEX3(0xDDD);
    tab_rel.text.font = _font;

    lv_style_copy(&tab_pr, &tab_rel);
    tab_pr.body.main_color = LV_COLOR_HEX3(0x444);
    tab_pr.body.grad_color = LV_COLOR_HEX3(0x444);

    lv_style_copy(&tab_trel, &def);
    tab_trel.body.empty = 1;
    tab_trel.body.padding.hor = 0;
    tab_trel.body.padding.ver = LV_DPI / 6;
    tab_trel.body.padding.inner = 0;
    tab_trel.body.border.width = 1;
    tab_trel.body.border.color = LV_COLOR_SILVER;
    tab_trel.body.border.opa = LV_OPA_40;
    tab_trel.text.color = lv_color_hsv_to_rgb(_hue, 10, 94);
    tab_trel.text.font = _font;

    lv_style_copy(&tab_tpr, &def);
    tab_tpr.body.main_color = LV_COLOR_GRAY;
    tab_tpr.body.grad_color = LV_COLOR_GRAY;
    tab_tpr.body.padding.hor = 0;
    tab_tpr.body.padding.ver = LV_DPI / 6;
    tab_tpr.body.padding.inner = 0;
    tab_tpr.body.border.width = 1;
    tab_tpr.body.border.color = LV_COLOR_SILVER;
    tab_tpr.body.border.opa = LV_OPA_40;
    tab_tpr.text.color = lv_color_hsv_to_rgb(_hue, 10, 94);
    tab_tpr.text.font = _font;

    lv_style_copy(&tab_indic, &def);
    tab_indic.body.border.width = 0;
    tab_indic.body.main_color = lv_color_hsv_to_rgb(_hue, 80, 87);
    tab_indic.body.grad_color = lv_color_hsv_to_rgb(_hue, 80, 87);
    tab_indic.body.padding.inner = LV_DPI / 10; /*Indicator height*/

    theme.tabview.bg = &bg;
    theme.tabview.indic = &tab_indic;
    theme.tabview.btn.bg = &lv_style_transp_tight;
    theme.tabview.btn.rel = &tab_rel;
    theme.tabview.btn.pr = &tab_pr;
    theme.tabview.btn.tgl_rel = &tab_trel;
    theme.tabview.btn.tgl_pr = &tab_tpr;
#endif
}

static void win_init(void)
{
#if USE_LV_WIN != 0
    static lv_style_t win_header;

    lv_style_copy(&win_header, &def);
    win_header.body.radius = 0;
    win_header.body.padding.hor = LV_DPI / 12;
    win_header.body.padding.ver = LV_DPI / 20;
    win_header.body.main_color = lv_color_hsv_to_rgb(_hue, 20, 50);
    win_header.body.grad_color = win_header.body.main_color;
    win_header.body.border.opa = panel.body.border.opa;
    win_header.body.border.width = panel.body.border.width;
    win_header.body.border.color = lv_color_hsv_to_rgb(_hue, 20, 80);
    win_header.body.border.part = LV_BORDER_BOTTOM;
    win_header.text.color = lv_color_hsv_to_rgb(_hue, 5, 100);

    theme.win.bg = &bg;
    theme.win.sb = &sb;
    theme.win.header = &win_header;
    theme.win.content.bg = &lv_style_transp;
    theme.win.content.scrl = &lv_style_transp;
    theme.win.btn.rel = &btn_rel;
    theme.win.btn.pr = &btn_pr;
#endif
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize the alien theme
 * @param hue [0..360] hue value from HSV color space to define the theme's base color
 * @param font pointer to a font (NULL to use the default)
 * @return pointer to the initialized theme
 */
lv_theme_t * lv_theme_alien_init(uint16_t hue, lv_font_t *font)
{
    if(font == NULL) font = LV_FONT_DEFAULT;

    _hue = hue;
    _font = font;

    /*For backward compatibility initialize all theme elements with a default style */
    uint16_t i;
    lv_style_t **style_p = (lv_style_t**) &theme;
    for(i = 0; i < sizeof(lv_theme_t) / sizeof(lv_style_t*); i++) {
        *style_p = &def;
        style_p++;
    }

    basic_init();
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
    chart_init();
    cb_init();
    btnm_init();
    kb_init();
    mbox_init();
    page_init();
    ta_init();
    list_init();
    ddlist_init();
    roller_init();
    tabview_init();
    win_init();

    return &theme;
}

/**
 * Get a pointer to the theme
 * @return pointer to the theme
 */
lv_theme_t * lv_theme_get_alien(void)
{
    return &theme;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif

