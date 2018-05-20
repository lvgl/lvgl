/**
 * @file lv_theme_zen.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_theme.h"


#if USE_LV_THEME_ZEN

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
    static lv_style_t bg;
    static lv_style_t panel;

    lv_style_copy(&def, &lv_style_pretty);  /*Initialize the default style*/
    def.body.border.opa = LV_OPA_COVER;
    def.text.font = _font;
    def.text.color = LV_COLOR_HEX3(0x444);

    lv_style_copy(&bg, &def);
    bg.body.main_color = LV_COLOR_WHITE;
    bg.body.grad_color = LV_COLOR_WHITE;
    bg.body.radius = 0;
    bg.body.border.width = 0;
    bg.body.shadow.width = 0;

    lv_style_copy(&panel, &bg);
    panel.body.radius = LV_DPI / 10;
    panel.body.border.width = 2;
    panel.body.border.color = lv_color_hsv_to_rgb(_hue, 30, 90);
    panel.body.border.opa = LV_OPA_COVER;
    panel.body.shadow.width = 4;
    panel.body.shadow.color = LV_COLOR_HEX3(0xddd);
    panel.body.padding.hor = LV_DPI / 6;
    panel.body.padding.ver = LV_DPI / 8;
    panel.body.padding.inner = LV_DPI / 10;

    lv_style_copy(&sb, &def);
    sb.body.main_color = lv_color_hsv_to_rgb(_hue, 30, 90);
    sb.body.grad_color = sb.body.main_color;
    sb.body.border.width = 0;
    sb.body.radius = LV_RADIUS_CIRCLE;
    sb.body.padding.inner = LV_DPI / 10;

    theme.bg = &bg;
    theme.panel = &panel;
}

static void cont_init(void)
{
#if USE_LV_CONT != 0


    theme.cont = theme.panel;
#endif
}

static void btn_init(void)
{
#if USE_LV_BTN != 0
    static lv_style_t rel, pr, tgl_pr, ina;
    lv_style_copy(&rel, &def);
    rel.body.empty = 1;
    rel.body.radius = LV_RADIUS_CIRCLE;
    rel.body.border.width = 2;
    rel.body.border.color = lv_color_hsv_to_rgb(_hue, 40, 90);
    rel.body.border.opa = LV_OPA_COVER;
    rel.body.shadow.width = 4;
    rel.body.shadow.color = LV_COLOR_HEX3(0xddd);
    rel.body.padding.hor = LV_DPI / 4;
    rel.body.padding.ver = LV_DPI / 8;
    rel.text.color = lv_color_hsv_to_rgb(_hue, 40, 90);

    lv_style_copy(&pr, &rel);
    pr.body.border.color = lv_color_hsv_to_rgb(_hue, 40, 60);
    pr.text.color = lv_color_hsv_to_rgb(_hue, 40, 60);
    pr.body.shadow.width = 0;

    lv_style_copy(&tgl_pr, &pr);
    tgl_pr.body.border.color = lv_color_hsv_to_rgb(_hue, 40, 50);
    tgl_pr.text.color = lv_color_hsv_to_rgb(_hue, 40, 50);

    lv_style_copy(&ina, &tgl_pr);
    ina.body.border.color = LV_COLOR_HEX3(0xbbb);
    ina.text.color = LV_COLOR_HEX3(0xbbb);

    theme.btn.rel = &rel;
    theme.btn.pr = &pr;
    theme.btn.tgl_rel = &pr;
    theme.btn.tgl_pr =  &tgl_pr;
    theme.btn.ina =  &ina;
#endif
}


static void label_init(void)
{
#if USE_LV_LABEL != 0
    static lv_style_t prim, sec, hint;
    lv_style_copy(&prim, &def);
    lv_style_copy(&sec, &def);
    lv_style_copy(&hint, &def);

    prim.text.color = LV_COLOR_HEX3(0x555);
    sec.text.color = lv_color_hsv_to_rgb(_hue, 50, 80);
    hint.text.color = lv_color_hsv_to_rgb(_hue, 25, 85);

    theme.label.prim = &prim;
    theme.label.sec = &sec;
    theme.label.hint = &hint;
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
    img_light.image.color = lv_color_hsv_to_rgb(_hue, 85, 55);
    img_light.image.intense = LV_OPA_80;

    theme.img.light = &img_light;
    theme.img.dark = &img_dark;
#endif
}

