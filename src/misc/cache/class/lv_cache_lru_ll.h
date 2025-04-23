/**
* @file lv_cache_lru_ll.h
*
*/

#ifndef LV_CACHE_LRU_LL_H
#define LV_CACHE_LRU_LL_H

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
LV_ATTRIBUTE_EXTERN_DATA extern const lv_cache_class_t lv_cache_class_lru_ll_count;
LV_ATTRIBUTE_EXTERN_DATA extern const lv_cache_class_t lv_cache_class_lru_ll_size;
/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_CACHE_LRU_LL_H*/
