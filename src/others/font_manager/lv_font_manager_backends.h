/**
 * @file lv_font_manager_backends.h
 *
 */
#ifndef LV_FONT_MANAGER_BACKENDS_H
#define LV_FONT_MANAGER_BACKENDS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_font_manager_types.h"

#if LV_USE_FONT_MANAGER

/*********************
 *      DEFINES
 *********************/

#define LV_BUILTIN_FONT_SRC_END {NULL, 0}
#define LV_TINY_TTF_FONT_SRC_END {NULL, NULL, 0, 0}

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    const lv_font_t * font_p;
    uint32_t size;
} lv_builtin_font_src_t;

LV_ATTRIBUTE_EXTERN_DATA extern const lv_font_class_t lv_builtin_font_class;

typedef const char lv_freetype_font_src_t;

#if LV_USE_FREETYPE
LV_ATTRIBUTE_EXTERN_DATA extern const lv_font_class_t lv_freetype_font_class;
#endif /*LV_USE_FREETYPE*/

typedef struct {
    const char * path;
    const void * data;
    size_t data_size;
    size_t cache_size;
} lv_tiny_ttf_font_src_t;

#if LV_USE_TINY_TTF
LV_ATTRIBUTE_EXTERN_DATA extern const lv_font_class_t lv_tiny_ttf_font_class;
#endif /*LV_USE_TINY_TTF*/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_FONT_MANAGER*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_FONT_MANAGER_BACKENDS_H */