static void line_init(void)
{
#if USE_LV_LINE != 0


    theme.line.decor = &def;
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
    led.body.main_color = lv_color_hsv_to_rgb(_hue, 60, 100);
    led.body.grad_color = lv_color_hsv_to_rgb(_hue, 60, 40);
    led.body.border.color = lv_color_hsv_to_rgb(_hue, 60, 60);
    led.body.shadow.color = lv_color_hsv_to_rgb(_hue, 80, 100);

    theme.led = &led;
#endif
}

static void bar_init(void)
{
#if USE_LV_BAR
    static lv_style_t bg, indic;

    lv_style_copy(&bg, &def);
    bg.body.empty = 1;
    bg.body.radius = LV_RADIUS_CIRCLE;
    bg.body.border.width = 2;
    bg.body.border.opa = LV_OPA_COVER;
    bg.body.border.color = lv_color_hsv_to_rgb(_hue, 40, 90);

    lv_style_copy(&indic, &def);
    indic.body.radius = LV_RADIUS_CIRCLE;
    indic.body.main_color = lv_color_hsv_to_rgb(_hue, 40, 90);
    indic.body.grad_color = indic.body.main_color;
    indic.body.border.width = 0;
    indic.body.padding.hor = LV_DPI / 20;
    indic.body.padding.ver = LV_DPI / 20;


    theme.bar.bg = &bg;
    theme.bar.indic = &indic;
#endif
}

static void slider_init(void)
{
#if USE_LV_SLIDER != 0
    static lv_style_t knob;

    lv_style_copy(&knob, &def);
    knob.body.main_color = theme.bar.indic->body.main_color;
    knob.body.grad_color = knob.body.main_color;
    knob.body.radius = LV_RADIUS_CIRCLE;
    knob.body.border.width = 0;

    theme.slider.bg = theme.bar.bg;
    theme.slider.indic = theme.bar.indic;
    theme.slider.knob = &knob;
#endif
}

static void sw_init(void)
{
#if USE_LV_SW != 0
    static lv_style_t indic;

    lv_style_copy(&indic, theme.slider.indic);
    indic.body.radius = LV_RADIUS_CIRCLE;
    indic.body.main_color = lv_color_hsv_to_rgb(_hue, 15, 95);
    indic.body.grad_color = indic.body.main_color;
    indic.body.border.width = theme.slider.bg->body.border.width;
    indic.body.border.color = theme.slider.bg->body.border.color;
    indic.body.border.opa = theme.slider.bg->body.border.opa;
    indic.body.padding.hor = 0;
    indic.body.padding.ver = 0;



    theme.sw.bg = theme.slider.bg;
    theme.sw.indic = &indic;
    theme.sw.knob_off = theme.slider.knob;
    theme.sw.knob_on = theme.slider.knob;
#endif
}


static void lmeter_init(void)
{
#if USE_LV_LMETER != 0
    static lv_style_t lmeter;

    lv_style_copy(&lmeter, &def);
    lmeter.line.color = LV_COLOR_HEX3(0xddd);
    lmeter.line.width = 2;
    lmeter.body.main_color = lv_color_hsv_to_rgb(_hue, 80, 70);
    lmeter.body.grad_color = lmeter.body.main_color;
    lmeter.body.padding.hor = LV_DPI / 8;

    theme.lmeter = &lmeter;
#endif
}

static void gauge_init(void)
{
#if USE_LV_GAUGE != 0
    static lv_style_t gauge;

    lv_style_copy(&gauge, &def);
    gauge.line.color = lv_color_hsv_to_rgb(_hue, 50, 70);
    gauge.line.width = 1;
    gauge.body.main_color = LV_COLOR_HEX3(0x999);
    gauge.body.grad_color = gauge.body.main_color;
    gauge.body.padding.hor = LV_DPI / 16;
    gauge.body.border.color = LV_COLOR_HEX3(0x666);     /*Needle middle color*/

    theme.gauge = &gauge;
#endif
}

static void chart_init(void)
{
#if USE_LV_CHART


    theme.chart = theme.panel;
#endif
}

