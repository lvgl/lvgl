/**
 * @file lv_font_loader.h
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
 * @param font_name filename where the font file is located
 * @return a pointer to the font or NULL in case of error
 */
lv_font_t * lv_font_load(const char * fontName);


#if LV_USE_FS_MEMFS
/**
 * Loads a `lv_font_t` object from a memory buffer containing the binary font file.
 * Requires LV_USE_FS_MEMFS
 * @param buffer address of the font file in the memory
 * @param size size of the font file buffer
 * @return a pointer to the font or NULL in case of error
 */
lv_font_t * lv_font_load_from_buffer(void * buffer, uint32_t size);
#endif

/**
 * Frees the memory allocated by the `lv_font_load()` function
 * @param font lv_font_t object created by the lv_font_load function
 */
void lv_font_free(lv_font_t * font);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_FONT_LOADER_H*/
