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

/**
 * Create an iterator based on an instance, and then the next element of the iterator can be obtained through lv_iter_next,
 * In order to obtain the next operation in a unified and abstract way.
 * @param instance       The instance to be iterated
 * @param elem_size      The size of the element to be iterated in bytes
 * @param context_size   The size of the context to be passed to the next_cb in bytes
 * @param next_cb        The callback function to get the next element
 * @return               The iterator object
 */
lv_iter_t * lv_iter_create(void * instance, uint32_t elem_size, uint32_t context_size, lv_iter_next_cb next_cb);

/**
 * Get the context of the iterator. You can use it to store some temporary variables associated with current iterator..
 * @param iter           `lv_iter_t` object create before
 * @return the iter context
 */
void * lv_iter_get_context(const lv_iter_t * iter);

/**
 * Destroy the iterator object, and release the context. Other resources allocated by the user are not released.
 * The user needs to release it by itself.
 * @param iter          `lv_iter_t` object create before
 */
void lv_iter_destroy(lv_iter_t * iter);

/**
 * Get the next element of the iterator.
 * @param iter          `lv_iter_t` object create before
 * @param elem          The pointer to store the next element
 * @return              LV_RESULT_OK: Get the next element successfully
 *                      LV_RESULT_INVALID: The next element is invalid
 */
lv_result_t lv_iter_next(lv_iter_t * iter, void * elem);

/**
 * Make the iterator peekable, which means that the user can peek the next element without advancing the iterator.
 * @param iter          `lv_iter_t` object create before
 * @param capacity      The capacity of the peek buffer
 */
void lv_iter_make_peekable(lv_iter_t * iter, uint32_t capacity);

/**
 * Peek the next element of the iterator without advancing the iterator.
 * @param iter          `lv_iter_t` object create before
 * @param elem          The pointer to store the next element
 * @return              LV_RESULT_OK: Peek the next element successfully
 *                      LV_RESULT_INVALID: The next element is invalid
 */
lv_result_t lv_iter_peek(lv_iter_t * iter, void * elem);

/**
 * Only advance the iterator without getting the next element.
 * @param iter          `lv_iter_t` object create before
 * @return              LV_RESULT_OK: Peek the next element successfully
 *                      LV_RESULT_INVALID: The next element is invalid
 */
lv_result_t lv_iter_peek_advance(lv_iter_t * iter);

/**
 * Reset the peek cursor to the `next` cursor.
 * @param iter          `lv_iter_t` object create before
 * @return              LV_RESULT_OK: Reset the peek buffer successfully
 *                      LV_RESULT_INVALID: The peek buffer is invalid
 */
lv_result_t lv_iter_peek_reset(lv_iter_t * iter);

/**
 * Inspect the element of the iterator. The callback function will be called for each element of the iterator.
 * @param iter          `lv_iter_t` object create before
 * @param inspect_cb    The callback function to inspect the element
 */
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