static void cb_init(void)
{
#if USE_LV_CB != 0
    static lv_style_t rel ,pr, tgl_rel, tgl_pr, ina;
    lv_style_copy(&rel, &def);
    rel.body.radius = LV_DPI / 20;
    rel.body.shadow.width = 0;
    rel.body.border.width = 3;
    rel.body.border.opa = LV_OPA_COVER;
    rel.body.border.color = lv_color_hsv_to_rgb(_hue, 35, 80);
    rel.body.main_color = LV_COLOR_HEX3(0xfff);
    rel.body.grad_color = rel.body.main_color;


    lv_style_copy(&pr, &rel);
    pr.body.border.color = lv_color_hsv_to_rgb(_hue, 35, 70);


    lv_style_copy(&tgl_rel, &rel);
    tgl_rel.body.border.color = lv_color_hsv_to_rgb(_hue, 45, 80);
    tgl_rel.body.main_color = lv_color_hsv_to_rgb(_hue, 40, 90);
    tgl_rel.body.grad_color = tgl_rel.body.main_color;

    lv_style_copy(&tgl_pr, &rel);
    tgl_pr.body.border.color = lv_color_hsv_to_rgb(_hue, 45, 70);
    tgl_pr.body.main_color = lv_color_hsv_to_rgb(_hue, 40, 80);
    tgl_pr.body.grad_color = tgl_pr.body.main_color;


    lv_style_copy(&ina, &rel);
    ina.body.border.color = LV_COLOR_HEX3(0xaaa);


    theme.cb.bg = &lv_style_transp;
    theme.cb.box.rel = &rel;
    theme.cb.box.pr = &pr;
    theme.cb.box.tgl_rel = &tgl_rel;
    theme.cb.box.tgl_pr = &tgl_pr;
    theme.cb.box.ina = &ina;
#endif
}


static void btnm_init(void)
{
#if USE_LV_BTNM
    static lv_style_t bg, rel ,pr, tgl_rel, tgl_pr, ina;

    lv_style_copy(&bg, &lv_style_transp);
    bg.glass = 0;
    bg.body.padding.hor = 0;
    bg.body.padding.ver = 0;
    bg.body.padding.inner = LV_DPI / 15;
    bg.text.font = _font;

    lv_style_copy(&rel, &def);
    rel.body.empty = 1;
    rel.body.border.width = 0;

    lv_style_copy(&pr, &def);
    pr.body.empty = 1;
    pr.body.radius = LV_DPI / 1;
    pr.body.border.width = 2;
    pr.body.border.color = lv_color_hsv_to_rgb(_hue, 40, 60);
    pr.body.border.opa = LV_OPA_COVER;
    pr.text.color = lv_color_hsv_to_rgb(_hue, 40, 60);

    lv_style_copy(&tgl_rel, &pr);
    tgl_rel.body.empty = 0;
    tgl_rel.body.main_color = lv_color_hsv_to_rgb(_hue, 15, 95);
    tgl_rel.body.grad_color = tgl_rel.body.main_color;
    tgl_rel.body.border.width = 0;
    tgl_rel.text.color = lv_color_hsv_to_rgb(_hue, 60, 40);

    lv_style_copy(&tgl_pr, &tgl_rel);
    tgl_pr.body.main_color = lv_color_hsv_to_rgb(_hue, 30, 70);
    tgl_pr.body.grad_color = tgl_pr.body.main_color;

    lv_style_copy(&ina, &pr);
    ina.body.main_color = LV_COLOR_HEX3(0x888);
    ina.body.grad_color = tgl_pr.body.main_color;
    ina.text.color = LV_COLOR_HEX3(0x888);;

    theme.btnm.bg = &bg;
    theme.btnm.btn.rel = &rel;
    theme.btnm.btn.pr = &pr;
    theme.btnm.btn.tgl_rel = &tgl_rel;
    theme.btnm.btn.tgl_pr = &tgl_pr;
    theme.btnm.btn.ina = &ina;
#endif
}

static void kb_init(void)
{
#if USE_LV_KB
    static lv_style_t bg, rel ,pr, tgl_rel, tgl_pr, ina;
    lv_style_copy(&bg, &def);
     bg.body.main_color = LV_COLOR_HEX3(0x666);
     bg.body.grad_color = bg.body.main_color;
     bg.body.padding.hor = 0;
     bg.body.padding.ver = 0;
     bg.body.padding.inner = 0;
     bg.body.radius = 0;
     bg.body.border.width = 0;

    lv_style_copy(&rel, &def);
    rel.body.empty = 1;
    rel.body.radius = 0;
    rel.body.border.width = 1;
    rel.body.border.color = LV_COLOR_HEX3(0x888);
    rel.body.border.opa = LV_OPA_COVER;
    rel.text.color = LV_COLOR_WHITE;

    lv_style_copy(&pr, &def);
    pr.body.main_color = LV_COLOR_HEX3(0xeee);
    pr.body.grad_color = pr.body.main_color;
    pr.body.border.color = LV_COLOR_HEX3(0x888);
    pr.body.border.width = 1;
    pr.body.border.opa = LV_OPA_COVER;
    pr.body.radius = 0;
    pr.text.color = LV_COLOR_HEX3(0x666);

    lv_style_copy(&tgl_rel, &pr);
    tgl_rel.body.main_color = LV_COLOR_HEX3(0x999);
    tgl_rel.body.grad_color = tgl_rel.body.main_color;
    tgl_rel.text.color = LV_COLOR_WHITE;


    lv_style_copy(&tgl_pr, &pr);
    tgl_pr.body.main_color = LV_COLOR_HEX3(0xbbb);
    tgl_pr.body.grad_color = tgl_pr.body.main_color;
    tgl_pr.text.color = LV_COLOR_HEX3(0xddd);

    lv_style_copy(&ina, &pr);
    ina.body.main_color = LV_COLOR_HEX3(0x777);
    ina.body.grad_color = ina.body.main_color;
    ina.text.color = LV_COLOR_HEX3(0xbbb);

    theme.kb.bg = &bg;
    theme.kb.btn.rel = &rel;
    theme.kb.btn.pr = &pr;
    theme.kb.btn.tgl_rel = &tgl_rel;
    theme.kb.btn.tgl_pr = &tgl_pr;
    theme.kb.btn.ina = &ina;
#endif

}

