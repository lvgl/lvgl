/**
 * @file lv_mem_dcache.h
 *
 */

#ifndef LV_MEM_DCACHE_H
#define LV_MEM_DCACHE_H

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

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Clean all data in the data cache.
 */
void lv_mem_clean_dcache_all(void);

/**
 * Clean data in the data cache.
 * @param start start address of the data to clean
 * @param end end address of the data to clean
 */
void lv_mem_clean_dcache(lv_uintptr_t start, lv_uintptr_t end);

/**
 * Flush all data in the data cache.
 */
void lv_mem_flush_dcache_all(void);

/**
 * Flush data in the data cache.
 * @param start start address of the data to flush
 * @param end end address of the data to flush
 */
void lv_mem_flush_dcache(lv_uintptr_t start, lv_uintptr_t end);

/**
 * Invalidate all data in the data cache.
 */
void lv_mem_invalidate_dcache_all(void);

/**
 * Invalidate data in the data cache.
 * @param start start address of the data to invalidate
 * @param end end address of the data to invalidate
 */
void lv_mem_invalidate_dcache(lv_uintptr_t start, lv_uintptr_t end);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_MEM_DCACHE_H*/
