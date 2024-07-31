/**
* @file lv_iter.h
*
 */


#ifndef LV_ITER_H
#define LV_ITER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_types.h"
#include "lv_assert.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef lv_result_t (*lv_iter_next_cb)(void * instance, void * context, void * elem);
typedef void (*lv_iter_inspect_cb)(void * elem);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_iter_t * lv_iter_create(void * instance, uint32_t elem_size, uint32_t context_size, lv_iter_next_cb next_cb);

void * lv_iter_get_context(lv_iter_t * iter);

void lv_iter_destroy(lv_iter_t * iter);

lv_result_t lv_iter_next(lv_iter_t * iter, void * elem);

void lv_iter_make_peekable(lv_iter_t * iter, uint32_t capacity);

lv_result_t lv_iter_peek(lv_iter_t * iter, void * elem);

lv_result_t lv_iter_peek_advance(lv_iter_t * iter);

lv_result_t lv_iter_peek_reset(lv_iter_t * iter);

void lv_iter_inspect(lv_iter_t * iter, lv_iter_inspect_cb inspect_cb);

/*************************
 *    GLOBAL VARIABLES
 *************************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_ITER_H*/
