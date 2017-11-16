/**
 * @file lv_theme.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lvgl.h"
#include "lv_theme.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void create_tab1(lv_theme_t * th, lv_obj_t *parent);
static void create_tab2(lv_theme_t * th, lv_obj_t *parent);
static void create_tab3(lv_theme_t * th, lv_obj_t *parent);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_theme_t *current_theme;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Set a theme for the system.
 * From now, all the created objects will use styles from this theme by default
 * @param th pointer to theme (return value of: 'lv_theme_init_xxx()')
 */
void lv_theme_set_current(lv_theme_t *th)
{
    current_theme = th;
}

/**
 * Get the current system theme.
 * @return pointer to the current system theme. NULL if not set.
 */
lv_theme_t * lv_theme_get_current(void)
{
    return current_theme;
}


/**
 * Create a test screen with a lot objects and apply the given theme on them
 * @param th pointer to a theme
 */
void lv_theme_create_test_screen(lv_theme_t *th)
{
    lv_theme_set_current(th);
    lv_obj_t *scr = lv_cont_create(NULL, NULL);
    lv_scr_load(scr);
    lv_cont_set_style(scr, th->bg);


    lv_obj_t *tv = lv_tabview_create(scr, NULL);

   lv_obj_set_size(tv, LV_HOR_RES, LV_VER_RES);
   lv_obj_t *tab1 = lv_tabview_add_tab(tv, "Tab 1");
   lv_obj_t *tab2 = lv_tabview_add_tab(tv, "Tab 2");
   lv_obj_t *tab3 = lv_tabview_add_tab(tv, "Tab 3");

   create_tab1(th, tab1);
   create_tab2(th, tab2);
   create_tab3(th, tab3);

   lv_tabview_set_current_tab(tv, 2, false);

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void create_tab1(lv_theme_t * th, lv_obj_t *parent)
{
    lv_page_set_scrl_layout(parent, LV_CONT_LAYOUT_PRETTY);

    static lv_style_t h_style;
    lv_style_copy(&h_style, &lv_style_transp);
    h_style.body.padding.inner = LV_DPI / 4;
    h_style.body.padding.hor = LV_DPI / 4;
    h_style.body.padding.ver = LV_DPI / 6;

    lv_obj_t *h = lv_cont_create(parent, NULL);
    lv_obj_set_style(h, &h_style);
    lv_cont_set_fit(h, true, true);
    lv_cont_set_layout(h, LV_CONT_LAYOUT_COL_M);

    lv_obj_t *btn = lv_btn_create(h, NULL);
    lv_btn_set_style(btn, LV_BTN_STYLE_REL, th->btn.sm.rel);
    lv_btn_set_style(btn, LV_BTN_STYLE_PR, th->btn.sm.pr);
    lv_btn_set_style(btn, LV_BTN_STYLE_TGL_REL, th->btn.sm.tgl_rel);
    lv_btn_set_style(btn, LV_BTN_STYLE_TGL_PR, th->btn.sm.tgl_pr);
    lv_btn_set_style(btn, LV_BTN_STYLE_INA, th->btn.sm.ina);
    lv_btn_set_fit(btn, true, true);
    lv_btn_set_toggle(btn, true);
    lv_obj_t *btn_label = lv_label_create(btn, NULL);
    lv_label_set_text(btn_label, "Small");

    btn = lv_btn_create(h, btn);
    lv_btn_toggle(btn);
    btn_label = lv_label_create(btn, NULL);
    lv_label_set_text(btn_label, "Toggled");

    btn = lv_btn_create(h, btn);
    lv_btn_set_state(btn, LV_BTN_STATE_INA);
    btn_label = lv_label_create(btn, NULL);
    lv_label_set_text(btn_label, "Inactive");

    btn = lv_btn_create(h, btn);
    lv_btn_set_state(btn, LV_BTN_STATE_REL);

    btn_label = lv_label_create(btn, NULL);
    lv_label_set_text(btn_label, "Medium");

    btn = lv_btn_create(h, btn);
    lv_btn_set_style(btn, LV_BTN_STYLE_REL, th->btn.lg.rel);
    lv_btn_set_style(btn, LV_BTN_STYLE_PR, th->btn.lg.pr);
    lv_btn_set_style(btn, LV_BTN_STYLE_TGL_REL, th->btn.lg.tgl_rel);
    lv_btn_set_style(btn, LV_BTN_STYLE_TGL_PR, th->btn.lg.tgl_pr);
    lv_btn_set_style(btn, LV_BTN_STYLE_INA, th->btn.lg.ina);
    btn_label = lv_label_create(btn, NULL);
    lv_label_set_text(btn_label, "Large");

    lv_obj_t *label = lv_label_create(h, NULL);
    lv_label_set_text(label, "Small");
    lv_obj_set_style(label, th->label.sm);

    label = lv_label_create(h, NULL);
    lv_label_set_text(label, "Medium");
    lv_obj_set_style(label, th->label.md);

    label = lv_label_create(h, NULL);
    lv_label_set_text(label, "Large");
    lv_obj_set_style(label, th->label.lg);
    lv_obj_set_protect(label, LV_PROTECT_FOLLOW);

    h = lv_cont_create(parent, h);

    lv_obj_t *sw_h = lv_cont_create(h, NULL);
    lv_cont_set_style(sw_h, &lv_style_transp);
    lv_cont_set_fit(sw_h, false, true);
    lv_obj_set_width(sw_h, LV_HOR_RES / 4);
    lv_cont_set_layout(sw_h, LV_CONT_LAYOUT_PRETTY);

    lv_obj_t *sw = lv_sw_create(sw_h, NULL);

    sw = lv_sw_create(sw_h, sw);
    lv_sw_set_on(sw);

    lv_obj_t *bar = lv_bar_create(h, NULL);
    lv_bar_set_value(bar, 70);

    lv_obj_t *slider = lv_slider_create(h, NULL);
    lv_obj_set_height(slider, LV_DPI / 2);
    lv_bar_set_value(slider, 70);

    lv_obj_t *line = lv_line_create(h, NULL);
    static const point_t line_p[] = {{0,0},{LV_HOR_RES / 5, 0}};
    lv_line_set_points(line, line_p, 2);
    lv_line_set_style(line, th->line.decor);

    lv_obj_t *ta = lv_ta_create(h, NULL);
    lv_obj_set_style(ta, th->ta.oneline);
    lv_ta_set_text(ta, "Some text");
    lv_ta_set_one_line(ta, true);

    lv_obj_t *cb = lv_cb_create(h, NULL);

    cb = lv_cb_create(h, cb);
    lv_btn_set_state(cb, LV_BTN_STATE_TGL_REL);


    lv_obj_t *ddlist = lv_ddlist_create(h, NULL);
    lv_ddlist_open(ddlist, false);
    lv_ddlist_set_selected(ddlist, 1);


    static const char *btnm_str[] = {"1", "2", "3", SYMBOL_OK, SYMBOL_CLOSE, ""};

    lv_obj_t *btnm = lv_btnm_create(h, NULL);
    lv_obj_set_size(btnm,LV_HOR_RES / 4, 2 * LV_DPI / 3);
    lv_btnm_set_map(btnm, btnm_str);
    ((lv_btnm_ext_t *) btnm->ext_attr)->btn_id_pr = 3; /*Hack to show a button pressed*/

    h = lv_cont_create(parent, h);

    lv_obj_t * list = lv_list_create(h, NULL);
    lv_list_add(list, SYMBOL_GPS,  "GPS",  NULL);
    lv_list_add(list, SYMBOL_WIFI, "WiFi", NULL);
    lv_list_add(list, SYMBOL_CALL, "Call", NULL);
    lv_list_add(list, SYMBOL_BELL, "Bell", NULL);
    lv_list_add(list, SYMBOL_FILE, "File", NULL);
    lv_list_add(list, SYMBOL_EDIT, "Edit", NULL);
    lv_list_add(list, SYMBOL_CUT,  "Cut",  NULL);
    lv_list_add(list, SYMBOL_COPY, "Copy", NULL);

    lv_obj_t *roller = lv_roller_create(h, NULL);
    lv_roller_set_options(roller, "Monday\nTuesday\nWednesday\nThursday\nFriday\nSaturday\nSunday");
    lv_obj_set_height(roller, LV_DPI);

    lv_obj_t *gauge = lv_gauge_create(h, NULL);
    lv_gauge_set_value(gauge, 0, 40);
    lv_obj_set_size(gauge, 3 * LV_DPI / 2, 3 * LV_DPI / 2);
}

static void create_tab2(lv_theme_t * th, lv_obj_t *parent)
{
    cord_t w = lv_page_get_scrl_width(parent);

    lv_obj_t *chart = lv_chart_create(parent, NULL);
    lv_chart_series_t * s1 = lv_chart_add_series(chart, COLOR_RED);
    lv_chart_set_next(chart, s1, 30);
    lv_chart_set_next(chart, s1, 20);
    lv_chart_set_next(chart, s1, 10);
    lv_chart_set_next(chart, s1, 12);
    lv_chart_set_next(chart, s1, 20);
    lv_chart_set_next(chart, s1, 27);
    lv_chart_set_next(chart, s1, 35);
    lv_chart_set_next(chart, s1, 55);
    lv_chart_set_next(chart, s1, 70);
    lv_chart_set_next(chart, s1, 75);

    lv_obj_t *ta = lv_ta_create(parent, NULL);
    lv_obj_set_size(ta, w / 3, LV_VER_RES / 4);
    lv_obj_align(ta, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 0);
    lv_ta_set_cursor_type(ta, LV_TA_CURSOR_BLOCK);

    lv_obj_t *kb = lv_kb_create(parent, NULL);
    lv_obj_set_size(kb, w / 2, LV_VER_RES / 3);
    lv_obj_align(kb, ta, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, LV_DPI / 4);
    lv_kb_set_ta(kb, ta);
}


static void create_tab3(lv_theme_t * th, lv_obj_t *parent)
{
    lv_obj_t *win = lv_win_create(parent, NULL);
    lv_win_add_btn(win, SYMBOL_CLOSE, lv_win_close_action);
    lv_win_add_btn(win, SYMBOL_DOWN, NULL);
    lv_obj_set_size(win, LV_HOR_RES / 2, LV_VER_RES / 2);
    lv_obj_set_top(win, true);

    lv_obj_t *label = lv_label_create(win, NULL);
    lv_label_set_text(label, "Label in the window");

    lv_obj_t *lmeter = lv_lmeter_create(win, NULL);
    lv_obj_align(lmeter, label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, LV_DPI / 2);
    lv_lmeter_set_value(lmeter, 70);

    lv_obj_t *led1 = lv_led_create(win, NULL);
    lv_obj_align(led1, lmeter, LV_ALIGN_OUT_BOTTOM_MID, 0, LV_DPI / 2);
    lv_led_on(led1);

    lv_obj_t *led2 = lv_led_create(win, NULL);
    lv_obj_align(led2, led1, LV_ALIGN_OUT_RIGHT_MID, LV_DPI / 2, 0);
    lv_led_off(led2);


    lv_obj_t *page = lv_page_create(parent, NULL);
    lv_obj_set_size(page, LV_HOR_RES / 3, LV_VER_RES / 2);
    lv_obj_set_top(page, true);
    lv_obj_align(page, win, LV_ALIGN_IN_TOP_RIGHT,  LV_DPI, LV_DPI);

    label = lv_label_create(page, NULL);
    lv_label_set_text(label, "Lorem ipsum dolor sit amet, repudiare voluptatibus pri cu. "
                             "Ei mundi pertinax posidonium eum, cum tempor maiorum at, "
                             "mea fuisset assentior ad. Usu cu suas civibus iudicabit. "
                             "Eum eu congue tempor facilisi. Tale hinc unum te vim. "
                             "Te cum populo animal eruditi, labitur inciderint at nec.\n\n"
                             "Eius corpora et quo. Everti voluptaria instructior est id, "
                             "vel in falli primis. Mea ei porro essent admodum, "
                             "his ei malis quodsi, te quis aeterno his. "
                             "Qui tritani recusabo reprehendunt ne, "
                             "per duis explicari at. Simul mediocritatem mei et.");
    lv_label_set_long_mode(label, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(label, lv_page_get_scrl_width(page));


    static const char * mbox_btn_map[] = {"\211", "\222Got it!", "\211", ""};
    lv_obj_t *mbox = lv_mbox_create(parent, NULL);
    lv_mbox_set_text(mbox, "Click on the window or the page to bring it to the foreground");
    lv_mbox_set_btns(mbox, mbox_btn_map, NULL);
    lv_obj_set_top(mbox, true);
}


