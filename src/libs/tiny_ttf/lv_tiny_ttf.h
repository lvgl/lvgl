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

#if LV_TINY_TTF_FILE_SUPPORT !=0
/* create a font from the specified file or path with the specified line height.*/
lv_font_t * lv_tiny_ttf_create_file(const char * path, int32_t font_size);

/* create a font from the specified file or path with the specified line height with the specified cache size.*/
lv_font_t * lv_tiny_ttf_create_file_ex(const char * path, int32_t font_size, size_t cache_size);
#endif

void lv_tiny_ttf_init(void);

void lv_tiny_ttf_deinit(void);

/* create a font from the specified data pointer with the specified line height.*/
lv_font_t * lv_tiny_ttf_create_data(const void * data, size_t data_size, int32_t font_size);

/* create a font from the specified data pointer with the specified line height and the specified cache size.*/
lv_font_t * lv_tiny_ttf_create_data_ex(const void * data, size_t data_size, int32_t font_size, size_t cache_size);

/* set the size of the font to a new font_size*/
void lv_tiny_ttf_set_size(lv_font_t * font, int32_t font_size);

/* destroy a font previously created with lv_tiny_ttf_create_xxxx()*/
void lv_tiny_ttf_destroy(lv_font_t * font);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_TINY_TTF*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TINY_TTF_H*/
