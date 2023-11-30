/**
 * @file lv_lru_rb.h
 *
 */

#ifndef LV_LRU_RB_H
#define LV_LRU_RB_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_types.h"

#include "stdbool.h"
#include "lv_assert.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
struct _lv_lru_rb_t;
typedef struct _lv_lru_rb_t lv_lru_rb_t;

typedef int8_t lv_lru_rb_compare_res_t;
typedef bool (*lv_lru_rb_alloc_cb_t)(void * node, void * user_data);
typedef void (*lv_lru_rb_free_cb_t)(void * node, void * user_data);
typedef lv_lru_rb_compare_res_t (*lv_lru_rb_compare_cb_t)(const void * a, const void * b);

/**********************
 * GLOBAL PROTOTYPES
 **********************/
lv_lru_rb_t * lv_lru_rb_create(size_t node_size, size_t max_size, lv_lru_rb_compare_cb_t compare_cb,
                               lv_lru_rb_alloc_cb_t alloc_cb, lv_lru_rb_free_cb_t free_cb);
void lv_lru_rb_destroy(lv_lru_rb_t * lru, void * user_data);
void * lv_lru_rb_get(lv_lru_rb_t * lru, const void * key, void * user_data);
void lv_lru_rb_reset(lv_lru_rb_t * lru, const void * key, void * user_data);
void lv_lru_rb_clear(lv_lru_rb_t * lru, void * user_data);
void lv_lru_rb_set_max_size(lv_lru_rb_t * lru, size_t max_size, void * user_data);
size_t lv_lru_rb_get_max_size(lv_lru_rb_t * lru, void * user_data);
size_t lv_lru_rb_get_size(lv_lru_rb_t * lru, void * user_data);
size_t lv_lru_rb_get_free_size(lv_lru_rb_t * lru, void * user_data);
void lv_lru_rb_set_compare_cb(lv_lru_rb_t * lru, lv_lru_rb_compare_cb_t compare_cb, void * user_data);
void lv_lru_rb_set_alloc_cb(lv_lru_rb_t * lru, lv_lru_rb_alloc_cb_t alloc_cb, void * user_data);
void lv_lru_rb_set_free_cb(lv_lru_rb_t * lru, lv_lru_rb_free_cb_t free_cb, void * user_data);
/*************************
 *    GLOBAL VARIABLES
 *************************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_LRU_RB_H*/
