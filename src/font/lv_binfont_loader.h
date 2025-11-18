/**
 * @file lv_binfont_loader.h
 *
 */

#ifndef LV_BINFONT_LOADER_H
#define LV_BINFONT_LOADER_H

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


typedef struct {
    uint32_t font_size; /**< Size of the font in pixels*/
    const char * path; /**< Path to font file*/
    const void * buffer; /**< Address of the font file in the memory*/
    uint32_t buffer_size; /**< Size of the font file buffer*/
} lv_binfont_font_src_t;

LV_ATTRIBUTE_EXTERN_DATA extern const lv_font_class_t lv_binfont_font_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Loads a `lv_font_t` object from a binary font file
 * @param path   path to font file
 * @return  pointer to font where to load
 */
lv_font_t * lv_binfont_create(const char * path);

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

#endif /* LV_BINFONT_LOADER_H */
