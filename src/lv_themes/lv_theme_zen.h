/**
 * @file lv_theme_zen.h
 *
 */

#ifndef LV_THEME_ZEN_H
#define LV_THEME_ZEN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../../lv_conf.h"
#endif

#if LV_USE_THEME_ZEN

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the zen theme
 * @param hue [0..360] hue value from HSV color space to define the theme's base color
 * @param font pointer to a font (NULL to use the default)
 * @return pointer to the initialized theme
 */
lv_theme_t * lv_theme_zen_init(uint16_t hue, lv_font_t * font);

/**
 * Get a pointer to the theme
 * @return pointer to the theme
 */
lv_theme_t * lv_theme_get_zen(void);

/**********************
 *      MACROS
 **********************/

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_THEME_ZEN_H*/