static void mbox_init(void)
{
#if USE_LV_MBOX
    static lv_style_t bg, rel, pr;
    lv_style_copy(&bg, theme.panel);
    bg.body.main_color = lv_color_hsv_to_rgb(_hue, 10, 95);
    bg.body.grad_color = bg.body.main_color;
    bg.text.color = lv_color_hsv_to_rgb(_hue, 40, 25);

    lv_style_copy(&rel, &def);
    rel.body.main_color = lv_color_hsv_to_rgb(_hue, 25, 85);
    rel.body.grad_color = rel.body.main_color;
    rel.body.radius = LV_RADIUS_CIRCLE;
    rel.body.border.width = 2;
    rel.body.border.color = lv_color_hsv_to_rgb(_hue, 30, 70);
    rel.body.padding.hor = LV_DPI / 4;
    rel.body.padding.ver = LV_DPI / 8;
    rel.text.color = bg.text.color;

    lv_style_copy(&pr, &rel);
    pr.body.border.color = lv_color_hsv_to_rgb(_hue, 30, 90);
    pr.text.color = lv_color_hsv_to_rgb(_hue, 40, 40);
    pr.body.main_color = lv_color_hsv_to_rgb(_hue, 20, 85);
    pr.body.grad_color = pr.body.main_color;


    theme.mbox.bg = &bg;
    theme.mbox.btn.bg = &lv_style_transp;
    theme.mbox.btn.rel = &rel;
    theme.mbox.btn.pr = &pr;
#endif
}

static void page_init(void)
{
#if USE_LV_PAGE


    theme.page.bg = theme.panel;
    theme.page.scrl = &lv_style_transp;
    theme.page.sb = &sb;
#endif
}

static void ta_init(void)
{
#if USE_LV_TA
    static lv_style_t oneline;
    lv_style_copy(&oneline, theme.panel);
    oneline.body.radius = LV_RADIUS_CIRCLE;
    oneline.body.padding.ver = LV_DPI / 10;
    oneline.body.shadow.width = 0;


    theme.ta.area = theme.panel;
    theme.ta.oneline = &oneline;
    theme.ta.cursor = NULL;     /*Let library to calculate the cursor's style*/
    theme.ta.sb = &def;
#endif
}

static void list_init(void)
{
#if USE_LV_LIST != 0
    static lv_style_t bg, rel, pr, tgl_rel, tgl_pr, ina;

    lv_style_copy(&bg, theme.panel);
    bg.body.padding.hor = 0;
    bg.body.padding.ver = 0;

    lv_style_copy(&rel, &def);
    rel.body.empty = 1;
    rel.body.border.width = 0;
    rel.body.padding.hor = LV_DPI / 8;
    rel.body.padding.ver = LV_DPI / 8;
    rel.text.color = LV_COLOR_HEX3(0x666);

    lv_style_copy(&pr, &rel);
    pr.text.color = theme.btn.pr->text.color;

    lv_style_copy(&tgl_rel, &rel);
    tgl_rel.text.color = lv_color_hsv_to_rgb(_hue, 50, 90);

    lv_style_copy(&tgl_pr, &rel);
    tgl_pr.text.color = theme.btn.tgl_pr->text.color;

    lv_style_copy(&ina, &rel);
    ina.text.color = theme.btn.ina->text.color;

    theme.list.sb = &sb;
    theme.list.bg = &bg;
    theme.list.scrl = &lv_style_transp_tight;
    theme.list.btn.rel = &rel;
    theme.list.btn.pr = &pr;
    theme.list.btn.tgl_rel = &tgl_rel;
    theme.list.btn.tgl_pr = &tgl_pr;
    theme.list.btn.ina = &ina;
#endif
}

