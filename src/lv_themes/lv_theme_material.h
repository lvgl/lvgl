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

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_THEME_MATERIAL_PALETTE_LIGHT,
    LV_THEME_MATERIAL_PALETTE_DARK,
} lv_theme_material_palette_t;


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
                                    lv_font_t * font_small, lv_font_t * font_normal, lv_font_t * font_subtitle, lv_font_t * font_title);
/**********************
 *      MACROS
 **********************/

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_THEME_MATERIAL_H*/
