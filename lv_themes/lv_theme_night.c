/**
 * @file lv_theme_night.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_theme.h"

#if USE_LV_THEME_NIGHT

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
static lv_style_t bg, sb, panel;
static lv_style_t prim, sec, hint;
static lv_style_t btn_rel, btn_pr, btn_tgl_rel, btn_tgl_pr, btn_ina;
static lv_style_t bar_bg, bar_indic;
static lv_style_t slider_knob;
static lv_style_t ddlist_bg, ddlist_sel;
static lv_style_t lmeter_bg;

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

    lv_style_copy(&bg, &lv_style_plain);
    bg.body.main_color = lv_color_hsv_to_rgb(_hue, 11, 30);
    bg.body.grad_color = lv_color_hsv_to_rgb(_hue, 11, 30);
    bg.text.color = lv_color_hsv_to_rgb(_hue, 5, 95);
    bg.text.font = _font;

    lv_style_copy(&sb, &def);
    sb.body.main_color = lv_color_hsv_to_rgb(_hue, 30, 60);
    sb.body.grad_color = lv_color_hsv_to_rgb(_hue, 30, 60);
    sb.body.border.width = 0;
    sb.body.padding.inner = LV_DPI / 10;
    sb.body.padding.ver = 0;
    sb.body.padding.hor = 0;
    sb.body.radius = LV_DPI / 30;
    sb.body.opa = LV_OPA_COVER;

    lv_style_copy(&panel, &bg);
    panel.body.main_color = lv_color_hsv_to_rgb(_hue, 11, 18);
    panel.body.grad_color = lv_color_hsv_to_rgb(_hue, 11, 18);
    panel.body.radius = LV_DPI / 20;
    panel.body.border.color = lv_color_hsv_to_rgb(_hue, 10, 25);
    panel.body.border.width = 1;
    panel.body.border.opa = LV_OPA_COVER;
    panel.body.padding.ver = LV_DPI / 10;
    panel.body.padding.hor = LV_DPI / 10;
    panel.line.color = lv_color_hsv_to_rgb(_hue, 20, 40);
    panel.line.width = 1;
    theme.bg = &bg;
    theme.panel = &def;
}

static void cont_init(void)
{
#if USE_LV_CONT != 0


    theme.cont = &panel;
#endif
}
static void btn_init(void)
{
#if USE_LV_BTN != 0

    lv_style_copy(&btn_rel, &def);
    btn_rel.body.main_color = lv_color_hsv_to_rgb(_hue, 10, 40);
    btn_rel.body.grad_color = lv_color_hsv_to_rgb(_hue, 10, 20);
    btn_rel.body.border.color = LV_COLOR_HEX3(0x111);
    btn_rel.body.border.width = 1;
    btn_rel.body.border.opa = LV_OPA_70;
    btn_rel.body.padding.hor = LV_DPI / 4;
    btn_rel.body.padding.ver = LV_DPI / 8;
    btn_rel.body.shadow.type = LV_SHADOW_BOTTOM;
    btn_rel.body.shadow.color = LV_COLOR_HEX3(0x111);
    btn_rel.body.shadow.width = LV_DPI / 30;
    btn_rel.text.color = LV_COLOR_HEX3(0xeee);

    lv_style_copy(&btn_pr, &btn_rel);
    btn_pr.body.main_color = lv_color_hsv_to_rgb(_hue, 10, 30);
    btn_pr.body.grad_color = lv_color_hsv_to_rgb(_hue, 10, 10);

    lv_style_copy(&btn_tgl_rel, &btn_rel);
    btn_tgl_rel.body.main_color = lv_color_hsv_to_rgb(_hue, 10, 20);
    btn_tgl_rel.body.grad_color = lv_color_hsv_to_rgb(_hue, 10, 40);
    btn_tgl_rel.body.shadow.width = LV_DPI / 40;
    btn_tgl_rel.text.color = LV_COLOR_HEX3(0xddd);

    lv_style_copy(&btn_tgl_pr, &btn_rel);
    btn_tgl_pr.body.main_color = lv_color_hsv_to_rgb(_hue, 10, 10);
    btn_tgl_pr.body.grad_color = lv_color_hsv_to_rgb(_hue, 10, 30);
    btn_tgl_pr.body.shadow.width = LV_DPI / 30;
    btn_tgl_pr.text.color = LV_COLOR_HEX3(0xddd);

    lv_style_copy(&btn_ina, &btn_rel);
    btn_ina.body.main_color = lv_color_hsv_to_rgb(_hue, 10, 20);
    btn_ina.body.grad_color = lv_color_hsv_to_rgb(_hue, 10, 20);
    btn_ina.text.color = LV_COLOR_HEX3(0xaaa);
    btn_ina.body.shadow.width = 0;

    theme.btn.rel = &btn_rel;
    theme.btn.pr = &btn_pr;
    theme.btn.tgl_rel = &btn_tgl_rel;
    theme.btn.tgl_pr =  &btn_tgl_pr;
    theme.btn.ina =  &btn_ina;
#endif
}


static void label_init(void)
{
#if USE_LV_LABEL != 0

    lv_style_copy(&prim, &bg);
    prim.text.color = lv_color_hsv_to_rgb(_hue, 5, 95);

    lv_style_copy(&sec, &bg);
    sec.text.color = lv_color_hsv_to_rgb(_hue, 15, 65);

    lv_style_copy(&hint, &bg);
    hint.text.color = lv_color_hsv_to_rgb(_hue, 20, 55);

    theme.label.prim = &prim;
    theme.label.sec = &sec;
    theme.label.hint = &hint;
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
    lv_style_copy(&led, &def);
    led.body.shadow.width = LV_DPI / 10;
    led.body.radius = LV_RADIUS_CIRCLE;
    led.body.border.width = LV_DPI / 30;
    led.body.border.opa = LV_OPA_30;
    led.body.main_color = lv_color_hsv_to_rgb(_hue, 100, 100);
    led.body.grad_color = lv_color_hsv_to_rgb(_hue, 100, 40);
    led.body.border.color = lv_color_hsv_to_rgb(_hue, 60, 60);
    led.body.shadow.color = lv_color_hsv_to_rgb(_hue, 100, 100);

    theme.led = &led;
#endif
}

static void img_init(void)
{
#if USE_LV_IMG != 0


    theme.img.light = &def;
    theme.img.dark = &def;
#endif
}

static void bar_init(void)
{
#if USE_LV_BAR
    lv_style_copy(&bar_bg, &panel);
    bar_bg.body.padding.ver = LV_DPI / 16;
    bar_bg.body.padding.hor = LV_DPI / 16;
    bar_bg.body.radius = LV_RADIUS_CIRCLE;

    lv_style_copy(&bar_indic, &def);
    bar_indic.body.main_color = lv_color_hsv_to_rgb(_hue, 80, 70);
    bar_indic.body.grad_color = lv_color_hsv_to_rgb(_hue, 80, 70);
    bar_indic.body.border.color = lv_color_hsv_to_rgb(_hue, 20, 15);
    bar_indic.body.border.width = 1;
    bar_indic.body.border.opa = LV_OPA_COVER;
    bar_indic.body.radius = LV_RADIUS_CIRCLE;
    bar_indic.body.padding.hor = 0;
    bar_indic.body.padding.ver = 0;

    theme.bar.bg = &bar_bg;
    theme.bar.indic = &bar_indic;
#endif
}

static void slider_init(void)
{
#if USE_LV_SLIDER != 0

    lv_style_copy(&slider_knob, &btn_rel);
    slider_knob.body.radius = LV_RADIUS_CIRCLE;

    theme.slider.bg = &bar_bg;
    theme.slider.indic = &bar_indic;
    theme.slider.knob = &slider_knob;
#endif
}

static void sw_init(void)
{
#if USE_LV_SW != 0


    theme.sw.bg = &bar_bg;
    theme.sw.indic = &bar_indic;
    theme.sw.knob_off = &slider_knob;
    theme.sw.knob_on = &slider_knob;
#endif
}


static void lmeter_init(void)
{
#if USE_LV_LMETER != 0
    lv_style_copy(&lmeter_bg, &def);
    lmeter_bg.body.main_color = lv_color_hsv_to_rgb(_hue, 10, 70);
    lmeter_bg.body.grad_color = lv_color_hsv_to_rgb(_hue, 95, 90);
    lmeter_bg.body.padding.hor = LV_DPI / 10;           /*Scale line length*/
    lmeter_bg.body.padding.inner = LV_DPI / 10;         /*Text padding*/
    lmeter_bg.body.border.color = LV_COLOR_HEX3(0x333);
    lmeter_bg.line.color = LV_COLOR_HEX3(0x555);
    lmeter_bg.line.width = 1;
    lmeter_bg.text.color = LV_COLOR_HEX3(0xddd);

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
    gauge_bg.line.color = lv_color_hsv_to_rgb(_hue, 80, 75);
    gauge_bg.line.width = 1;
    gauge_bg.text.color = LV_COLOR_HEX3(0xddd);

    theme.gauge = &gauge_bg;
