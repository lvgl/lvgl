/**
* @file lv_cache_entry.h
*
 */

#ifndef LV_CACHE_ENTRY_H
#define LV_CACHE_ENTRY_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../osal/lv_os.h"
#include "../lv_types.h"
#include "lv_cache_private.h"
#include <stdbool.h>
#include <stdlib.h>
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
uint32_t lv_cache_entry_get_size(const uint32_t node_size);
int32_t  lv_cache_entry_get_ref(lv_cache_entry_t * entry);
uint32_t lv_cache_entry_get_node_size(lv_cache_entry_t * entry);
bool     lv_cache_entry_is_invalid(lv_cache_entry_t * entry);
void  *  lv_cache_entry_get_data(lv_cache_entry_t * entry);
const lv_cache_t * lv_cache_entry_get_cache(const lv_cache_entry_t * entry);
lv_cache_entry_t * lv_cache_entry_get_entry(void * data, const uint32_t node_size);

lv_cache_entry_t * lv_cache_entry_alloc(const uint32_t node_size, const lv_cache_t * cache);
void lv_cache_entry_init(lv_cache_entry_t * entry, const lv_cache_t * cache, const uint32_t node_size);
void lv_cache_entry_delete(lv_cache_entry_t * entry);
/*************************
 *    GLOBAL VARIABLES
 *************************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_CACHE_ENTRY_H*/
