/**
 * @file lv_theme_default.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_theme.h"

#if LV_USE_THEME_DEFAULT

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
static lv_style_t scr;

/*Static style definitions*/
static lv_style_t sb;
static lv_style_t plain_bordered;
static lv_style_t label_prim;
static lv_style_t label_sec;
static lv_style_t label_hint;

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
    lv_style_copy(&def, &lv_style_pretty); /*Initialize the default style*/

    lv_style_copy(&scr, &def);
    scr.body.padding.bottom = 0;
    scr.body.padding.top    = 0;
    scr.body.padding.left   = 0;
    scr.body.padding.right  = 0;

    lv_style_copy(&sb, &lv_style_pretty_color);
    sb.body.grad_color     = sb.body.main_color;
    sb.body.padding.right  = sb.body.padding.right / 2; /*Make closer to the edges*/
    sb.body.padding.bottom = sb.body.padding.bottom / 2;

    lv_style_copy(&plain_bordered, &lv_style_plain);
    plain_bordered.body.border.width = 2;
    plain_bordered.body.border.color = lv_color_hex3(0xbbb);

    theme.style.bg    = &lv_style_plain;
    theme.style.scr   = &scr;
    theme.style.panel = &lv_style_pretty;
}

static void btn_init(void)
{
#if LV_USE_BTN != 0
    theme.style.btn.rel     = &lv_style_btn_rel;
    theme.style.btn.pr      = &lv_style_btn_pr;
    theme.style.btn.tgl_rel = &lv_style_btn_tgl_rel;
    theme.style.btn.tgl_pr  = &lv_style_btn_tgl_pr;
    theme.style.btn.ina     = &lv_style_btn_ina;
#endif
}

static void label_init(void)
{
#if LV_USE_LABEL != 0

    lv_style_copy(&label_prim, &lv_style_plain);
    lv_style_copy(&label_sec, &lv_style_plain);
    lv_style_copy(&label_hint, &lv_style_plain);

    label_prim.text.color = lv_color_hex3(0x111);
    label_sec.text.color  = lv_color_hex3(0x888);
    label_hint.text.color = lv_color_hex3(0xaaa);

    theme.style.label.prim = &label_prim;
    theme.style.label.sec  = &label_sec;
    theme.style.label.hint = &label_hint;
#endif
}

static void img_init(void)
{
#if LV_USE_IMG != 0

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

    lv_style_copy(&led, &lv_style_pretty_color);
    led.body.shadow.width = LV_DPI / 10;
    led.body.radius       = LV_RADIUS_CIRCLE;
    led.body.border.width = LV_DPI / 30;
    led.body.border.opa   = LV_OPA_30;
    led.body.shadow.color = led.body.main_color;

    theme.style.led = &led;
#endif
}

static void bar_init(void)
{
#if LV_USE_BAR

    theme.style.bar.bg    = &lv_style_pretty;
    theme.style.bar.indic = &lv_style_pretty_color;
#endif
}

static void slider_init(void)
{
#if LV_USE_SLIDER != 0
	static lv_style_t slider_bg;
    lv_style_copy(&slider_bg, &lv_style_pretty);
    slider_bg.body.padding.left   = LV_DPI / 20;
    slider_bg.body.padding.right  = LV_DPI / 20;
    slider_bg.body.padding.top    = LV_DPI / 20;
    slider_bg.body.padding.bottom = LV_DPI / 20;

    theme.style.slider.bg    = &slider_bg;
    theme.style.slider.indic = &lv_style_pretty_color;
    theme.style.slider.knob  = &lv_style_pretty;
#endif
}

static void sw_init(void)
{
#if LV_USE_SW != 0
	static lv_style_t sw_bg;
    lv_style_copy(&sw_bg, &lv_style_pretty);
    sw_bg.body.padding.left   = 3;
    sw_bg.body.padding.right  = 3;
    sw_bg.body.padding.top    = 3;
    sw_bg.body.padding.bottom = 3;

    theme.style.sw.bg       = &sw_bg;
    theme.style.sw.indic    = &lv_style_pretty_color;
    theme.style.sw.knob_off = &lv_style_pretty;
    theme.style.sw.knob_on  = &lv_style_pretty;
#endif
}

static void lmeter_init(void)
{
#if LV_USE_LMETER != 0
	static lv_style_t lmeter;
    lv_style_copy(&lmeter, &lv_style_pretty_color);
    lmeter.line.color      = lv_color_hex3(0xddd);
    lmeter.line.width      = 2;
    lmeter.body.main_color = lv_color_mix(lmeter.body.main_color, LV_COLOR_WHITE, LV_OPA_50);
    lmeter.body.grad_color = lv_color_mix(lmeter.body.grad_color, LV_COLOR_BLACK, LV_OPA_50);

    theme.style.lmeter = &lmeter;
#endif
}

static void gauge_init(void)
{
#if LV_USE_GAUGE != 0
    static lv_style_t gauge;
    lv_style_copy(&gauge, theme.style.lmeter);
    gauge.line.color      = theme.style.lmeter->body.grad_color;
    gauge.line.width      = 2;
    gauge.body.main_color = lv_color_hex3(0x888);
    gauge.body.grad_color = theme.style.lmeter->body.main_color;
    gauge.text.color      = lv_color_hex3(0x888);

    theme.style.gauge = &gauge;
#endif
}

static void chart_init(void)
{
#if LV_USE_CHART

    theme.style.chart = &lv_style_pretty;
#endif
}

static void cb_init(void)
{
#if LV_USE_CB != 0

    theme.style.cb.bg          = &lv_style_transp;
    theme.style.cb.box.rel     = &lv_style_pretty;
    theme.style.cb.box.pr      = &lv_style_btn_pr;
    theme.style.cb.box.tgl_rel = &lv_style_btn_tgl_rel;
    theme.style.cb.box.tgl_pr  = &lv_style_btn_tgl_pr;
    theme.style.cb.box.ina     = &lv_style_btn_ina;
#endif
}

static void btnm_init(void)
{
#if LV_USE_BTNM

    theme.style.btnm.bg          = &lv_style_pretty;
    theme.style.btnm.btn.rel     = &lv_style_btn_rel;
    theme.style.btnm.btn.pr      = &lv_style_btn_pr;
    theme.style.btnm.btn.tgl_rel = &lv_style_btn_tgl_rel;
    theme.style.btnm.btn.tgl_pr  = &lv_style_btn_tgl_pr;
    theme.style.btnm.btn.ina     = &lv_style_btn_ina;
#endif
}

static void kb_init(void)
{
#if LV_USE_KB

    theme.style.kb.bg          = &lv_style_pretty;
    theme.style.kb.btn.rel     = &lv_style_btn_rel;
    theme.style.kb.btn.pr      = &lv_style_btn_pr;
    theme.style.kb.btn.tgl_rel = &lv_style_btn_tgl_rel;
    theme.style.kb.btn.tgl_pr  = &lv_style_btn_tgl_pr;
    theme.style.kb.btn.ina     = &lv_style_btn_ina;
#endif
}

static void mbox_init(void)
{
#if LV_USE_MBOX

    theme.style.mbox.bg      = &lv_style_pretty;
    theme.style.mbox.btn.bg  = &lv_style_transp;
    theme.style.mbox.btn.rel = &lv_style_btn_rel;
    theme.style.mbox.btn.pr  = &lv_style_btn_tgl_pr;
#endif
}

static void page_init(void)
{
#if LV_USE_PAGE

    theme.style.page.bg   = &lv_style_pretty;
    theme.style.page.scrl = &lv_style_transp_tight;
    theme.style.page.sb   = &sb;
#endif
}

