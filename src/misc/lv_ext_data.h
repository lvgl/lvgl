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
#include "../lv_conf_internal.h"

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

#endif /* LV_EXT_DATA_H */
