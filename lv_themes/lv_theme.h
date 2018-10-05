/**
 *@file lv_themes.h
 *
 */

#ifndef LV_THEMES_H
#define LV_THEMES_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *    INCLUDES
 *********************/
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../lv_conf.h"
#endif

#include "../lv_core/lv_style.h"

/*********************
 *    DEFINES
 *********************/

/**********************
 *    TYPEDEFS
 **********************/

typedef struct {
    lv_style_t *bg;
    lv_style_t *panel;

#if USE_LV_CONT != 0
    lv_style_t *cont;
#endif

#if USE_LV_BTN != 0
    struct {
        lv_style_t *rel;
        lv_style_t *pr;
        lv_style_t *tgl_rel;
        lv_style_t *tgl_pr;
        lv_style_t *ina;
    } btn;
#endif


#if USE_LV_IMGBTN != 0
    struct {
        lv_style_t *rel;
        lv_style_t *pr;
        lv_style_t *tgl_rel;
        lv_style_t *tgl_pr;
        lv_style_t *ina;
    } imgbtn;
#endif

#if USE_LV_LABEL != 0
    struct {
        lv_style_t *prim;
        lv_style_t *sec;
        lv_style_t *hint;
    } label;
#endif

#if USE_LV_IMG != 0
    struct {
        lv_style_t *light;
        lv_style_t *dark;
    } img;
#endif

#if USE_LV_LINE != 0
    struct {
        lv_style_t *decor;
    } line;
#endif

#if USE_LV_LED != 0
    lv_style_t *led;
#endif

#if USE_LV_BAR != 0
    struct {
        lv_style_t *bg;
        lv_style_t *indic;
    } bar;
#endif

#if USE_LV_SLIDER != 0
    struct {
        lv_style_t *bg;
        lv_style_t *indic;
        lv_style_t *knob;
    } slider;
#endif

#if USE_LV_LMETER != 0
    lv_style_t *lmeter;
#endif

#if USE_LV_GAUGE != 0
    lv_style_t *gauge;
#endif

#if USE_LV_ARC != 0
    lv_style_t *arc;
#endif

#if USE_LV_PRELOAD != 0
    lv_style_t *preload;
#endif

#if USE_LV_SW != 0
    struct {
        lv_style_t *bg;
        lv_style_t *indic;
        lv_style_t *knob_off;
        lv_style_t *knob_on;
    } sw;
#endif

#if USE_LV_CHART != 0
    lv_style_t *chart;
#endif

#if USE_LV_CALENDAR != 0
    struct {
        lv_style_t *bg;
        lv_style_t *header;
        lv_style_t *header_pr;
        lv_style_t *day_names;
        lv_style_t *highlighted_days;
        lv_style_t *inactive_days;
        lv_style_t *week_box;
        lv_style_t *today_box;
    } calendar;
#endif

#if USE_LV_CB != 0
    struct {
        lv_style_t *bg;
        struct {
            lv_style_t *rel;
            lv_style_t *pr;
            lv_style_t *tgl_rel;
            lv_style_t *tgl_pr;
            lv_style_t *ina;
        } box;
    } cb;
#endif

#if USE_LV_BTNM != 0
    struct {
        lv_style_t *bg;
        struct {
            lv_style_t *rel;
            lv_style_t *pr;
            lv_style_t *tgl_rel;
            lv_style_t *tgl_pr;
            lv_style_t *ina;
        } btn;
    } btnm;
#endif

#if USE_LV_KB != 0
    struct {
        lv_style_t *bg;
        struct {
            lv_style_t *rel;
            lv_style_t *pr;
            lv_style_t *tgl_rel;
            lv_style_t *tgl_pr;
            lv_style_t *ina;
        } btn;
    } kb;
#endif

#if USE_LV_MBOX != 0
    struct {
        lv_style_t *bg;
        struct {
            lv_style_t *bg;
            lv_style_t *rel;
            lv_style_t *pr;
        } btn;
    } mbox;
#endif

#if USE_LV_PAGE != 0
    struct {
        lv_style_t *bg;
        lv_style_t *scrl;
        lv_style_t *sb;
    } page;
#endif

#if USE_LV_TA != 0
    struct {
        lv_style_t *area;
        lv_style_t *oneline;
        lv_style_t *cursor;
        lv_style_t *sb;
    } ta;
#endif

#if USE_LV_LIST
    struct {
        lv_style_t *bg;
        lv_style_t *scrl;
        lv_style_t *sb;
        struct {
            lv_style_t *rel;
            lv_style_t *pr;
            lv_style_t *tgl_rel;
            lv_style_t *tgl_pr;
            lv_style_t *ina;
        } btn;
    } list;
#endif

#if USE_LV_DDLIST != 0
    struct {
        lv_style_t *bg;
        lv_style_t *sel;
        lv_style_t *sb;
    } ddlist;
#endif

#if USE_LV_ROLLER != 0
    struct {
        lv_style_t *bg;
        lv_style_t *sel;
    } roller;
#endif

#if USE_LV_TABVIEW != 0
    struct {
        lv_style_t *bg;
        lv_style_t *indic;
        struct {
            lv_style_t *bg;
            lv_style_t *rel;
            lv_style_t *pr;
            lv_style_t *tgl_rel;
            lv_style_t *tgl_pr;
        } btn;
    } tabview;
#endif

#if USE_LV_WIN != 0
    struct {
        lv_style_t *bg;
        lv_style_t *sb;
        lv_style_t *header;
        struct {
            lv_style_t *bg;
            lv_style_t *scrl;
        } content;
        struct {
            lv_style_t *rel;
            lv_style_t *pr;
        } btn;
    } win;
#endif
} lv_theme_t;

/**********************
 *  GLOBAL PROTOTYPES
 **********************/

/**
 * Set a theme for the system.
 * From now, all the created objects will use styles from this theme by default
 * @param th pointer to theme (return value of: 'lv_theme_init_xxx()')
 */
void lv_theme_set_current(lv_theme_t *th);

/**
 * Get the current system theme.
 * @return pointer to the current system theme. NULL if not set.
 */
lv_theme_t * lv_theme_get_current(void);

/**********************
 *    MACROS
 **********************/

/**********************
 *     POST INCLUDE
 *********************/
#include "lv_theme_templ.h"
#include "lv_theme_default.h"
#include "lv_theme_alien.h"
#include "lv_theme_night.h"
#include "lv_theme_zen.h"
#include "lv_theme_mono.h"
#include "lv_theme_nemo.h"
#include "lv_theme_material.h"


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_THEMES_H*/
