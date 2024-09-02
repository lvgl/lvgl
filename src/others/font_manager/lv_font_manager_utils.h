/**
 * @file lv_font_manager_utils.h
 *
 */
#ifndef LV_FONT_MANAGER_UTILS_H
#define LV_FONT_MANAGER_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../misc/lv_types.h"

#if LV_USE_FONT_MANAGER

#include "../../libs/freetype/lv_freetype.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    const char * name;
    lv_freetype_font_render_mode_t render_mode;
    lv_freetype_font_style_t style;
    uint32_t size;
} lv_freetype_info_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Compare font information.
 * @param ft_info_1 font information 1.
 * @param ft_info_2 font information 2.
 * @return return true if the fonts are equal.
 */
bool lv_freetype_info_is_equal(const lv_freetype_info_t * ft_info_1, const lv_freetype_info_t * ft_info_2);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_FONT_MANAGER*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_FONT_MANAGER_UTILS_H */
