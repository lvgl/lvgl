/**
 * @file lv_color_op_private.h
 *
 */

#ifndef LV_COLOR_OP_PRIVATE_H
#define LV_COLOR_OP_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_color_op.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct lv_color_filter_dsc_t {
    lv_color_filter_cb_t filter_cb;
    void * user_data;
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_COLOR_OP_PRIVATE_H*/
