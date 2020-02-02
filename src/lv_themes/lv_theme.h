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
#include "../lv_conf_internal.h"

#include "../lv_core/lv_style.h"

#include "../../lvgl.h"

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
typedef enum {
    LV_THEME_NONE = 0,
    LV_THEME_SCR,
    LV_THEME_OBJ,

    LV_THEME_CONT,

    LV_THEME_BTN,

    LV_THEME_LABEL,

    LV_THEME_IMAGE,
    LV_THEME_IMGBTN,

    LV_THEME_BTNM,

    LV_THEME_BAR,

    LV_THEME_SLIDER,

    LV_THEME_SW,

    LV_THEME_CB,
    LV_THEME_CB_BULLET,

    LV_THEME_PAGE,

    LV_THEME_DDLIST,
    LV_THEME_DDLIST_PAGE,

    LV_THEME_ROLLER,

    LV_THEME_TABVIEW,
    LV_THEME_TABVIEW_PAGE,

    LV_THEME_LMETER,

    LV_THEME_GAUGE,
    LV_THEME_GAUGE_STRONG,

    LV_THEME_TA,
    LV_THEME_TA_ONELINE,
    LV_THEME_TA_CURSOR,
    LV_THEME_TA_SCRLBAR,


    LV_THEME_LIST,
    LV_THEME_LIST_BTN,


    LV_THEME_CALENDAR,

    LV_THEME_ARC,

    LV_THEME_LED,

    LV_THEME_MBOX_BG,
    LV_THEME_MBOX_BTN_BG,
    LV_THEME_MBOX_BTN,

    LV_THEME_TABLE,
    LV_THEME_TABLE_BG,
    LV_THEME_TABLE_CELL1,
    LV_THEME_TABLE_CELL2,
    LV_THEME_TABLE_CELL3,
    LV_THEME_TABLE_CELL4,

    LV_THEME_CHART,
    LV_THEME_CHART_BG,
    LV_THEME_CHART_SERIES_BG,
    LV_THEME_CHART_SERIES,

}lv_theme_style_t;

typedef struct {
    lv_style_t * (*get_style_cb)(lv_theme_style_t);
    void (*apply_cb)(lv_obj_t *,lv_theme_style_t);
}lv_theme_t;

/**********************
 *  GLOBAL PROTOTYPES
 **********************/

/**
 * Set a theme for the system.
 * From now, all the created objects will use styles from this theme by default
 * @param th pointer to theme (return value of: 'lv_theme_init_xxx()')
 */
void lv_theme_set_act(lv_theme_t * th);

/**
 * Get the current system theme.
 * @return pointer to the current system theme. NULL if not set.
 */
lv_theme_t * lv_theme_get_act(void);

lv_style_t * lv_theme_get_style(lv_theme_style_t name);

void lv_theme_apply(lv_obj_t * obj, lv_theme_style_t name);

lv_style_t * lv_theme_get_style_part(lv_theme_style_t name, uint8_t part);

/**********************
 *    MACROS
 **********************/
#define _t(name) lv_theme_get_style(LV_THEME_ ## name)
#define _ot(obj, part, name) lv_obj_add_style(obj, part, _t(name))

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
