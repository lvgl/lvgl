/**
 * @file lv_freetype.h
 *
 */
#ifndef LV_FREETYPE_H
#define LV_FREETYPE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl.h"
#if LV_USE_FREETYPE

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    LV_FREETYPE_FONT_STYLE_NORMAL = 0,
    LV_FREETYPE_FONT_STYLE_ITALIC = 1 << 0,
    LV_FREETYPE_FONT_STYLE_BOLD   = 1 << 1
} lv_freetype_font_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_res_t lv_freetype_init(uint16_t max_faces, uint16_t max_sizes, uint32_t max_bytes);

void lv_freetype_uninit(void);

lv_font_t * lv_freetype_font_create(const char * pathname, uint16_t weight, lv_freetype_font_style_t style);

void lv_freetype_font_del(lv_font_t * font);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_FREETYPE*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_FREETYPE_H */
