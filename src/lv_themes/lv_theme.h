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
#include "../../../lv_conf.h"
#endif

#include "../lv_core/lv_style.h"
#include "../lv_core/lv_group.h"

/*********************
 *    DEFINES
 *********************/

/**********************
 *    TYPEDEFS
 **********************/

/**
 * A theme in LittlevGL consists of many styles bound together.
 * 
 * There is a style for each object type, as well as a generic style for
 * backgrounds and panels.
 */
typedef struct
{
    struct
    {
        lv_style_t * scr;
        lv_style_t * bg;
        lv_style_t * panel;

#if LV_USE_CONT != 0
        lv_style_t * cont;
#endif

#if LV_USE_BTN != 0
        struct
        {
            lv_style_t * rel;
            lv_style_t * pr;
            lv_style_t * tgl_rel;
            lv_style_t * tgl_pr;
            lv_style_t * ina;
        } btn;
#endif

#if LV_USE_IMGBTN != 0
        struct
        {
            lv_style_t * rel;
            lv_style_t * pr;
            lv_style_t * tgl_rel;
            lv_style_t * tgl_pr;
            lv_style_t * ina;
        } imgbtn;
#endif

#if LV_USE_LABEL != 0
        struct
        {
            lv_style_t * prim;
            lv_style_t * sec;
            lv_style_t * hint;
        } label;
#endif

#if LV_USE_IMG != 0
        struct
        {
            lv_style_t * light;
            lv_style_t * dark;
        } img;
#endif

#if LV_USE_LINE != 0
        struct
        {
            lv_style_t * decor;
        } line;
#endif

#if LV_USE_LED != 0
        lv_style_t * led;
#endif

#if LV_USE_BAR != 0
        struct
        {
            lv_style_t * bg;
            lv_style_t * indic;
        } bar;
#endif

#if LV_USE_SLIDER != 0
        struct
        {
            lv_style_t * bg;
            lv_style_t * indic;
            lv_style_t * knob;
        } slider;
#endif

#if LV_USE_LMETER != 0
        lv_style_t * lmeter;
#endif

#if LV_USE_GAUGE != 0
        lv_style_t * gauge;
#endif

#if LV_USE_ARC != 0
        lv_style_t * arc;
#endif

#if LV_USE_PRELOAD != 0
        lv_style_t * preload;
#endif

#if LV_USE_SW != 0
        struct
        {
            lv_style_t * bg;
            lv_style_t * indic;
            lv_style_t * knob_off;
            lv_style_t * knob_on;
        } sw;
#endif

#if LV_USE_CHART != 0
        lv_style_t * chart;
#endif

#if LV_USE_CALENDAR != 0
        struct
        {
            lv_style_t * bg;
            lv_style_t * header;
            lv_style_t * header_pr;
            lv_style_t * day_names;
            lv_style_t * highlighted_days;
            lv_style_t * inactive_days;
            lv_style_t * week_box;
            lv_style_t * today_box;
        } calendar;
#endif

#if LV_USE_CB != 0
        struct
        {
            lv_style_t * bg;
            struct
            {
                lv_style_t * rel;
                lv_style_t * pr;
                lv_style_t * tgl_rel;
                lv_style_t * tgl_pr;
                lv_style_t * ina;
            } box;
        } cb;
#endif

#if LV_USE_BTNM != 0
        struct
        {
            lv_style_t * bg;
            struct
            {
                lv_style_t * rel;
                lv_style_t * pr;
                lv_style_t * tgl_rel;
                lv_style_t * tgl_pr;
                lv_style_t * ina;
            } btn;
        } btnm;
#endif

#if LV_USE_KB != 0
        struct
        {
            lv_style_t * bg;
            struct
            {
                lv_style_t * rel;
                lv_style_t * pr;
                lv_style_t * tgl_rel;
                lv_style_t * tgl_pr;
                lv_style_t * ina;
            } btn;
        } kb;
#endif

#if LV_USE_MBOX != 0
        struct
        {
            lv_style_t * bg;
            struct
            {
                lv_style_t * bg;
                lv_style_t * rel;
                lv_style_t * pr;
            } btn;
        } mbox;
#endif

#if LV_USE_PAGE != 0
        struct
        {
            lv_style_t * bg;
            lv_style_t * scrl;
            lv_style_t * sb;
        } page;
#endif

#if LV_USE_TA != 0
        struct
        {
            lv_style_t * area;
            lv_style_t * oneline;
            lv_style_t * cursor;
            lv_style_t * sb;
        } ta;
#endif

#if LV_USE_SPINBOX != 0
        struct
        {
            lv_style_t * bg;
            lv_style_t * cursor;
            lv_style_t * sb;
        } spinbox;
#endif

#if LV_USE_LIST
        struct
        {
            lv_style_t * bg;
            lv_style_t * scrl;
            lv_style_t * sb;
            struct
            {
                lv_style_t * rel;
                lv_style_t * pr;
                lv_style_t * tgl_rel;
                lv_style_t * tgl_pr;
                lv_style_t * ina;
            } btn;
        } list;
#endif

#if LV_USE_DDLIST != 0
        struct
        {
            lv_style_t * bg;
            lv_style_t * sel;
            lv_style_t * sb;
        } ddlist;
#endif

#if LV_USE_ROLLER != 0
        struct
        {
            lv_style_t * bg;
            lv_style_t * sel;
        } roller;
#endif

#if LV_USE_TABVIEW != 0
        struct
        {
            lv_style_t * bg;
            lv_style_t * indic;
            struct
            {
                lv_style_t * bg;
                lv_style_t * rel;
                lv_style_t * pr;
                lv_style_t * tgl_rel;
                lv_style_t * tgl_pr;
            } btn;
        } tabview;
#endif

#if LV_USE_TILEVIEW != 0
        struct
        {
            lv_style_t * bg;
            lv_style_t * scrl;
            lv_style_t * sb;
        } tileview;
#endif

#if LV_USE_TABLE != 0
        struct
        {
            lv_style_t * bg;
            lv_style_t * cell;
        } table;
#endif

#if LV_USE_WIN != 0
        struct
        {
            lv_style_t * bg;
            lv_style_t * sb;
            lv_style_t * header;
            lv_style_t * content;
            struct
            {
                lv_style_t * rel;
                lv_style_t * pr;
            } btn;
        } win;
#endif
    } style;

#if LV_USE_GROUP
    struct
    {
        /* The `x` in the names inidicates that inconsistence becasue
         * the group related function are stored in the theme.*/
        lv_group_style_mod_cb_t style_mod_xcb;
        lv_group_style_mod_cb_t style_mod_edit_xcb;
    } group;
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
void lv_theme_set_current(lv_theme_t * th);

/**
 * Get the current system theme.
 * @return pointer to the current system theme. NULL if not set.
 */
lv_theme_t * lv_theme_get_current(void);

/**********************
 *    MACROS
 **********************/

/* Returns number of styles within the `lv_theme_t` structure. */
#define LV_THEME_STYLE_COUNT (sizeof(((lv_theme_t *)0)->style) / sizeof(lv_style_t *))

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
