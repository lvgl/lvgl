/**
* @file lv_cache_private.h
*
*/

#ifndef LV_CACHE_PRIVATE_H
#define LV_CACHE_PRIVATE_H

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

/*-----------------
 * Cache entry slot
 *----------------*/
struct _lv_cache_ops_t;
struct _lv_cache_t;
struct _lv_cache_class_t;
struct _lv_cache_entry_t;

typedef struct _lv_cache_ops_t lv_cache_ops_t;
typedef struct _lv_cache_t lv_cache_t;
typedef struct _lv_cache_class_t lv_cache_class_t;
typedef struct _lv_cache_entry_t lv_cache_entry_t;

typedef int8_t lv_cache_compare_res_t;
typedef bool (*lv_cache_create_cb_t)(void * node, void * user_data);
typedef void (*lv_cache_free_cb_t)(void * node, void * user_data);
typedef lv_cache_compare_res_t (*lv_cache_compare_cb_t)(const void * a, const void * b);

typedef void * (*lv_cache_alloc_cb_t)(void);
typedef bool (*lv_cache_init_cb_t)(lv_cache_t * cache);
typedef void (*lv_cache_destroy_cb_t)(lv_cache_t * cache, void * user_data);
typedef lv_cache_entry_t * (*lv_cache_get_cb_t)(lv_cache_t * cache, const void * key, void * user_data);
typedef lv_cache_entry_t * (*lv_cache_add_cb_t)(lv_cache_t * cache, const void * key, void * user_data);
typedef void (*lv_cache_remove_cb_t)(lv_cache_t * cache, lv_cache_entry_t * entry, void * user_data);
typedef void (*lv_cache_drop_cb_t)(lv_cache_t * cache, const void * key, void * user_data);
typedef void (*lv_cache_clear_cb_t)(lv_cache_t * cache, void * user_data);

struct _lv_cache_ops_t {
    lv_cache_compare_cb_t compare_cb;
    lv_cache_create_cb_t create_cb;
    lv_cache_free_cb_t free_cb;
};

struct _lv_cache_t {
    const lv_cache_class_t * clz;

    size_t node_size;

    size_t max_size;
    size_t size;

    lv_cache_ops_t ops;

    lv_mutex_t lock;
};

struct _lv_cache_class_t {
    lv_cache_alloc_cb_t alloc_cb;
    lv_cache_init_cb_t init_cb;
    lv_cache_destroy_cb_t destroy_cb;

    lv_cache_get_cb_t get_cb;
    lv_cache_add_cb_t add_cb;
    lv_cache_remove_cb_t remove_cb;
    lv_cache_drop_cb_t drop_cb;
    lv_cache_clear_cb_t drop_all_cb;
};

/*-----------------
 * Cache entry slot
 *----------------*/

struct _lv_cache_slot_size_t;

typedef struct _lv_cache_slot_size_t lv_cache_slot_size_t;

struct _lv_cache_slot_size_t {
    size_t size;
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

#endif /*LV_CACHE_PRIVATE_H*/