#endif
}

static void arc_init(void)
{
#if USE_LV_ARC != 0

    static lv_style_t arc;
    lv_style_copy(&arc, &def);
    arc.line.width = 8;
    arc.line.color = lv_color_hsv_to_rgb(_hue, 80, 70);
    arc.line.rounded = 1;

    /*For preloader*/
    arc.body.border.width = 7;
    arc.body.border.color = lv_color_hsv_to_rgb(_hue, 11, 48);
    arc.body.padding.hor = 1;
    arc.body.padding.ver = 1;

    theme.arc = &arc;
#endif
}

static void preload_init(void)
{
#if USE_LV_PRELOAD != 0

    theme.preload = theme.arc;
#endif
}

static void chart_init(void)
{
#if USE_LV_CHART
    theme.chart = &panel;
#endif
}

static void calendar_init(void)
{
#if USE_LV_CALENDAR
    static lv_style_t cal_bg;
    lv_style_copy(&cal_bg, &bg);
    cal_bg.body.main_color = lv_color_hsv_to_rgb(_hue, 10, 40);
    cal_bg.body.grad_color = lv_color_hsv_to_rgb(_hue, 10, 40);
    cal_bg.body.border.color = LV_COLOR_HEX3(0x333);
    cal_bg.body.border.width = 1;
    cal_bg.body.radius = LV_DPI / 20;
    cal_bg.body.padding.hor = LV_DPI / 10;
    cal_bg.body.padding.ver = LV_DPI / 10;

    static lv_style_t cal_header;
    lv_style_copy(&cal_header, &bg);
    cal_header.body.main_color = lv_color_hsv_to_rgb(_hue, 10, 20);
    cal_header.body.grad_color = lv_color_hsv_to_rgb(_hue, 10, 20);
    cal_header.body.radius = 0;
    cal_header.body.border.width = 1;
    cal_header.body.border.color = LV_COLOR_HEX3(0x333);
    cal_header.body.padding.hor = LV_DPI / 10;
    cal_header.body.padding.ver = LV_DPI / 10;

    static lv_style_t week_box;
    lv_style_copy(&week_box, &panel);
    week_box.body.main_color = lv_color_hsv_to_rgb(_hue, 30, 45);
    week_box.body.grad_color = lv_color_hsv_to_rgb(_hue, 30, 45);
    week_box.body.radius = LV_DPI / 20;
    week_box.body.border.width = 1;
    week_box.body.padding.hor = LV_DPI / 20;
    week_box.body.padding.ver = LV_DPI / 25;

    static lv_style_t today_box;
    lv_style_copy(&today_box, &week_box);
    today_box.body.main_color = lv_color_hsv_to_rgb(_hue, 80, 70);
    today_box.body.grad_color = lv_color_hsv_to_rgb(_hue, 80, 70);
    today_box.body.radius = LV_DPI / 20;
    today_box.body.padding.hor = LV_DPI / 14;
    today_box.body.padding.ver = LV_DPI / 14;

    static lv_style_t highlighted_days;
    lv_style_copy(&highlighted_days, &bg);
    highlighted_days.text.color = lv_color_hsv_to_rgb(_hue, 40, 80);

    static lv_style_t ina_days;
    lv_style_copy(&ina_days, &bg);
    ina_days.text.color = lv_color_hsv_to_rgb(_hue, 0, 60);

    theme.calendar.bg = &cal_bg;
    theme.calendar.header = &cal_header;
    theme.calendar.week_box = &week_box;
    theme.calendar.today_box = &today_box;
    theme.calendar.highlighted_days = &highlighted_days;
    theme.calendar.day_names = &cal_bg;
    theme.calendar.inactive_days = &ina_days;
#endif
}

