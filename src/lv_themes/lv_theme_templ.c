/**
 * @file lv_theme_templ.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_theme.h"

#if LV_USE_THEME_TEMPL

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
    def.text.font = _font;

    theme.style.scr   = &def;
    theme.style.bg    = &def;
    theme.style.panel = &def;
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

    theme.style.btn.rel     = &def;
    theme.style.btn.pr      = &def;
    theme.style.btn.tgl_rel = &def;
    theme.style.btn.tgl_pr  = &def;
    theme.style.btn.ina     = &def;
#endif
}

static void imgbtn_init(void)
{
#if LV_USE_IMGBTN != 0
    theme.style.imgbtn.rel     = &def;
    theme.style.imgbtn.pr      = &def;
    theme.style.imgbtn.tgl_rel = &def;
    theme.style.imgbtn.tgl_pr  = &def;
    theme.style.imgbtn.ina     = &def;
#endif
}

static void label_init(void)
{
#if LV_USE_LABEL != 0

    theme.style.label.prim = &def;
    theme.style.label.sec  = &def;
    theme.style.label.hint = &def;
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

    theme.style.led = &def;
#endif
}

static void bar_init(void)
{
#if LV_USE_BAR

    theme.style.bar.bg    = &def;
    theme.style.bar.indic = &def;
#endif
}

static void slider_init(void)
{
#if LV_USE_SLIDER != 0

    theme.style.slider.bg    = &def;
    theme.style.slider.indic = &def;
    theme.style.slider.knob  = &def;
#endif
}

static void sw_init(void)
{
#if LV_USE_SW != 0

    theme.style.sw.bg       = &def;
    theme.style.sw.indic    = &def;
    theme.style.sw.knob_off = &def;
    theme.style.sw.knob_on  = &def;
#endif
}

static void lmeter_init(void)
{
#if LV_USE_LMETER != 0

    theme.style.lmeter = &def;
#endif
}

static void gauge_init(void)
{
#if LV_USE_GAUGE != 0

    theme.style.gauge = &def;
#endif
}

static void arc_init(void)
{
#if LV_USE_ARC != 0

    theme.style.arc = &def;
#endif
}

static void preload_init(void)
{
#if LV_USE_PRELOAD != 0

    theme.style.preload = &def;
#endif
}

static void chart_init(void)
{
#if LV_USE_CHART

    theme.style.chart = &def;
#endif
}

static void calendar_init(void)
{
#if LV_USE_CALENDAR

    theme.style.calendar.bg               = theme.style.panel;
    theme.style.calendar.header           = &def;
    theme.style.calendar.inactive_days    = &def;
    theme.style.calendar.highlighted_days = &def;
    theme.style.calendar.week_box         = &def;
    theme.style.calendar.today_box        = &def;
    theme.style.calendar.header_pr        = &def;
    theme.style.calendar.day_names        = &def;
#endif
}

static void cb_init(void)
{
#if LV_USE_CB != 0

    theme.style.cb.bg          = &def;
    theme.style.cb.box.rel     = &def;
    theme.style.cb.box.pr      = &def;
    theme.style.cb.box.tgl_rel = &def;
    theme.style.cb.box.tgl_pr  = &def;
    theme.style.cb.box.ina     = &def;
#endif
}

static void btnm_init(void)
{
#if LV_USE_BTNM

    theme.style.btnm.bg          = &def;
    theme.style.btnm.btn.rel     = &def;
    theme.style.btnm.btn.pr      = &def;
    theme.style.btnm.btn.tgl_rel = &def;
    theme.style.btnm.btn.tgl_pr  = &def;
    theme.style.btnm.btn.ina     = &def;
#endif
}

static void kb_init(void)
{
#if LV_USE_KB

    theme.style.kb.bg          = &def;
    theme.style.kb.btn.rel     = &def;
    theme.style.kb.btn.pr      = &def;
    theme.style.kb.btn.tgl_rel = &def;
    theme.style.kb.btn.tgl_pr  = &def;
    theme.style.kb.btn.ina     = &def;
#endif
}

static void mbox_init(void)
{
#if LV_USE_MBOX

    theme.style.mbox.bg      = &def;
    theme.style.mbox.btn.bg  = &def;
    theme.style.mbox.btn.rel = &def;
    theme.style.mbox.btn.pr  = &def;
#endif
}

static void page_init(void)
{
#if LV_USE_PAGE

    theme.style.page.bg   = &def;
    theme.style.page.scrl = &def;
    theme.style.page.sb   = &def;
#endif
}

static void ta_init(void)
{
#if LV_USE_TA

    theme.style.ta.area    = &def;
    theme.style.ta.oneline = &def;
    theme.style.ta.cursor  = NULL; /*Let library to calculate the cursor's style*/
    theme.style.ta.sb      = &def;
#endif
}

static void list_init(void)
{
#if LV_USE_LIST != 0

    theme.style.list.sb          = &def;
    theme.style.list.bg          = &def;
    theme.style.list.scrl        = &def;
    theme.style.list.btn.rel     = &def;
    theme.style.list.btn.pr      = &def;
    theme.style.list.btn.tgl_rel = &def;
    theme.style.list.btn.tgl_pr  = &def;
    theme.style.list.btn.ina     = &def;
#endif
}

static void ddlist_init(void)
{
#if LV_USE_DDLIST != 0

    theme.style.ddlist.bg  = &def;
    theme.style.ddlist.sel = &def;
    theme.style.ddlist.sb  = &def;
#endif
}

static void roller_init(void)
{
#if LV_USE_ROLLER != 0

    theme.style.roller.bg  = &def;
    theme.style.roller.sel = &def;
#endif
}

static void tabview_init(void)
{
#if LV_USE_TABVIEW != 0

    theme.style.tabview.bg          = &def;
    theme.style.tabview.indic       = &def;
    theme.style.tabview.btn.bg      = &def;
    theme.style.tabview.btn.rel     = &def;
    theme.style.tabview.btn.pr      = &def;
    theme.style.tabview.btn.tgl_rel = &def;
    theme.style.tabview.btn.tgl_pr  = &def;
#endif
}

static void table_init(void)
{
#if LV_USE_TABLE != 0
    theme.style.table.bg   = &def;
    theme.style.table.cell = &def;
#endif
}

static void win_init(void)
{
#if LV_USE_WIN != 0

    theme.style.win.bg      = &def;
    theme.style.win.sb      = &def;
    theme.style.win.header  = &def;
    theme.style.win.content = &def;
    theme.style.win.btn.rel = &def;
    theme.style.win.btn.pr  = &def;
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
    if(style->body.border.width != 0) style->body.border.width = LV_DPI / 20;

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
    if(style->body.border.width != 0) style->body.border.width = LV_DPI / 20;

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
 * Initialize the templ theme
 * @param hue [0..360] hue value from HSV color space to define the theme's base color
 * @param font pointer to a font (NULL to use the default)
 * @return pointer to the initialized theme
 */
lv_theme_t * lv_theme_templ_init(uint16_t hue, lv_font_t * font)
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
    imgbtn_init();
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
lv_theme_t * lv_theme_get_templ(void)
{
    return &theme;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif
