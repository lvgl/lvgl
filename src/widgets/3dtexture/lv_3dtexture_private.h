/**
 * @file lv_3dtexture_private.h
 *
 */

#ifndef LV_3DTEXTURE_PRIVATE_H
#define LV_3DTEXTURE_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_3dtexture.h"
#if LV_USE_3DTEXTURE

#include "../../core/lv_obj_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Data of 3dtexture*/
struct _lv_3dtexture_t {
    lv_obj_t obj;
    lv_3dtexture_id_t id;
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_3DTEXTURE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_3DTEXTURE_PRIVATE_H*/
