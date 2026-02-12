/**
 * @file lv_theme_private.h
 *
 */

#ifndef LV_THEME_PRIVATE_H
#define LV_THEME_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_theme.h"

#if LV_USE_EXT_DATA
#include "../lvgl_private.h"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_theme_t {
#if LV_USE_EXT_DATA
    lv_ext_data_t ext_data;
#endif
    lv_theme_apply_cb_t apply_cb;
    lv_theme_t * parent;            /**< Apply the current theme's style on top of this theme. */
    void * user_data;
    lv_display_t * disp;
    lv_color_t color_primary;
    lv_color_t color_secondary;
    const lv_font_t * font_small;
    const lv_font_t * font_normal;
    const lv_font_t * font_large;
    uint32_t flags;                 /**< Any custom flag used by the theme */
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_THEME_PRIVATE_H*/
