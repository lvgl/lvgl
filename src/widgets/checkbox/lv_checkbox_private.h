/**
 * @file lv_checkbox_private.h
 *
 */

#ifndef LV_CHECKBOX_PRIVATE_H
#define LV_CHECKBOX_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../core/lv_obj_private.h"
#include "../../lvgl_public.h"

#if LV_USE_CHECKBOX

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_checkbox_t {
    lv_obj_t obj;
    char * txt;
    uint32_t static_txt : 1;
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_CHECKBOX != 0 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_CHECKBOX_PRIVATE_H*/
