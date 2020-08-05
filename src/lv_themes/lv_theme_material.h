/**
 * @file lv_theme_material.h
 *
 */

#ifndef LV_THEME_MATERIAL_H
#define LV_THEME_MATERIAL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#if LV_USE_THEME_MATERIAL

/*********************
 *      DEFINES
 *********************/
typedef enum {
    LV_THEME_MATERIAL_FLAG_DARK =  0x01,
    LV_THEME_MATERIAL_FLAG_LIGHT = 0x02,
    LV_THEME_MATERIAL_FLAG_NO_TRANSITION  = 0x10,
    LV_THEME_MATERIAL_FLAG_NO_FOCUS  = 0x20,
} lv_theme_material_flag_t;

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the default
 * @param color_primary the primary color of the theme
 * @param color_secondary the secondary color for the theme
 * @param flags ORed flags starting with `LV_THEME_DEF_FLAG_...`
 * @param font_small pointer to a small font
 * @param font_normal pointer to a normal font
 * @param font_subtitle pointer to a large font
 * @param font_title pointer to a extra large font
 * @return a pointer to reference this theme later
 */
lv_theme_t * lv_theme_material_init(lv_color_t color_primary, lv_color_t color_secondary, uint32_t flags,
                                    const lv_font_t * font_small, const lv_font_t * font_normal, const lv_font_t * font_subtitle,
                                    const lv_font_t * font_title);
/**********************
 *      MACROS
 **********************/

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_THEME_MATERIAL_H*/