static void cb_init(void)
{
#if USE_LV_CB != 0

    static lv_style_t rel, pr, tgl_rel, tgl_pr, ina;

    lv_style_copy(&rel, &def);
    rel.body.radius = LV_DPI / 20;
    rel.body.main_color = lv_color_hsv_to_rgb(_hue, 10, 95);
    rel.body.grad_color = lv_color_hsv_to_rgb(_hue, 10, 95);
    rel.body.border.color = lv_color_hsv_to_rgb(_hue, 10, 50);
    rel.body.border.width = 2;;

    lv_style_copy(&pr, &rel);
    pr.body.main_color = lv_color_hsv_to_rgb(_hue, 10, 80);
    pr.body.grad_color = lv_color_hsv_to_rgb(_hue, 10, 80);
    pr.body.border.color = lv_color_hsv_to_rgb(_hue, 10, 20);
    pr.body.border.width = 1;;

    lv_style_copy(&tgl_rel, &rel);
    tgl_rel.body.main_color = lv_color_hsv_to_rgb(_hue, 80, 90);
    tgl_rel.body.grad_color = lv_color_hsv_to_rgb(_hue, 80, 90);
    tgl_rel.body.border.color = lv_color_hsv_to_rgb(_hue, 80, 50);

    lv_style_copy(&tgl_pr, &tgl_rel);
    tgl_pr.body.main_color = lv_color_hsv_to_rgb(_hue, 80, 70);
    tgl_pr.body.grad_color = lv_color_hsv_to_rgb(_hue, 80, 70);
    tgl_pr.body.border.color = lv_color_hsv_to_rgb(_hue, 80, 30);
    tgl_pr.body.border.width = 1;;


    lv_style_copy(&ina, &rel);
    ina.body.main_color = LV_COLOR_HEX3(0x777);
    ina.body.grad_color = LV_COLOR_HEX3(0x777);
    ina.body.border.width = 0;

    theme.cb.bg = &lv_style_transp;
    theme.cb.box.rel = &rel;
    theme.cb.box.pr = &pr;
    theme.cb.box.tgl_rel = &tgl_rel;
    theme.cb.box.tgl_pr = &tgl_pr;
    theme.cb.box.ina = &def;
#endif
}


