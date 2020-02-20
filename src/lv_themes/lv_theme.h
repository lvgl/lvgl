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
    LV_THEME_ARC,
    LV_THEME_BAR,
    LV_THEME_BTN,
    LV_THEME_BTNMATRIX,
    LV_THEME_CALENDAR,
    LV_THEME_CANVAS,
    LV_THEME_CHECKBOX,
    LV_THEME_CHART,
    LV_THEME_CONT,
    LV_THEME_CPICKER,
    LV_THEME_DROPDOWN,
    LV_THEME_GAUGE,
    LV_THEME_IMAGE,
    LV_THEME_IMGBTN,
    LV_THEME_KEYBOARD,
    LV_THEME_LABEL,
    LV_THEME_LED,
    LV_THEME_LINE,
    LV_THEME_LIST,
    LV_THEME_LIST_BTN,
    LV_THEME_LINEMETER,
    LV_THEME_MSGBOX,
    LV_THEME_MSGBOX_BTNS,   /*The button matrix of the buttons are initialized separately*/
    LV_THEME_OBJMASK,
    LV_THEME_PAGE,
    LV_THEME_ROLLER,
    LV_THEME_SLIDER,
    LV_THEME_SPINBOX,
    LV_THEME_SPINBOX_BTN,   /*Button extra for spinbox*/
    LV_THEME_SPINNER,
    LV_THEME_SWITCH,
    LV_THEME_TABLE,
    LV_THEME_TABVIEW,
    LV_THEME_TABVIEW_PAGE,  /*The tab pages are initialized separately*/
    LV_THEME_TEXTAREA,
    LV_THEME_TEXTAREA_ONELINE,
    LV_THEME_TILEVIEW,
    LV_THEME_WIN,
    LV_THEME_WIN_BTN,   /*The buttons are initialized separately*/
}lv_theme_style_t;

typedef struct {
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

/**********************
 *     POST INCLUDE
 *********************/
#include "lv_theme_material.h"

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_THEMES_H*/
