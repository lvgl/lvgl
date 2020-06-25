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
#include "../lv_core/lv_obj.h"

/*********************
 *    DEFINES
 *********************/

/**********************
 *    TYPEDEFS
 **********************/

/**
 * A theme in LVGL consists of many styles bound together.
 *
 * There is a style for each object type, as well as a generic style for
 * backgrounds and panels.
 */
typedef enum {
    LV_THEME_NONE = 0,
    LV_THEME_SCR,
    LV_THEME_OBJ,
#if LV_USE_ARC
    LV_THEME_ARC,
#endif
#if LV_USE_BAR
    LV_THEME_BAR,
#endif
#if LV_USE_BTN
    LV_THEME_BTN,
#endif
#if LV_USE_BTNMATRIX
    LV_THEME_BTNMATRIX,
#endif
#if LV_USE_CALENDAR
    LV_THEME_CALENDAR,
#endif
#if LV_USE_CANVAS
    LV_THEME_CANVAS,
#endif
#if LV_USE_CHECKBOX
    LV_THEME_CHECKBOX,
#endif
#if LV_USE_CHART
    LV_THEME_CHART,
#endif
#if LV_USE_CONT
    LV_THEME_CONT,
#endif
#if LV_USE_CPICKER
    LV_THEME_CPICKER,
#endif
#if LV_USE_DROPDOWN
    LV_THEME_DROPDOWN,
#endif
#if LV_USE_GAUGE
    LV_THEME_GAUGE,
#endif
#if LV_USE_IMG
    LV_THEME_IMAGE,
#endif
#if LV_USE_IMGBTN
    LV_THEME_IMGBTN,
#endif
#if LV_USE_KEYBOARD
    LV_THEME_KEYBOARD,
#endif
#if LV_USE_LABEL
    LV_THEME_LABEL,
#endif
#if LV_USE_LED
    LV_THEME_LED,
#endif
#if LV_USE_LINE
    LV_THEME_LINE,
#endif
#if LV_USE_LIST
    LV_THEME_LIST,
    LV_THEME_LIST_BTN,
#endif
#if LV_USE_LINEMETER
    LV_THEME_LINEMETER,
#endif
#if LV_USE_MSGBOX
    LV_THEME_MSGBOX,
    LV_THEME_MSGBOX_BTNS,   /*The button matrix of the buttons are initialized separately*/
#endif
#if LV_USE_OBJMASK
    LV_THEME_OBJMASK,
#endif
#if LV_USE_PAGE
    LV_THEME_PAGE,
#endif
#if LV_USE_ROLLER
    LV_THEME_ROLLER,
#endif
#if LV_USE_SLIDER
    LV_THEME_SLIDER,
#endif
#if LV_USE_SPINBOX
    LV_THEME_SPINBOX,
    LV_THEME_SPINBOX_BTN,   /*Button extra for spinbox*/
#endif
#if LV_USE_SPINNER
    LV_THEME_SPINNER,
#endif
#if LV_USE_SWITCH
    LV_THEME_SWITCH,
#endif
#if LV_USE_TABLE
    LV_THEME_TABLE,
#endif
#if LV_USE_TABVIEW
    LV_THEME_TABVIEW,
    LV_THEME_TABVIEW_PAGE,  /*The tab pages are initialized separately*/
#endif
#if LV_USE_TEXTAREA
    LV_THEME_TEXTAREA,
#endif
#if LV_USE_TILEVIEW
    LV_THEME_TILEVIEW,
#endif
#if LV_USE_WIN
    LV_THEME_WIN,
    LV_THEME_WIN_BTN,   /*The buttons are initialized separately*/
#endif

    _LV_THEME_BUILTIN_LAST,
    LV_THEME_CUSTOM_START = _LV_THEME_BUILTIN_LAST,
    _LV_THEME_CUSTOM_LAST = 0xFFFF,

} lv_theme_style_t;

typedef struct _lv_theme_t {
    void (*apply_xcb)(lv_obj_t *, lv_theme_style_t); /*Deprecated: use `apply_cb` instead*/
    void (*apply_cb)(struct _lv_theme_t *, lv_obj_t *, lv_theme_style_t);
    lv_color_t color_primary;
    lv_color_t color_secondary;
    const lv_font_t * font_small;
    const lv_font_t * font_normal;
    const lv_font_t * font_subtitle;
    const lv_font_t * font_title;
    uint32_t flags;
    void * user_data;
} lv_theme_t;

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


void lv_theme_apply(lv_obj_t * obj, lv_theme_style_t name);


/**
 * Get the small font of the theme
 * @return pointer to the font
 */
const lv_font_t * lv_theme_get_font_small(void);

/**
 * Get the normal font of the theme
 * @return pointer to the font
 */
const lv_font_t * lv_theme_get_font_normal(void);

/**
 * Get the subtitle font of the theme
 * @return pointer to the font
 */
const lv_font_t * lv_theme_get_font_subtitle(void);

/**
 * Get the title font of the theme
 * @return pointer to the font
 */
const lv_font_t * lv_theme_get_font_title(void);

/**
 * Get the primary color of the theme
 * @return the color
 */
lv_color_t lv_theme_get_color_primary(void);

/**
 * Get the secondary color of the theme
 * @return the color
 */
lv_color_t lv_theme_get_color_secondary(void);

/**
 * Get the flags of the theme
 * @return the flags
 */
uint32_t lv_theme_get_flags(void);


/**********************
 *    MACROS
 **********************/

/**********************
 *     POST INCLUDE
 *********************/
#include "lv_theme_empty.h"
#include "lv_theme_template.h"
#include "lv_theme_material.h"
#include "lv_theme_mono.h"

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_THEMES_H*/
