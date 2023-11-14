/**
 * @file lv_binfont_loader.h
 *
 */

#ifndef LV_FONT_LOADER_H
#define LV_FONT_LOADER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

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
 * Loads a `lv_font_t` object from a binary font file
 * @param font          pointer to font where to load
 * @param path          path where the font file is located
 * @return              LV_RESULT_OK on success; LV_RESULT_INVALID on error
 */
lv_result_t lv_binfont_load(lv_font_t * font, const char * font_name);

#if LV_USE_FS_MEMFS
/**
 * Loads a `lv_font_t` object from a memory buffer containing the binary font file.
 * Requires LV_USE_FS_MEMFS
 * @param font          pointer to font where to load
 * @param buffer        address of the font file in the memory
 * @param size          size of the font file buffer
 * @return              LV_RESULT_OK on success; LV_RESULT_INVALID on error
 */
lv_result_t lv_binfont_load_from_buffer(lv_font_t * font, void * buffer, uint32_t size);
#endif

/**
 * Frees the memory allocated by the `lv_binfont_load()` function
 * @param font          lv_font_t object created by the lv_binfont_load function
 */
void lv_font_free(lv_font_t * font);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_FONT_LOADER_H*/