static void btnm_init(void)
{
#if USE_LV_BTNM
    static lv_style_t btnm_bg, rel, pr, tgl_rel, tgl_pr, ina;

    lv_style_copy(&btnm_bg, &btn_rel);
    btnm_bg.body.padding.hor = 2;
    btnm_bg.body.padding.ver = 2;
    btnm_bg.body.padding.inner = 0;
    btnm_bg.body.border.width =  1;

    lv_style_copy(&rel, &btn_rel);
    rel.body.border.part = LV_BORDER_RIGHT;
    rel.body.border.width = 2;
    rel.body.radius = 0;

    lv_style_copy(&pr, &btn_pr);
    pr.body.border.part = LV_BORDER_RIGHT;
    pr.body.border.width = 2;
    pr.body.radius = 0;

    lv_style_copy(&tgl_rel, &btn_tgl_rel);
    tgl_rel.body.border.part = LV_BORDER_RIGHT;
    tgl_rel.body.border.width = 2;
    tgl_rel.body.radius = 0;

    lv_style_copy(&tgl_pr, &btn_tgl_pr);
    tgl_pr.body.border.part = LV_BORDER_RIGHT;
    tgl_pr.body.border.width = 2;
    tgl_pr.body.radius = 0;

    lv_style_copy(&ina, &btn_ina);
    ina.body.border.part = LV_BORDER_RIGHT;
    ina.body.border.width = 2;
    ina.body.radius = 0;

    theme.btnm.bg = &btnm_bg;
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
    theme.kb.bg = &bg;
    theme.kb.btn.rel = &btn_rel;
    theme.kb.btn.pr = &btn_pr;
    theme.kb.btn.tgl_rel = &btn_tgl_rel;
    theme.kb.btn.tgl_pr = &btn_tgl_pr;
    theme.kb.btn.ina = &btn_ina;
#endif

}

