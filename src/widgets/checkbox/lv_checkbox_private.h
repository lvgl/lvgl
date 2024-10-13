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

#include "lv_checkbox.h"

#if LV_USE_CHECKBOX != 0
#include "../../core/lv_obj_private.h"

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
