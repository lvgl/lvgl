/**
 * @file lv_theme_templ.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_theme.h"


#if USE_LV_THEME_TEMPL

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
    lv_style_copy(&def, &lv_style_pretty);  /*Initialize the default style*/
    def.text.font = _font;

    theme.bg = &def;
    theme.panel = &def;

}

static void cont_init(void)
{
#if USE_LV_CONT != 0


    theme.cont = &def;
#endif
}

static void btn_init(void)
{
#if USE_LV_BTN != 0


    theme.btn.rel = &def;
    theme.btn.pr = &def;
    theme.btn.tgl_rel = &def;
    theme.btn.tgl_pr =  &def;
    theme.btn.ina =  &def;
#endif
}


static void label_init(void)
{
#if USE_LV_LABEL != 0


    theme.label.prim = &def;
    theme.label.sec = &def;
    theme.label.hint = &def;
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


    theme.led = &def;
#endif
}

static void bar_init(void)
{
#if USE_LV_BAR


    theme.bar.bg = &def;
    theme.bar.indic = &def;
#endif
}

static void slider_init(void)
{
#if USE_LV_SLIDER != 0


    theme.slider.bg = &def;
    theme.slider.indic = &def;
    theme.slider.knob = &def;
#endif
}

static void sw_init(void)
{
#if USE_LV_SW != 0


    theme.sw.bg = &def;
    theme.sw.indic = &def;
    theme.sw.knob_off = &def;
    theme.sw.knob_on = &def;
#endif
}


static void lmeter_init(void)
{
#if USE_LV_LMETER != 0


    theme.lmeter = &def;
#endif
}

static void gauge_init(void)
{
#if USE_LV_GAUGE != 0


    theme.gauge = &def;
#endif
}

static void chart_init(void)
{
#if USE_LV_CHART


    theme.chart = &def;
#endif
}

static void cb_init(void)
{
#if USE_LV_CB != 0


    theme.cb.bg = &def;
    theme.cb.box.rel = &def;
    theme.cb.box.pr = &def;
    theme.cb.box.tgl_rel = &def;
    theme.cb.box.tgl_pr = &def;
    theme.cb.box.ina = &def;
#endif
}


static void btnm_init(void)
{
#if USE_LV_BTNM


    theme.btnm.bg = &def;
    theme.btnm.btn.rel = &def;
    theme.btnm.btn.pr = &def;
    theme.btnm.btn.tgl_rel = &def;
    theme.btnm.btn.tgl_pr = &def;
    theme.btnm.btn.ina = &def;
#endif
}

static void kb_init(void)
{
#if USE_LV_KB


    theme.kb.bg = &def;
    theme.kb.btn.rel = &def;
    theme.kb.btn.pr = &def;
    theme.kb.btn.tgl_rel = &def;
    theme.kb.btn.tgl_pr = &def;
    theme.kb.btn.ina = &def;
#endif

}

static void mbox_init(void)
{
#if USE_LV_MBOX


    theme.mbox.bg = &def;
    theme.mbox.btn.bg = &def;
    theme.mbox.btn.rel = &def;
    theme.mbox.btn.pr = &def;
#endif
}

static void page_init(void)
{
#if USE_LV_PAGE


    theme.page.bg = &def;
    theme.page.scrl = &def;
    theme.page.sb = &def;
#endif
}

static void ta_init(void)
{
#if USE_LV_TA


    theme.ta.area = &def;
    theme.ta.oneline = &def;
    theme.ta.cursor = NULL;     /*Let library to calculate the cursor's style*/
    theme.ta.sb = &def;
#endif
}

static void list_init(void)
{
#if USE_LV_LIST != 0


    theme.list.sb = &def;
    theme.list.bg = &def;
    theme.list.scrl = &def;
    theme.list.btn.rel = &def;
    theme.list.btn.pr = &def;
    theme.list.btn.tgl_rel = &def;
    theme.list.btn.tgl_pr = &def;
    theme.list.btn.ina = &def;
#endif
}

static void ddlist_init(void)
{
#if USE_LV_DDLIST != 0


    theme.ddlist.bg = &def;
    theme.ddlist.sel = &def;
    theme.ddlist.sb = &def;
#endif
}

static void roller_init(void)
{
#if USE_LV_ROLLER != 0


    theme.roller.bg = &def;
    theme.roller.sel = &def;
#endif
}

static void tabview_init(void)
{
#if USE_LV_TABVIEW != 0


    theme.tabview.bg = &def;
    theme.tabview.indic = &def;
    theme.tabview.btn.bg = &def;
    theme.tabview.btn.rel = &def;
    theme.tabview.btn.pr = &def;
    theme.tabview.btn.tgl_rel = &def;
    theme.tabview.btn.tgl_pr = &def;
#endif
}


static void win_init(void)
{
#if USE_LV_WIN != 0


    theme.win.bg = &def;
    theme.win.sb = &def;
    theme.win.header = &def;
    theme.win.content.bg = &def;
    theme.win.content.scrl = &def;
    theme.win.btn.rel = &def;
    theme.win.btn.pr = &def;
#endif
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/



/**
 * Initialize the templ theme
 * @param hue [0..360] hue value from HSV color space to define the theme's base color
 * @param font pointer to a font (NULL to use the default)
 * @return pointer to the initialized theme
 */
lv_theme_t * lv_theme_templ_init(uint16_t hue, lv_font_t *font)
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
lv_theme_t * lv_theme_get_templ(void)
{
    return &theme;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif

