/**
 * @file lv_style_private.h
 *
 */

#ifndef LV_STYLE_PRIVATE_H
#define LV_STYLE_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lvgl_public.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

static inline bool lv_style_is_const_internal(const lv_style_t * style)
{
    LV_ASSERT(style != NULL);
    return style->prop_cnt == 255;
}

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_STYLE_PRIVATE_H*/
