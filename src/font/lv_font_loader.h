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

lv_font_t * lv_font_load(const char * fontName);
lv_font_t* lv_font_load_from_buffer(const void* buffer, uint32_t size);
void lv_font_free(lv_font_t * font);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_FONT_LOADER_H*/
