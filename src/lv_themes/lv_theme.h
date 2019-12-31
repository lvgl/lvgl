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
    LV_THEME_SCR,
    LV_THEME_SCR_TRANSP,

    LV_THEME_PANEL,
    LV_THEME_PANEL_TRANSP,
    LV_THEME_PANEL_FRAME,
    LV_THEME_PANEL_TIGHT,
    LV_THEME_PANEL_FIT,

    LV_THEME_BTN,
    LV_THEME_BTN_DANGER,
    LV_THEME_BTN_APPROVE,
    LV_THEME_BTN_TRANSP,
    LV_THEME_BTN_FRAME,
    LV_THEME_BTN_LARGE,
    LV_THEME_BTN_SMALL,

    LV_THEME_LABEL,
    LV_THEME_LABEL_TITLE,
    LV_THEME_LABEL_HINT,


    LV_THEME_BTNM,
    LV_THEME_BTNM_BTN,

}lv_theme_style_t;

typedef struct {
    lv_style_t * (*get_style_cb)(lv_theme_style_t);
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

/**********************
 *    MACROS
 **********************/
#define _t(name) lv_theme_get_style(LV_THEME_ ## name)

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