static void ddlist_init(void)
{
#if USE_LV_DDLIST != 0
    static lv_style_t bg, sel;
    lv_style_copy(&bg, theme.panel);
    bg.text.line_space = LV_DPI / 8;
    bg.body.padding.hor = LV_DPI / 6;
    bg.body.padding.ver = LV_DPI / 8;
    bg.text.color = LV_COLOR_HEX3(0x666);

    lv_style_copy(&sel, &def);
    sel.body.empty = 1;
    sel.body.border.width = 0;
    sel.text.color = lv_color_hsv_to_rgb(_hue, 50, 80);

    theme.ddlist.bg = &bg;
    theme.ddlist.sel = &sel;
    theme.ddlist.sb = &def;
#endif
}

static void roller_init(void)
{
#if USE_LV_ROLLER != 0
    static lv_style_t bg, sel;
    lv_style_copy(&bg, &def);
    bg.body.border.width = 0;
    bg.body.empty = 1;
    bg.text.line_space = LV_DPI / 6;
    bg.text.color = LV_COLOR_HEX3(0x999);

    lv_style_copy(&sel, theme.panel);
    sel.body.radius = LV_RADIUS_CIRCLE;
    sel.body.empty = 1;

    theme.roller.bg = &bg;
    theme.roller.sel = &sel;
#endif
}

static void tabview_init(void)
{
#if USE_LV_TABVIEW != 0
    static lv_style_t btn_bg, indic, rel, pr, tgl_rel, tgl_pr;

    lv_style_copy(&btn_bg, &def);
    btn_bg.body.empty = 1;
    btn_bg.body.border.width = 2;
    btn_bg.body.border.part = LV_BORDER_BOTTOM;
    btn_bg.body.border.color = lv_color_hsv_to_rgb(_hue, 10, 90);

    lv_style_copy(&indic, &def);
    indic.body.padding.inner = LV_DPI / 16;
    indic.body.border.width = 0;
    indic.body.radius = LV_RADIUS_CIRCLE;
    indic.body.main_color = lv_color_hsv_to_rgb(_hue, 50, 80);
    indic.body.grad_color = indic.body.main_color;

    lv_style_copy(&rel, &def);
    rel.body.empty = 1;
    rel.body.border.width = 0;
    rel.text.color = LV_COLOR_HEX3(0x999);


    lv_style_copy(&pr, &rel);
    pr.text.color = LV_COLOR_HEX3(0x777);

    lv_style_copy(&tgl_rel, &rel);
    tgl_rel.text.color = lv_color_hsv_to_rgb(_hue, 50, 80);

    lv_style_copy(&tgl_pr, &rel);
    tgl_pr.text.color = lv_color_hsv_to_rgb(_hue, 50, 70);

    theme.tabview.bg = theme.bg;
    theme.tabview.indic = &indic;
    theme.tabview.btn.bg = &btn_bg;
    theme.tabview.btn.rel = &rel;
    theme.tabview.btn.pr = &pr;
    theme.tabview.btn.tgl_rel = &tgl_rel;
    theme.tabview.btn.tgl_pr = &tgl_pr;
#endif
}


static void win_init(void)
{
#if USE_LV_WIN != 0
    static lv_style_t header, rel, pr;

    lv_style_copy(&header, &def);
    header.body.empty = 1;
    header.body.border.width = 2;
    header.body.border.part = LV_BORDER_BOTTOM;
    header.body.border.color = lv_color_hsv_to_rgb(_hue, 10, 90);
    header.text.color = LV_COLOR_HEX3(0x666);

    lv_style_copy(&rel, &def);
    rel.body.empty = 1;
    rel.body.border.width = 0;
    rel.text.color =  LV_COLOR_HEX3(0x666);

    lv_style_copy(&pr, &rel);
    pr.text.color = LV_COLOR_HEX3(0x333);

    theme.win.bg = theme.panel;
    theme.win.sb = &sb;
    theme.win.header = &header;
    theme.win.content.bg = &lv_style_transp;
    theme.win.content.scrl = &lv_style_transp;
    theme.win.btn.rel = &rel;
    theme.win.btn.pr = &pr;
#endif
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/



/**
 * Initialize the zen theme
 * @param hue [0..360] hue value from HSV color space to define the theme's base color
 * @param font pointer to a font (NULL to use the default)
 * @return pointer to the initialized theme
 */
lv_theme_t * lv_theme_zen_init(uint16_t hue, lv_font_t *font)
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
lv_theme_t * lv_theme_get_zen(void)
{
    return &theme;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif

