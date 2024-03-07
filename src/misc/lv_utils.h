/**
 * @file lv_utils.h
 *
 */

#ifndef LV_UTILS_H
#define LV_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_types.h"
#include "../draw/lv_draw_buf.h"

#include <stdint.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/** Searches base[0] to base[n - 1] for an item that matches *key.
 *
 * @note The function cmp must return negative if it's first
 *  argument (the search key) is less that it's second (a table entry),
 *  zero if equal, and positive if greater.
 *
 *  @note Items in the array must be in ascending order.
 *
 * @param key    Pointer to item being searched for
 * @param base   Pointer to first element to search
 * @param n      Number of elements
 * @param size   Size of each element
 * @param cmp    Pointer to comparison function (see #unicode_list_compare as a comparison function
 * example)
 *
 * @return a pointer to a matching item, or NULL if none exists.
 */
void * _lv_utils_bsearch(const void * key, const void * base, uint32_t n, uint32_t size,
                         int32_t (*cmp)(const void * pRef, const void * pElement));

/**
 * Save a draw buf to a file
 * @param draw_buf  pointer to a draw buffer
 * @param path      path to the file to save
 * @return          LV_RES_OK: success; LV_RES_INV: error
 */
lv_result_t lv_draw_buf_save_to_file(const lv_draw_buf_t * draw_buf, const char * path);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