static void mbox_init(void)
{
#if USE_LV_MBOX
    static lv_style_t mbox_bg;
    lv_style_copy(&mbox_bg, &bg);
    mbox_bg.body.main_color =  lv_color_hsv_to_rgb(_hue, 30, 30);
    mbox_bg.body.grad_color =  lv_color_hsv_to_rgb(_hue, 30, 30);
    mbox_bg.body.border.color =  lv_color_hsv_to_rgb(_hue, 11, 20);
    mbox_bg.body.border.width = 1;
    mbox_bg.body.shadow.width = LV_DPI / 10;
    mbox_bg.body.shadow.color = LV_COLOR_HEX3(0x222);
    mbox_bg.body.radius = LV_DPI / 20;
    theme.mbox.bg = &mbox_bg;
    theme.mbox.btn.bg = &lv_style_transp;
    theme.mbox.btn.rel = &btn_rel;
    theme.mbox.btn.pr = &btn_pr;
#endif
}

static void page_init(void)
{
#if USE_LV_PAGE

    static lv_style_t page_scrl;
    lv_style_copy(&page_scrl, &bg);
    page_scrl.body.main_color = lv_color_hsv_to_rgb(_hue, 10, 40);
    page_scrl.body.grad_color = lv_color_hsv_to_rgb(_hue, 10, 40);
    page_scrl.body.border.color = LV_COLOR_HEX3(0x333);
    page_scrl.body.border.width = 1;
    page_scrl.body.radius = LV_DPI / 20;

    theme.page.bg = &panel;
    theme.page.scrl = &page_scrl;
    theme.page.sb = &sb;
#endif
}

static void ta_init(void)
{
#if USE_LV_TA
    theme.ta.area = &panel;
    theme.ta.oneline = &panel;
    theme.ta.cursor = NULL;
    theme.ta.sb = &def;
#endif
}

static void list_init(void)
{
#if USE_LV_LIST != 0
    static lv_style_t list_bg, list_btn_rel, list_btn_pr, list_btn_tgl_rel, list_btn_tgl_pr;

    lv_style_copy(&list_bg, &panel);
    list_bg.body.padding.ver = 0;
    list_bg.body.padding.hor = 0;
    list_bg.body.padding.inner = 0;

    lv_style_copy(&list_btn_rel, &bg);
    list_btn_rel.body.empty = 1;
    list_btn_rel.body.border.part = LV_BORDER_BOTTOM;
    list_btn_rel.body.border.color = lv_color_hsv_to_rgb(_hue, 10, 5);
    list_btn_rel.body.border.width = 1;
    list_btn_rel.body.radius = LV_DPI / 10;
    list_btn_rel.text.color = lv_color_hsv_to_rgb(_hue, 5, 80);
    list_btn_rel.body.padding.ver = LV_DPI / 6;
    list_btn_rel.body.padding.hor = LV_DPI / 8;

    lv_style_copy(&list_btn_pr, &btn_pr);
    list_btn_pr.body.main_color = btn_pr.body.grad_color;
    list_btn_pr.body.grad_color = btn_pr.body.main_color;
    list_btn_pr.body.border.color = lv_color_hsv_to_rgb(_hue, 10, 5);
    list_btn_pr.body.border.width = 0;
    list_btn_pr.body.radius = LV_DPI / 30;
    list_btn_pr.body.padding.ver = LV_DPI / 6;
    list_btn_pr.body.padding.hor = LV_DPI / 8;
    list_btn_pr.text.color = lv_color_hsv_to_rgb(_hue, 5, 80);

    lv_style_copy(&list_btn_tgl_rel, &list_btn_rel);
    list_btn_tgl_rel.body.empty = 0;
    list_btn_tgl_rel.body.main_color = lv_color_hsv_to_rgb(_hue, 10, 8);
    list_btn_tgl_rel.body.grad_color = lv_color_hsv_to_rgb(_hue, 10, 8);
    list_btn_tgl_rel.body.radius = LV_DPI / 30;

    lv_style_copy(&list_btn_tgl_pr, &list_btn_tgl_rel);
    list_btn_tgl_pr.body.main_color = btn_tgl_pr.body.main_color;
    list_btn_tgl_pr.body.grad_color = btn_tgl_pr.body.grad_color;

    theme.list.sb = &sb;
    theme.list.bg = &list_bg;
    theme.list.scrl = &lv_style_transp_tight;
    theme.list.btn.rel = &list_btn_rel;
    theme.list.btn.pr = &list_btn_pr;
    theme.list.btn.tgl_rel = &list_btn_tgl_rel;
    theme.list.btn.tgl_pr = &list_btn_tgl_pr;
    theme.list.btn.ina = &def;
#endif
}

