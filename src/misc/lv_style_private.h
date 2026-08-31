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

static inline lv_style_res_t lv_style_get_prop_internal(const lv_style_t * style, lv_style_prop_t prop,
                                                        lv_style_value_t * value)
{
    LV_ASSERT(style != NULL);
    LV_ASSERT(value != NULL);
    if(lv_style_is_const_internal(style)) {
        lv_style_const_prop_t * props = (lv_style_const_prop_t *)style->values_and_props;
        uint32_t i;
        for(i = 0; props[i].prop != LV_STYLE_PROP_INV; i++) {
            if(props[i].prop == prop) {
                *value = props[i].value;
                return LV_STYLE_RES_FOUND;
            }
        }
    }
    else {
        lv_style_prop_t * props = (lv_style_prop_t *)style->values_and_props + style->prop_cnt * sizeof(lv_style_value_t);
        uint32_t i;
        for(i = 0; i < style->prop_cnt; i++) {
            if(props[i] == prop) {
                lv_style_value_t * values = (lv_style_value_t *)style->values_and_props;
                *value = values[i];
                return LV_STYLE_RES_FOUND;
            }
        }
    }
    return LV_STYLE_RES_NOT_FOUND;
}

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_STYLE_PRIVATE_H*/
