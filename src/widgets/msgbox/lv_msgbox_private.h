/**
 * @file lv_msgbox_private.h
 *
 */

#ifndef LV_MSGBOX_PRIVATE_H
#define LV_MSGBOX_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_msgbox.h"

#if LV_USE_MSGBOX
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

struct _lv_msgbox_t {
    lv_obj_t obj;
    lv_obj_t * header;
    lv_obj_t * content;
    lv_obj_t * footer;
    lv_obj_t * title;
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_MSGBOX */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_MSGBOX_PRIVATE_H*/
