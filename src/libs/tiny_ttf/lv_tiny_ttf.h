/**
 * @file lv_tiny_ttf.h
 *
 */

#ifndef LV_TINY_TTF_H
#define LV_TINY_TTF_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_TINY_TTF

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

#if LV_TINY_TTF_FILE_SUPPORT != 0
/**
 * Create a font from the specified file or path with the specified line height.
 * @param path        the path or file name of the font
 * @param font_size   the font size in pixel
 * @return a font object
 */
lv_font_t * lv_tiny_ttf_create_file(const char * path, int32_t font_size);

/**
 * Create a font from the specified file or path with the specified line height with the specified cache size.
 * @param path        the path or file name of the font
 * @param font_size   the font size in pixel
 * @param kerning     kerning value in pixel
 * @param cache_size  the cache size in count
 * @return a font object
 */
lv_font_t * lv_tiny_ttf_create_file_ex(const char * path, int32_t font_size, lv_font_kerning_t kerning,
                                       size_t cache_size);
#endif

/**
 * Create a font from the specified data pointer with the specified line height.
 * @param data        the data pointer
 * @param data_size   the data size
 * @param font_size   the font size in pixel
 * @return a font object
 */
lv_font_t * lv_tiny_ttf_create_data(const void * data, size_t data_size, int32_t font_size);

/**
 * Create a font from the specified data pointer with the specified line height and the specified cache size.
 * @param data        the data pointer
 * @param data_size   the data size
 * @param font_size   the font size in pixel
 * @param kerning     kerning value in pixel
 * @param cache_size  the cache size in count
 * @return
 */
lv_font_t * lv_tiny_ttf_create_data_ex(const void * data, size_t data_size, int32_t font_size,
                                       lv_font_kerning_t kerning, size_t cache_size);

/**
 * Set the size of the font to a new font_size
 * @note the font bitmap cache and glyph cache will be flushed.
 * @param font        the font object
 * @param font_size   the font size in pixel
 */
void lv_tiny_ttf_set_size(lv_font_t * font, int32_t font_size);

/**
 * Destroy a font previously created with lv_tiny_ttf_create_xxxx()
 * @param font        the font object
 */
void lv_tiny_ttf_destroy(lv_font_t * font);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_TINY_TTF*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TINY_TTF_H*/
