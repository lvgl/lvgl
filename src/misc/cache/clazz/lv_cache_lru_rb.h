/**
* @file lv_cache_lru_rb.h
*
*/

#ifndef LV_CACHE_LRU_RB_H
#define LV_CACHE_LRU_RB_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../lv_cache_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/*************************
 *    GLOBAL VARIABLES
 *************************/
LV_ATTRIBUTE_EXTERN_DATA extern const lv_cache_class_t lv_cache_class_lru_rb_count;
LV_ATTRIBUTE_EXTERN_DATA extern const lv_cache_class_t lv_cache_class_lru_rb_size;
/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_CACHE_LRU_RB_H*/
