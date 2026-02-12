/**
 * @file lvgl_private.h
 * This file exists only to be compatible with Arduino's library structure
 */

#ifndef LVGL_PRIVATE_SRC_H
#define LVGL_PRIVATE_SRC_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lvgl_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

#if LV_USE_EXT_DATA
typedef struct {
    void * data;
    void (* free_cb)(void * data);
} lv_ext_data_t;
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LVGL_PRIVATE_SRC_H */
