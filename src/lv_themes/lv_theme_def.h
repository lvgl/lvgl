/**
 * @file lv_theme_alien.h
 *
 */

#ifndef LV_THEME_ALIEN_H
#define LV_THEME_ALIEN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#if LV_USE_THEME_ALIEN

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
 * Initialize the alien theme
 * @param hue [0..360] hue value from HSV color space to define the theme's base color
 * @param font pointer to a font (NULL to use the default)
 */
lv_theme_t * lv_theme_alien_init(uint16_t hue, lv_font_t * font);

lv_theme_t * lv_theme_alien_get(void);
lv_style_t * lv_theme_alien_get_style_part(lv_theme_style_t name, uint8_t part);

lv_style_t * lv_theme_alien_get_style(lv_theme_style_t name);

/**********************
 *      MACROS
 **********************/

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_THEME_ALIEN_H*/
