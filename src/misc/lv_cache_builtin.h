/**
 * @file lv_cache_builtin.h
 *
 */

#ifndef LV_CACHE_BUILTIN_H
#define LV_CACHE_BUILTIN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_ll.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    uint32_t cur_size;
    lv_ll_t entry_ll;
} lv_cache_builtin_dsc_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void _lv_cache_builtin_init(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_CACHE_BUILTIN_H*/
