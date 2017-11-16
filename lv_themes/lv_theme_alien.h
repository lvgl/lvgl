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
#include "lv_conf.h"

#if USE_LV_THEME_ALIEN

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
 * Initalize the alien theme
 * @param hue [0..360] hue value from HSV color space to define the theme's base color
 * @param font_sm pointer to a small font (NULL to use the default)
 * @param font_md pointer to a medium font (NULL to use the default)
 * @param font_lg pointer to a large font (NULL to use the default)
 */
void lv_theme_alien_init(uint16_t hue, font_t *font_sm, font_t *font_md, font_t *font_lg);

/**
 * Get a pointer to the theme
 * @return pointer to the theme
 */
lv_theme_t * lv_theme_get_alien(void);

/**********************
 *      MACROS
 **********************/

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_THEME_ALIEN_H*/
