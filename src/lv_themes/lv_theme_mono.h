/**
 * @file lv_theme_mono.h
 *
 */

#ifndef LV_THEME_MONO_H
#define LV_THEME_MONO_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#if LV_USE_THEME_MONO

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
 * Initialize the mono theme.
 * @param color_primary the primary color of the theme
 * @param color_secondary the secondary color for the theme
 * @param flags ORed flags starting with `LV_THEME_DEF_FLAG_...`
 * @param font_small pointer to a small font
 * @param font_normal pointer to a normal font
 * @param font_subtitle pointer to a large font
 * @param font_title pointer to a extra large font
 * @return a pointer to reference this theme later
 */
lv_theme_t * lv_theme_mono_init(lv_color_t color_primary, lv_color_t color_secondary, uint32_t flags,
                                const lv_font_t * font_small, const lv_font_t * font_normal, const lv_font_t * font_subtitle,
                                const lv_font_t * font_title);
/**********************
 *      MACROS
 **********************/

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_THEME_TEMPLATE_H*/