static void ddlist_init(void)
{
#if USE_LV_DDLIST != 0
    lv_style_copy(&ddlist_bg, &btn_rel);
    ddlist_bg.text.line_space = LV_DPI / 8;
    ddlist_bg.body.padding.ver = LV_DPI / 8;
    ddlist_bg.body.padding.hor = LV_DPI / 8;
    ddlist_bg.body.radius = LV_DPI / 30;

    lv_style_copy(&ddlist_sel, &btn_rel);
    ddlist_sel.body.main_color = lv_color_hsv_to_rgb(_hue, 20, 50);
    ddlist_sel.body.grad_color = lv_color_hsv_to_rgb(_hue, 20, 50);
    ddlist_sel.body.radius = 0;

    theme.ddlist.bg = &ddlist_bg;
    theme.ddlist.sel = &ddlist_sel;
    theme.ddlist.sb = &def;
#endif
}

static void roller_init(void)
{
#if USE_LV_ROLLER != 0
    static lv_style_t roller_bg;

    lv_style_copy(&roller_bg, &ddlist_bg);
    roller_bg.body.main_color = lv_color_hsv_to_rgb(_hue, 10, 20);
    roller_bg.body.grad_color = lv_color_hsv_to_rgb(_hue, 10, 40);
    roller_bg.text.color = lv_color_hsv_to_rgb(_hue, 5, 70);
    roller_bg.text.opa = LV_OPA_60;

    theme.roller.bg = &roller_bg;
    theme.roller.sel = &ddlist_sel;
#endif
}

static void tabview_init(void)
{
#if USE_LV_TABVIEW != 0
    theme.tabview.bg = &bg;
    theme.tabview.indic = &lv_style_transp;
    theme.tabview.btn.bg = &lv_style_transp;
    theme.tabview.btn.rel = &btn_rel;
    theme.tabview.btn.pr = &btn_pr;
    theme.tabview.btn.tgl_rel = &btn_tgl_rel;
    theme.tabview.btn.tgl_pr = &btn_tgl_pr;
#endif
}


static void win_init(void)
{
#if USE_LV_WIN != 0
    static lv_style_t win_bg;
    lv_style_copy(&win_bg, &bg);
    win_bg.body.border.color = LV_COLOR_HEX3(0x333);
    win_bg.body.border.width = 1;

    static lv_style_t win_header;
    lv_style_copy(&win_header, &win_bg);
    win_header.body.main_color = lv_color_hsv_to_rgb(_hue, 10, 20);
    win_header.body.grad_color = lv_color_hsv_to_rgb(_hue, 10, 20);
    win_header.body.radius = 0;
    win_header.body.padding.hor = 0;
    win_header.body.padding.ver = 0;
    win_header.body.padding.ver = 0;

    static lv_style_t win_btn_pr;
    lv_style_copy(&win_btn_pr, &def);
    win_btn_pr.body.main_color = lv_color_hsv_to_rgb(_hue, 10, 10);
    win_btn_pr.body.grad_color = lv_color_hsv_to_rgb(_hue, 10, 10);
    win_btn_pr.text.color = LV_COLOR_HEX3(0xaaa);

    theme.win.bg = &win_bg;
    theme.win.sb = &sb;
    theme.win.header = &win_header;
    theme.win.content.bg = &lv_style_transp;
    theme.win.content.scrl = &lv_style_transp;
    theme.win.btn.rel = &lv_style_transp;
    theme.win.btn.pr = &win_btn_pr;
#endif
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/



/**
 * Initialize the night theme
 * @param hue [0..360] hue value from HSV color space to define the theme's base color
 * @param font pointer to a font (NULL to use the default)
 * @return pointer to the initialized theme
 */
lv_theme_t * lv_theme_night_init(uint16_t hue, lv_font_t * font)
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
lv_theme_t * lv_theme_get_night(void)
{
    return &theme;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif

