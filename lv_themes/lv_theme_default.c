/**
 * @file lv_theme_default.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_theme.h"

#if USE_LV_THEME_DEFAULT

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
static lv_style_t plain_bordered;
static lv_style_t label_prim;
static lv_style_t label_sec;
static lv_style_t label_hint;
static lv_style_t slider_bg;
static lv_style_t sw_bg;
static lv_style_t lmeter;

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
    lv_style_copy(&def, &lv_style_pretty);  /*Initialize the default style*/

    lv_style_copy(&sb, &lv_style_pretty_color);
    sb.body.grad_color = sb.body.main_color;
    sb.body.padding.hor = sb.body.padding.hor / 2;      /*Make closer to the edges*/
    sb.body.padding.ver = sb.body.padding.ver / 2;

    lv_style_copy(&plain_bordered, &lv_style_plain);
    plain_bordered.body.border.width = 2;
    plain_bordered.body.border.color = LV_COLOR_HEX3(0xbbb);

    theme.bg = &lv_style_plain;
    theme.panel = &lv_style_pretty;

}

static void btn_init(void)
{
#if USE_LV_BTN != 0
    theme.btn.rel = &lv_style_btn_rel;
    theme.btn.pr = &lv_style_btn_pr;
    theme.btn.tgl_rel = &lv_style_btn_tgl_rel;
    theme.btn.tgl_pr =  &lv_style_btn_tgl_pr;
    theme.btn.ina =  &lv_style_btn_ina;
#endif
}

static void label_init(void)
{
#if USE_LV_LABEL != 0

    lv_style_copy(&label_prim, &lv_style_plain);
    lv_style_copy(&label_sec, &lv_style_plain);
    lv_style_copy(&label_hint, &lv_style_plain);

    label_prim.text.color = LV_COLOR_HEX3(0x111);
    label_sec.text.color = LV_COLOR_HEX3(0x888);
    label_hint.text.color = LV_COLOR_HEX3(0xaaa);


    theme.label.prim = &label_prim;
    theme.label.sec = &label_sec;
    theme.label.hint = &label_hint;
#endif
}


static void img_init(void)
{
#if USE_LV_IMG != 0


    theme.img.light = &def;
    theme.img.dark = &def;
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
    led.body.border.width = LV_DPI / 30;
    led.body.border.opa = LV_OPA_30;
    led.body.shadow.color = led.body.main_color;


    theme.led = &led;
#endif
}

static void bar_init(void)
{
#if USE_LV_BAR

    theme.bar.bg = &lv_style_pretty;
    theme.bar.indic = &lv_style_pretty_color;
#endif
}

static void slider_init(void)
{
#if USE_LV_SLIDER != 0
    lv_style_copy(&slider_bg, &lv_style_pretty);
    slider_bg.body.padding.hor = LV_DPI / 20;
    slider_bg.body.padding.ver = LV_DPI / 20;

    theme.slider.bg = &slider_bg;
    theme.slider.indic = &lv_style_pretty_color;
    theme.slider.knob = &lv_style_pretty;
#endif
}

static void sw_init(void)
{
#if USE_LV_SW != 0
    lv_style_copy(&sw_bg, &lv_style_pretty);
    sw_bg.body.padding.hor = 3;
    sw_bg.body.padding.ver = 3;

    theme.sw.bg = &sw_bg;
    theme.sw.indic = &lv_style_pretty_color;
    theme.sw.knob_off = &lv_style_pretty;
    theme.sw.knob_on = &lv_style_pretty;
#endif
}


static void lmeter_init(void)
{
#if USE_LV_LMETER != 0

    lv_style_copy(&lmeter, &lv_style_pretty_color);
    lmeter.line.color = LV_COLOR_HEX3(0xddd);
    lmeter.line.width = 2;
    lmeter.body.main_color = lv_color_mix(lmeter.body.main_color, LV_COLOR_WHITE, LV_OPA_50);
    lmeter.body.grad_color = lv_color_mix(lmeter.body.grad_color, LV_COLOR_BLACK, LV_OPA_50);

    theme.lmeter = &lmeter;
#endif
}

static void gauge_init(void)
{
#if USE_LV_GAUGE != 0
    static lv_style_t gauge;
    lv_style_copy(&gauge, &lmeter);
    gauge.line.color = lmeter.body.grad_color;
    gauge.line.width = 2;
    gauge.body.main_color = LV_COLOR_HEX3(0x888);
    gauge.body.grad_color = lmeter.body.main_color;
    gauge.text.color = LV_COLOR_HEX3(0x888);

    theme.gauge = &gauge;
#endif
}

static void chart_init(void)
{
#if USE_LV_CHART


    theme.chart = &lv_style_pretty;
#endif
}

