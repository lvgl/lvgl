/**
 * @file lv_theme_templ.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_theme.h"

#if LV_USE_THEME_MONO

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
static lv_style_t light_plain;
static lv_style_t dark_plain;
static lv_style_t light_frame;
static lv_style_t dark_frame;

/*Saved input parameters*/
static lv_font_t * _font;

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void basic_init(void)
{
    lv_style_copy(&def, &lv_style_plain); /*Initialize the default style*/
    def.body.main_color     = LV_COLOR_WHITE;
    def.body.grad_color     = LV_COLOR_WHITE;
    def.body.radius         = 0;
    def.body.opa            = LV_OPA_COVER;
    def.body.padding.left   = LV_DPI / 10;
    def.body.padding.right  = LV_DPI / 10;
    def.body.padding.top    = LV_DPI / 10;
    def.body.padding.bottom = LV_DPI / 10;
    def.body.padding.inner  = LV_DPI / 10;
    def.body.border.color   = LV_COLOR_BLACK;
    def.body.border.width   = 1;
    def.body.border.opa     = LV_OPA_COVER;
    def.body.border.part    = LV_BORDER_FULL;

    def.text.font         = _font;
    def.text.color        = LV_COLOR_BLACK;
    def.text.letter_space = 1;
    def.text.line_space   = 1;

    def.line.color = LV_COLOR_BLACK;
    def.line.opa   = LV_OPA_COVER;
    def.line.width = 1;

    def.image.color   = LV_COLOR_BLACK;
    def.image.intense = LV_OPA_TRANSP;
    def.image.opa     = LV_OPA_COVER;

    lv_style_copy(&scr, &light_plain);
    scr.body.padding.bottom = 0;
    scr.body.padding.top    = 0;
    scr.body.padding.left   = 0;
    scr.body.padding.right  = 0;

    lv_style_copy(&light_plain, &def);

    lv_style_copy(&light_frame, &light_plain);
    light_frame.body.radius = LV_DPI / 20;

    lv_style_copy(&dark_plain, &light_plain);
    dark_plain.body.main_color   = LV_COLOR_BLACK;
    dark_plain.body.grad_color   = LV_COLOR_BLACK;
    dark_plain.body.border.color = LV_COLOR_WHITE;
    dark_plain.text.color        = LV_COLOR_WHITE;
    dark_plain.line.color        = LV_COLOR_WHITE;
    dark_plain.image.color       = LV_COLOR_WHITE;

    lv_style_copy(&dark_frame, &dark_plain);
    dark_frame.body.radius = LV_DPI / 20;

    theme.style.bg    = &def;
    theme.style.scr   = &scr;
    theme.style.panel = &light_frame;
}

static void cont_init(void)
{
#if LV_USE_CONT != 0

    theme.style.cont = &def;
#endif
}

static void btn_init(void)
{
#if LV_USE_BTN != 0

    theme.style.btn.rel     = &light_frame;
    theme.style.btn.pr      = &dark_frame;
    theme.style.btn.tgl_rel = &dark_frame;
    theme.style.btn.tgl_pr  = &light_frame;
    theme.style.btn.ina     = &light_frame;
#endif
}

static void label_init(void)
{
#if LV_USE_LABEL != 0

    theme.style.label.prim = NULL;
    theme.style.label.sec  = NULL;
    theme.style.label.hint = NULL;
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
    theme.style.line.decor = NULL;
#endif
}

static void led_init(void)
{
#if LV_USE_LED != 0
    static lv_style_t led;
    lv_style_copy(&led, &light_frame);
    led.body.radius       = LV_RADIUS_CIRCLE;
    led.body.shadow.width = LV_DPI / 8;
    led.body.shadow.color = LV_COLOR_BLACK;
    led.body.shadow.type  = LV_SHADOW_FULL;

    theme.style.led = &led;
#endif
}

static void bar_init(void)
{
#if LV_USE_BAR
    static lv_style_t bar_bg;
    static lv_style_t bar_indic;

    lv_style_copy(&bar_bg, &light_frame);
    bar_bg.body.padding.left   = LV_DPI / 15;
    bar_bg.body.padding.right  = LV_DPI / 15;
    bar_bg.body.padding.top    = LV_DPI / 15;
    bar_bg.body.padding.bottom = LV_DPI / 15;
    bar_bg.body.radius         = LV_RADIUS_CIRCLE;

    lv_style_copy(&bar_indic, &dark_frame);
    bar_indic.body.padding.left   = LV_DPI / 30;
    bar_indic.body.padding.right  = LV_DPI / 30;
    bar_indic.body.padding.top    = LV_DPI / 30;
    bar_indic.body.padding.bottom = LV_DPI / 30;
    bar_indic.body.radius         = LV_RADIUS_CIRCLE;

    theme.style.bar.bg    = &bar_bg;
    theme.style.bar.indic = &bar_indic;
#endif
}

