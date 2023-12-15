/**
* @file _lv_cache_lru_rb.h
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
#include "_lv_cache_internal.h"
#include "lv_cache_entry.h"

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
LV_ATTRIBUTE_EXTERN_DATA extern const lv_cache_class_t lv_lru_rb_class;
/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_CACHE_LRU_RB_H*/
