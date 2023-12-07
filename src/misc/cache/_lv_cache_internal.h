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

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
struct _lv_cache_t_;
struct _lv_cache_class_t;

typedef struct _lv_cache_t_ lv_cache_t_;
typedef struct _lv_cache_class_t lv_cache_class_t;

typedef int8_t lv_cache_compare_res_t;
typedef bool (*lv_cache_create_cb_t)(void * node, void * user_data);
typedef void (*lv_cache_free_cb_t)(void * node, void * user_data);
typedef lv_cache_compare_res_t (*lv_cache_compare_cb_t)(const void * a, const void * b);

typedef void * (*lv_cache_alloc_cb_t)(void);
typedef bool (*lv_cache_init_cb_t)(lv_cache_t_ * cache);
typedef void (*lv_cache_destroy_cb_t)(lv_cache_t_ * cache, void * user_data);
typedef void * (*lv_cache_get_or_create_cb_t)(lv_cache_t_ * cache, const void * key, void * user_data);
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
};

struct _lv_cache_class_t {
    lv_cache_alloc_cb_t alloc_cb;
    lv_cache_init_cb_t init_cb;
    lv_cache_destroy_cb_t destroy_cb;

    lv_cache_get_or_create_cb_t get_or_create_cb;
    lv_cache_drop_cb_t drop_cb;
    lv_cache_clear_cb_t drop_all_cb;
};
/**********************
 * GLOBAL PROTOTYPES
 **********************/

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
