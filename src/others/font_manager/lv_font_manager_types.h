/**
 * @file lv_font_manager_types.h
 *
 */
#ifndef LV_FONT_MANAGER_TYPES_H
#define LV_FONT_MANAGER_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../misc/lv_types.h"

#if LV_USE_FONT_MANAGER

#include "../../font/lv_font.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_font_info_t;

typedef struct _lv_font_class_t {
    lv_font_t * (*create_cb)(const struct _lv_font_info_t * info, const void * src);
    void (*delete_cb)(lv_font_t * font);
    void * (*dup_src_cb)(const void * src);
    void (*free_src_cb)(void * src);
} lv_font_class_t;

typedef struct _lv_font_info_t {
    const char * name;
    const lv_font_class_t * class_p;
    uint32_t size;
    uint32_t render_mode;
    uint32_t style;
    lv_font_kerning_t kerning;
} lv_font_info_t;

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

#endif /* LV_FONT_MANAGER_TYPES_H */
