/**
* @file _lv_cache_internal.h
*
*/

#ifndef _LV_CACHE_INTERNAL_H
#define _LV_CACHE_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_types.h"
#include <stdbool.h>
#include <stdlib.h>
#include "../../osal/lv_os.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
struct _lv_cache_t_;
struct _lv_cache_class_t;
struct _lv_cache_entry_t_;

typedef struct _lv_cache_t_ lv_cache_t_;
typedef struct _lv_cache_class_t lv_cache_class_t;
typedef struct _lv_cache_entry_t_ lv_cache_entry_t_;

typedef int8_t lv_cache_compare_res_t;
typedef bool (*lv_cache_create_cb_t)(void * node, void * user_data);
typedef void (*lv_cache_free_cb_t)(void * node, void * user_data);
typedef lv_cache_compare_res_t (*lv_cache_compare_cb_t)(const void * a, const void * b);

typedef void * (*lv_cache_alloc_cb_t)(void);
typedef bool (*lv_cache_init_cb_t)(lv_cache_t_ * cache);
typedef void (*lv_cache_destroy_cb_t)(lv_cache_t_ * cache, void * user_data);
typedef lv_cache_entry_t_ * (*lv_cache_get_cb_t)(lv_cache_t_ * cache, const void * key, void * user_data);
typedef lv_cache_entry_t_ * (*lv_cache_create_entry_cb_t)(lv_cache_t_ * cache, const void * key, void * user_data);
typedef void (*lv_cache_remove_cb_t)(lv_cache_t_ * cache, lv_cache_entry_t_ * entry, void * user_data);
typedef void (*lv_cache_drop_cb_t)(lv_cache_t_ * cache, const void * key, void * user_data);
typedef void (*lv_cache_clear_cb_t)(lv_cache_t_ * cache, void * user_data);

struct _lv_cache_t_ {
    const lv_cache_class_t * clz;

    size_t node_size;

    size_t max_size;
    size_t size;

    lv_cache_compare_cb_t compare_cb;
    lv_cache_create_cb_t create_cb;
    lv_cache_free_cb_t free_cb;

    lv_mutex_t lock;
};

struct _lv_cache_class_t {
    lv_cache_alloc_cb_t alloc_cb;
    lv_cache_init_cb_t init_cb;
    lv_cache_destroy_cb_t destroy_cb;

    lv_cache_get_cb_t get_cb;
    lv_cache_create_entry_cb_t create_entry_cb;
    lv_cache_remove_cb_t remove_cb;
    lv_cache_drop_cb_t drop_cb;
    lv_cache_clear_cb_t drop_all_cb;
};
/**********************
 * GLOBAL PROTOTYPES
 **********************/
void     lv_cache_entry_ref_reset(lv_cache_entry_t_ * entry);
void     lv_cache_entry_ref_inc(lv_cache_entry_t_ * entry);
void     lv_cache_entry_ref_dec(lv_cache_entry_t_ * entry);
int32_t  lv_cache_entry_ref_get(lv_cache_entry_t_ * entry);
uint32_t lv_cache_entry_get_generation(lv_cache_entry_t_ * entry);
void     lv_cache_entry_inc_generation(lv_cache_entry_t_ * entry);
void     lv_cache_entry_set_generation(lv_cache_entry_t_ * entry, uint32_t generation);
uint32_t lv_cache_entry_get_node_size(lv_cache_entry_t_ * entry);
void     lv_cache_entry_set_node_size(lv_cache_entry_t_ * entry, uint32_t node_size);
void     lv_cache_entry_set_invalid(lv_cache_entry_t_ * entry, bool is_invalid);
void     lv_cache_entry_set_cache(lv_cache_entry_t_ * entry, const lv_cache_t_ * cache);
const lv_cache_t_ * lv_cache_entry_get_cache(const lv_cache_entry_t_ * entry);
bool     lv_cache_entry_is_invalid(lv_cache_entry_t_ * entry);
void  *  lv_cache_entry_get_data(lv_cache_entry_t_ * entry);

void  *  lv_cache_entry_acquire_data(lv_cache_entry_t_ * entry);
void     lv_cache_entry_release_data(lv_cache_entry_t_ * entry, void * user_data);
lv_cache_entry_t_ * lv_cache_entry_get_entry(void * data, const uint32_t node_size);

uint32_t lv_cache_entry_get_size(const uint32_t node_size);
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

#endif /*_LV_CACHE_INTERNAL_H*/
