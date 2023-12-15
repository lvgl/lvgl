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
#include "_lv_cache_internal.h"
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
int32_t  lv_cache_entry_ref_get(lv_cache_entry_t_ * entry);
uint32_t lv_cache_entry_get_generation(lv_cache_entry_t_ * entry);
uint32_t lv_cache_entry_get_node_size(lv_cache_entry_t_ * entry);
bool     lv_cache_entry_is_invalid(lv_cache_entry_t_ * entry);
void  *  lv_cache_entry_acquire_data(lv_cache_entry_t_ * entry);
void     lv_cache_entry_release_data(lv_cache_entry_t_ * entry, void * user_data);
const lv_cache_t_ * lv_cache_entry_get_cache(const lv_cache_entry_t_ * entry);
lv_cache_entry_t_ * lv_cache_entry_get_entry(void * data, const uint32_t node_size);

lv_cache_entry_t_ * lv_cache_entry_alloc(const uint32_t node_size);
void lv_cache_entry_free(lv_cache_entry_t_ * entry);
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
