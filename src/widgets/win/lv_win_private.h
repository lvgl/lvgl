/**
 * @file lv_win_private.h
 *
 */

#ifndef LV_WIN_PRIVATE_H
#define LV_WIN_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../core/lv_obj_private.h"
#include "lv_win.h"

#if LV_USE_WIN

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *      TYPEDEFS
 **********************/
struct lv_win_t {
    lv_obj_t obj;
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_WIN */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_WIN_PRIVATE_H*/
