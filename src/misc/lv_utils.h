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
 * @note Items in the array must be in ascending order.
 *
 * @param key    Pointer to item being searched for
 * @param base   Pointer to first element to search
 * @param n      Number of elements
 * @param size   Size of each element
 * @param cmp    Pointer to comparison function (see unicode_list_compare()
 *                 as a comparison function example)
 *
 * @return a pointer to a matching item, or NULL if none exists.
 */
void * lv_utils_bsearch(const void * key, const void * base, size_t n, size_t size,
                        int (*cmp)(const void * pRef, const void * pElement));

/**
 * Save a draw buf to a file
 * @param draw_buf  pointer to a draw buffer
 * @param path      path to the file to save
 * @return          LV_RESULT_OK: success; LV_RESULT_INVALID: error
 */
lv_result_t lv_draw_buf_save_to_file(const lv_draw_buf_t * draw_buf, const char * path);

/**
 * Reverse the order of the bytes in a 32-bit value.
 * @param x     a 32-bit value.
 * @return      the value `x` with reversed byte-order.
 */
static inline uint32_t lv_swap_bytes_32(uint32_t x)
{
    return (x << 24)
           | ((x & 0x0000ff00U) <<  8)
           | ((x & 0x00ff0000U) >>  8)
           | (x >> 24);
}

/**
 * Reverse the order of the bytes in a 16-bit value.
 * @param x     a 16-bit value.
 * @return      the value `x` with reversed byte-order.
 */
static inline uint16_t lv_swap_bytes_16(uint16_t x)
{
    return (x << 8) | (x >> 8);
}

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
