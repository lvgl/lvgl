/**
 * @file lv_mem_private.h
 *
 */

#ifndef LV_MEM_PRIVATE_H
#define LV_MEM_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_mem.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Heap information structure.
 */
struct lv_mem_monitor_t {
    size_t total_size; /**< Total heap size*/
    size_t free_cnt;
    size_t free_size; /**< Size of available memory*/
    size_t free_biggest_size;
    size_t used_cnt;
    size_t max_used; /**< Max size of Heap memory used*/
    uint8_t used_pct; /**< Percentage used*/
    uint8_t frag_pct; /**< Amount of fragmentation*/
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_MEM_PRIVATE_H*/