static void slider_init(void)
{
#if LV_USE_SLIDER != 0
    static lv_style_t slider_knob;
    lv_style_copy(&slider_knob, &light_frame);
    slider_knob.body.radius         = LV_RADIUS_CIRCLE;
    slider_knob.body.padding.left   = LV_DPI / 30;
    slider_knob.body.padding.right  = LV_DPI / 30;
    slider_knob.body.padding.top    = LV_DPI / 30;
    slider_knob.body.padding.bottom = LV_DPI / 30;

    theme.style.slider.bg    = theme.style.bar.bg;
    theme.style.slider.indic = theme.style.bar.indic;
    theme.style.slider.knob  = &slider_knob;
#endif
}

static void sw_init(void)
{
#if LV_USE_SW != 0

    theme.style.sw.bg       = theme.style.slider.bg;
    theme.style.sw.indic    = theme.style.slider.indic;
    theme.style.sw.knob_off = theme.style.slider.knob;
    theme.style.sw.knob_on  = theme.style.slider.knob;
#endif
}

static void lmeter_init(void)
{
#if LV_USE_LMETER != 0
    static lv_style_t lmeter_bg;
    lv_style_copy(&lmeter_bg, &light_frame);
    lmeter_bg.body.opa           = LV_OPA_TRANSP;
    lmeter_bg.body.main_color    = LV_COLOR_BLACK;
    lmeter_bg.body.grad_color    = LV_COLOR_BLACK;
    lmeter_bg.body.padding.left  = LV_DPI / 20;
    lmeter_bg.body.padding.inner = LV_DPI / 8;
    lmeter_bg.line.color         = LV_COLOR_WHITE;
    lmeter_bg.line.width         = 1;

    theme.style.lmeter = &lmeter_bg;
#endif
}

static void gauge_init(void)
{
#if LV_USE_GAUGE != 0
    static lv_style_t gauge_bg;
    lv_style_copy(&gauge_bg, theme.style.lmeter);
    gauge_bg.line.color = LV_COLOR_BLACK;
    gauge_bg.line.width = 1;

    theme.style.gauge = &gauge_bg;
#endif
}

static void chart_init(void)
{
#if LV_USE_CHART
    theme.style.chart = &light_frame;
#endif
}

static void calendar_init(void)
{
#if LV_USE_CALENDAR
    static lv_style_t box;
    lv_style_copy(&box, &light_plain);
    box.body.padding.top    = LV_DPI / 20;
    box.body.padding.bottom = LV_DPI / 20;

    /*Can't handle highlighted dates in this theme*/
    theme.style.calendar.week_box  = &box;
    theme.style.calendar.today_box = &box;
#endif
}

static void cb_init(void)
{
#if LV_USE_CB != 0

    theme.style.cb.bg          = &lv_style_transp;
    theme.style.cb.box.rel     = &light_frame;
    theme.style.cb.box.pr      = &dark_frame;
    theme.style.cb.box.tgl_rel = &dark_frame;
    theme.style.cb.box.tgl_pr  = &light_frame;
    theme.style.cb.box.ina     = &light_frame;
#endif
}

static void btnm_init(void)
{
#if LV_USE_BTNM

    theme.style.btnm.bg          = &light_frame;
    theme.style.btnm.btn.rel     = &light_frame;
    theme.style.btnm.btn.pr      = &dark_frame;
    theme.style.btnm.btn.tgl_rel = &dark_frame;
    theme.style.btnm.btn.tgl_pr  = &light_frame;
    theme.style.btnm.btn.ina     = &light_frame;
#endif
}

static void kb_init(void)
{
#if LV_USE_KB
    theme.style.kb.bg          = &lv_style_transp_fit;
    theme.style.kb.btn.rel     = &light_frame;
    theme.style.kb.btn.pr      = &light_frame;
    theme.style.kb.btn.tgl_rel = &dark_frame;
    theme.style.kb.btn.tgl_pr  = &dark_frame;
    theme.style.kb.btn.ina     = &light_frame;
#endif
}

static void mbox_init(void)
{
#if LV_USE_MBOX

    theme.style.mbox.bg      = &dark_frame;
    theme.style.mbox.btn.bg  = &lv_style_transp_fit;
    theme.style.mbox.btn.rel = &light_frame;
    theme.style.mbox.btn.pr  = &dark_frame;
#endif
}