static void ta_init(void)
{
#if LV_USE_TA

    theme.style.ta.area    = &lv_style_pretty;
    theme.style.ta.oneline = &lv_style_pretty;
    theme.style.ta.cursor  = NULL;
    theme.style.ta.sb      = &sb;
#endif
}

static void list_init(void)
{
#if LV_USE_LIST != 0

    theme.style.list.bg          = &lv_style_pretty;
    theme.style.list.scrl        = &lv_style_transp_fit;
    theme.style.list.sb          = &sb;
    theme.style.list.btn.rel     = &lv_style_btn_rel;
    theme.style.list.btn.pr      = &lv_style_btn_pr;
    theme.style.list.btn.tgl_rel = &lv_style_btn_tgl_rel;
    theme.style.list.btn.tgl_pr  = &lv_style_btn_tgl_pr;
    theme.style.list.btn.ina     = &lv_style_btn_ina;
#endif
}

static void ddlist_init(void)
{
#if LV_USE_DDLIST != 0

    theme.style.ddlist.bg  = &lv_style_pretty;
    theme.style.ddlist.sel = &lv_style_plain_color;
    theme.style.ddlist.sb  = &sb;
#endif
}

static void roller_init(void)
{
#if LV_USE_ROLLER != 0

    theme.style.roller.bg  = &lv_style_pretty;
    theme.style.roller.sel = &lv_style_plain_color;
#endif
}

static void tabview_init(void)
{
#if LV_USE_TABVIEW != 0

    theme.style.tabview.bg          = &plain_bordered;
    theme.style.tabview.indic       = &lv_style_plain_color;
    theme.style.tabview.btn.bg      = &lv_style_transp;
    theme.style.tabview.btn.rel     = &lv_style_btn_rel;
    theme.style.tabview.btn.pr      = &lv_style_btn_pr;
    theme.style.tabview.btn.tgl_rel = &lv_style_btn_tgl_rel;
    theme.style.tabview.btn.tgl_pr  = &lv_style_btn_tgl_pr;
#endif
}

static void table_init(void)
{
#if LV_USE_TABLE != 0
    theme.style.table.bg   = &lv_style_transp_tight;
    theme.style.table.cell = &lv_style_plain;
#endif
}

static void win_init(void)
{
#if LV_USE_WIN != 0

    theme.style.win.bg      = &plain_bordered;
    theme.style.win.sb      = &sb;
    theme.style.win.header  = &lv_style_plain_color;
    theme.style.win.content = &lv_style_transp;
    theme.style.win.btn.rel = &lv_style_btn_rel;
    theme.style.win.btn.pr  = &lv_style_btn_pr;
#endif
}

#if LV_USE_GROUP

static void style_mod(lv_group_t * group, lv_style_t * style)
{
    (void)group; /*Unused*/
#if LV_COLOR_DEPTH != 1
    /*Make the style to be a little bit orange*/
    style->body.border.opa   = LV_OPA_COVER;
    style->body.border.color = LV_COLOR_ORANGE;

    /*If not empty or has border then emphasis the border*/
    if(style->body.opa != LV_OPA_TRANSP || style->body.border.width != 0) style->body.border.width = LV_DPI / 20;

    style->body.main_color   = lv_color_mix(style->body.main_color, LV_COLOR_ORANGE, LV_OPA_70);
    style->body.grad_color   = lv_color_mix(style->body.grad_color, LV_COLOR_ORANGE, LV_OPA_70);
    style->body.shadow.color = lv_color_mix(style->body.shadow.color, LV_COLOR_ORANGE, LV_OPA_60);

    style->text.color = lv_color_mix(style->text.color, LV_COLOR_ORANGE, LV_OPA_70);
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
 * Initialize the default theme
 * @param hue [0..360] hue value from HSV color space to define the theme's base color
 * @param font pointer to a font (NULL to use the default)
 * @return pointer to the initialized theme
 */
lv_theme_t * lv_theme_default_init(uint16_t hue, lv_font_t * font)
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
lv_theme_t * lv_theme_get_default(void)
{
    return &theme;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif
