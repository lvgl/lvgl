/**
 * @file lv_nuttx_image_cache.h
 *
 */

#ifndef LV_NUTTX_IMAGE_CACHE_H
#define LV_NUTTX_IMAGE_CACHE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_CACHE_DEF_SIZE > 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_nuttx_image_cache_init(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_CACHE_DEF_SIZE > 0*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_NUTTX_IMAGE_CACHE_H*/
