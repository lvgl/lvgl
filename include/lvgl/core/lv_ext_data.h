/**
 * @file lv_ext_data.h
 *
 */

#ifndef LV_EXT_DATA_H
#define LV_EXT_DATA_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../config/lv_conf_internal.h"
#if LV_USE_EXT_DATA

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    void * data;
    void (* free_cb)(void * data);
} lv_ext_data_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_EXT_DATA*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_EXT_DATA_H */
