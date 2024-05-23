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
 * @param path          path where the font file is located
 * @return              pointer to font where to load
 */
lv_font_t * lv_binfont_create(const char * font_name);

#if LV_USE_FS_MEMFS
/**
 * Loads a `lv_font_t` object from a memory buffer containing the binary font file.
 * Requires LV_USE_FS_MEMFS
 * @param buffer        address of the font file in the memory
 * @param size          size of the font file buffer
 * @return              pointer to font where to load
 */
lv_font_t * lv_binfont_create_from_buffer(void * buffer, uint32_t size);
#endif

/**
 * Frees the memory allocated by the `lv_binfont_create()` function
 * @param font          lv_font_t object created by the lv_binfont_create function
 */
void lv_binfont_destroy(lv_font_t * font);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_FONT_LOADER_H*/