static void cb_init(void)
{
#if USE_LV_CB != 0


    theme.cb.bg = &lv_style_transp;
    theme.cb.box.rel = &lv_style_pretty;
    theme.cb.box.pr = &lv_style_btn_pr;
    theme.cb.box.tgl_rel = &lv_style_btn_tgl_rel;
    theme.cb.box.tgl_pr = &lv_style_btn_tgl_pr;
    theme.cb.box.ina = &lv_style_btn_ina;
#endif
}


static void btnm_init(void)
{
#if USE_LV_BTNM


    theme.btnm.bg = &lv_style_pretty;
    theme.btnm.btn.rel = &lv_style_btn_rel;
    theme.btnm.btn.pr = &lv_style_btn_pr;
    theme.btnm.btn.tgl_rel = &lv_style_btn_tgl_rel;
    theme.btnm.btn.tgl_pr = &lv_style_btn_tgl_pr;
    theme.btnm.btn.ina = &lv_style_btn_ina;
#endif
}

static void kb_init(void)
{
#if USE_LV_KB


    theme.kb.bg = &lv_style_pretty;
    theme.kb.btn.rel = &lv_style_btn_rel;
    theme.kb.btn.pr = &lv_style_btn_pr;
    theme.kb.btn.tgl_rel = &lv_style_btn_tgl_rel;
    theme.kb.btn.tgl_pr = &lv_style_btn_tgl_pr;
    theme.kb.btn.ina = &lv_style_btn_ina;
#endif

}

static void mbox_init(void)
{
#if USE_LV_MBOX


    theme.mbox.bg = &lv_style_pretty;
    theme.mbox.btn.bg = &lv_style_transp;
    theme.mbox.btn.rel = &lv_style_btn_rel;
    theme.mbox.btn.pr = &lv_style_btn_tgl_pr;
#endif
}

static void page_init(void)
{
#if USE_LV_PAGE


    theme.page.bg = &lv_style_pretty;
    theme.page.scrl = &lv_style_transp_tight;
    theme.page.sb = &sb;
#endif
}

static void ta_init(void)
{
#if USE_LV_TA


    theme.ta.area = &lv_style_pretty;
    theme.ta.oneline = &lv_style_pretty;
    theme.ta.cursor = NULL;
    theme.ta.sb = &sb;
#endif
}

static void list_init(void)
{
#if USE_LV_LIST != 0

    theme.list.bg = &lv_style_pretty;
    theme.list.scrl = &lv_style_transp_fit;
    theme.list.sb = &sb;
    theme.list.btn.rel = &lv_style_btn_rel;
    theme.list.btn.pr = &lv_style_btn_pr;
    theme.list.btn.tgl_rel = &lv_style_btn_tgl_rel;
    theme.list.btn.tgl_pr = &lv_style_btn_tgl_pr;
    theme.list.btn.ina = &lv_style_btn_ina;
#endif
}

static void ddlist_init(void)
{
#if USE_LV_DDLIST != 0


    theme.ddlist.bg = &lv_style_pretty;
    theme.ddlist.sel = &lv_style_plain_color;
    theme.ddlist.sb = &sb;
#endif
}

static void roller_init(void)
{
#if USE_LV_ROLLER != 0


    theme.roller.bg = &lv_style_pretty;
    theme.roller.sel = &lv_style_plain_color;
#endif
}

static void tabview_init(void)
{
#if USE_LV_TABVIEW != 0


    theme.tabview.bg = &plain_bordered;
    theme.tabview.indic = &lv_style_plain_color;
    theme.tabview.btn.bg = &lv_style_transp;
    theme.tabview.btn.rel = &lv_style_btn_rel;
    theme.tabview.btn.pr = &lv_style_btn_pr;
    theme.tabview.btn.tgl_rel = &lv_style_btn_tgl_rel;
    theme.tabview.btn.tgl_pr = &lv_style_btn_tgl_pr;
#endif
}


static void win_init(void)
{
#if USE_LV_WIN != 0


    theme.win.bg = &plain_bordered;
    theme.win.sb = &sb;
    theme.win.header = &lv_style_plain_color;
    theme.win.content.bg = &lv_style_transp;
    theme.win.content.scrl = &lv_style_transp;
    theme.win.btn.rel = &lv_style_btn_rel;
    theme.win.btn.pr = &lv_style_btn_pr;
#endif
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/



/**
 * Initialize the default theme
 * @param hue [0..360] hue value from HSV color space to define the theme's base color
 * @param font pointer to a font (NULL to use the default)
 * @return pointer to the initialized theme
 */
lv_theme_t * lv_theme_default_init(uint16_t hue, lv_font_t * font)
{
    if(font == NULL) font = LV_FONT_DEFAULT;

    _hue = hue;
    _font = font;

    /*For backward compatibility initialize all theme elements with a default style */
    uint16_t i;
    lv_style_t ** style_p = (lv_style_t **) &theme;
    for(i = 0; i < sizeof(lv_theme_t) / sizeof(lv_style_t *); i++) {
        *style_p = &def;
        style_p++;
    }

    basic_init();
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
lv_theme_t * lv_theme_get_default(void)
{
    return &theme;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif

