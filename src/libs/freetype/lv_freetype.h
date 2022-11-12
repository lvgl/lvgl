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

/**
 * Initialize the freetype library.
 * @param max_faces Maximum number of opened FT_Face objects managed by this cache instance. Use 0 for defaults.
 * @param max_sizes Maximum number of opened FT_Size objects managed by this cache instance. Use 0 for defaults.
 * @param max_bytes Maximum number of bytes to use for cached data nodes. Use 0 for defaults.
 *                  Note that this value does not account for managed FT_Face and FT_Size objects.
 * @return LV_RES_OK on success, otherwise LV_RES_INV.
 */
lv_res_t lv_freetype_init(uint16_t max_faces, uint16_t max_sizes, uint32_t max_bytes);

/**
 * Uninitialize the freetype library
 */
void lv_freetype_uninit(void);

/**
 * Create a freetype font.
 * @param pathname font file path.
 * @param size font size.
 * @param style font style(see lv_freetype_font_style_t for details).
 * @return Created font, or NULL on failure.
 */
lv_font_t * lv_freetype_font_create(const char * pathname, uint16_t size, uint16_t style);

/**
 * Delete a freetype font.
 * @param font freetype font to be deleted.
 */
void lv_freetype_font_del(lv_font_t * font);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_FREETYPE*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_FREETYPE_H */