static void page_init(void)
{
#if LV_USE_PAGE

    theme.style.page.bg   = &light_frame;
    theme.style.page.scrl = &light_frame;
    theme.style.page.sb   = &dark_frame;
#endif
}

static void ta_init(void)
{
#if LV_USE_TA

    theme.style.ta.area    = &light_frame;
    theme.style.ta.oneline = &light_frame;
    theme.style.ta.cursor  = NULL; /*Let library to calculate the cursor's style*/
    theme.style.ta.sb      = &dark_frame;
#endif
}

static void spinbox_init(void)
{
#if LV_USE_SPINBOX

    theme.style.spinbox.bg    = &light_frame;
    theme.style.spinbox.cursor  = NULL; /*Let library to calculate the cursor's style*/
#endif
}

static void list_init(void)
{
#if LV_USE_LIST != 0

    theme.style.list.sb          = &dark_frame;
    theme.style.list.bg          = &light_frame;
    theme.style.list.scrl        = &lv_style_transp_fit;
    theme.style.list.btn.rel     = &light_plain;
    theme.style.list.btn.pr      = &dark_plain;
    theme.style.list.btn.tgl_rel = &dark_plain;
    theme.style.list.btn.tgl_pr  = &light_plain;
    theme.style.list.btn.ina     = &light_plain;
#endif
}

static void ddlist_init(void)
{
#if LV_USE_DDLIST != 0
    static lv_style_t bg;
    lv_style_copy(&bg, &light_frame);
    bg.text.line_space = LV_DPI / 12;

    theme.style.ddlist.bg  = &bg;
    theme.style.ddlist.sel = &dark_plain;
    theme.style.ddlist.sb  = &dark_frame;
#endif
}

static void roller_init(void)
{
#if LV_USE_ROLLER != 0
    static lv_style_t bg;
    lv_style_copy(&bg, &light_frame);
    bg.text.line_space = LV_DPI / 12;

    theme.style.roller.bg  = &bg;
    theme.style.roller.sel = &dark_frame;
#endif
}

static void tabview_init(void)
{
#if LV_USE_TABVIEW != 0

    theme.style.tabview.bg          = &light_frame;
    theme.style.tabview.indic       = &light_plain;
    theme.style.tabview.btn.bg      = &lv_style_transp_fit;
    theme.style.tabview.btn.rel     = &light_frame;
    theme.style.tabview.btn.pr      = &dark_frame;
    theme.style.tabview.btn.tgl_rel = &dark_frame;
    theme.style.tabview.btn.tgl_pr  = &light_frame;
#endif
}

static void win_init(void)
{
#if LV_USE_WIN != 0
    static lv_style_t win_header;
    lv_style_copy(&win_header, &dark_plain);
    win_header.body.padding.left   = LV_DPI / 30;
    win_header.body.padding.right  = LV_DPI / 30;
    win_header.body.padding.top    = LV_DPI / 30;
    win_header.body.padding.bottom = LV_DPI / 30;

    theme.style.win.bg      = &light_frame;
    theme.style.win.sb      = &dark_frame;
    theme.style.win.header  = &win_header;
    theme.style.win.content = &lv_style_transp;
    theme.style.win.btn.rel = &light_frame;
    theme.style.win.btn.pr  = &dark_frame;
#endif
}

#if LV_USE_GROUP

static void style_mod(lv_group_t * group, lv_style_t * style)
{
    (void)group; /*Unused*/
#if LV_COLOR_DEPTH != 1
    /*Make the style to be a little bit orange*/
    style->body.border.opa   = LV_OPA_COVER;
    style->body.border.color = LV_COLOR_BLACK;

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
    style->body.border.color = LV_COLOR_BLACK;

    /*If not empty or has border then emphasis the border*/
    if(style->body.opa != LV_OPA_TRANSP || style->body.border.width != 0) style->body.border.width = LV_DPI / 20;
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
 * Initialize the mono theme
 * @param hue [0..360] hue value from HSV color space to define the theme's base color; is not used
 * in lv_theme_mono
 * @param font pointer to a font (NULL to use the default)
 * @return pointer to the initialized theme
 */
lv_theme_t * lv_theme_mono_init(uint16_t hue, lv_font_t * font)
{

    (void)hue; /*Unused*/

    if(font == NULL) font = LV_FONT_DEFAULT;

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
lv_theme_t * lv_theme_get_mono(void)
{
    return &theme;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif
