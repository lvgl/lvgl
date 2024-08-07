/**
 * @file lv_tileview_private.h
 *
 */

#ifndef LV_TILEVIEW_PRIVATE_H
#define LV_TILEVIEW_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../core/lv_obj_private.h"
#include "lv_tileview.h"

#if LV_USE_TILEVIEW

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *      TYPEDEFS
 **********************/
struct lv_tileview_t {
    lv_obj_t obj;
    lv_obj_t * tile_act;
};

struct lv_tileview_tile_t {
    lv_obj_t obj;
    lv_dir_t dir;
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_TILEVIEW */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TILEVIEW_PRIVATE_H*/
